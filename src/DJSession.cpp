#include "DJSession.h"
#include <iostream>
#include <algorithm>
#include <sstream>
#include <dirent.h>

// ========== CONSTRUCTORS & RULE OF 5 ==========


DJSession::DJSession(const std::string& name, bool play_all)
    : session_name(name),
    library_service(),
    controller_service(),
    mixing_service(),
    config_manager(),
    session_config(),
    track_titles(),
    play_all(play_all),
    stats()
      {
    std::cout << "DJ Session System initialized: " << session_name << std::endl;
}


DJSession::~DJSession() {
    std::cout << "Shutting down DJ Session System: " << session_name << std::endl;
}

// ========== CORE FUNCTIONALITY ==========
bool DJSession::load_playlist(const std::string& playlist_name)  {
    std::cout << "[System] Loading playlist: " << playlist_name << "\n";
    
    auto it = session_config.playlists.find(playlist_name);
    if (it == session_config.playlists.end()) {
        std::cerr << "[ERROR] Playlist '" << playlist_name << "' not found in configuration.\n";
        return false;
    }
    
    library_service.loadPlaylistFromIndices(playlist_name, it->second);
    
    if (library_service.getPlaylist().is_empty()) {
        return false;
    }
    
    track_titles = library_service.getTrackTitles();
    return true;
}

int DJSession::load_track_to_controller(const std::string& track_name) {
AudioTrack* track_ptr = library_service.findTrack(track_name);
    if (track_ptr == nullptr){
        std::cerr << "[ERROR] Track:" <<track_name<< "not found in library" << std::endl;
        stats.errors++;
        return 0;
    }
    std::cout << "[System] Loading track '" << track_name << "' to controller..." << std::endl;
    int result_code = controller_service.loadTrackToCache(*track_ptr);
    if (result_code == 1) { 
        stats.cache_hits++;
    } else if (result_code == 0) { 
        stats.cache_misses++;
    } else if (result_code == -1) { 
        stats.cache_misses++;
        stats.cache_evictions++;
    }
    return result_code;
}

bool DJSession::load_track_to_mixer_deck(const std::string& track_title) {
    std::cout << "[System] Delegating track transfer to MixingEngineService for: " << track_title << std::endl;
    AudioTrack* track_ptr = controller_service.getTrackFromCache(track_title);
    if(track_ptr == nullptr){
        std::cerr << "[ERROR] Track:" <<track_title<< "not found in cache" << std::endl;
        stats.errors++;
        return false;
    }
    int return_value = mixing_service.loadTrackToDeck(*track_ptr);
    if(return_value == 0){
        stats.deck_loads_a++;
        stats.transitions++;
    }
    else if(return_value == 1){
        stats.deck_loads_b++;
        stats.transitions++;
    }
    else if(return_value == -1){
        std::cerr << "[ERROR] Failed to load track '" << track_title << "' to mixer deck." << std::endl;
        stats.errors++;
        return false;
    }
    return true;
}

void DJSession::simulate_dj_performance() {
    std::cout << "=== DJ Controller System ===" << std::endl;
    std::cout << "Starting interactive DJ session..." << std::endl;
    if (!load_configuration()) {
        std::cerr << "[ERROR] Failed to load configuration. Aborting session." << std::endl;
        return;
    }
    
    library_service.buildLibrary(session_config.library_tracks);
    
    if (session_config.playlists.empty()) {
        std::cerr << "[ERROR] No playlists found in configuration. Aborting session." << std::endl;
        return;
    }
    std::cout << "\nStarting DJ performance simulation..." << std::endl;
    std::cout << "BPM Tolerance: " << session_config.bpm_tolerance << " BPM" << std::endl;
    std::cout << "Auto Sync: " << (session_config.auto_sync ? "enabled" : "disabled") << std::endl;
    std::cout << "Cache Capacity: " << session_config.controller_cache_size << " slots (LRU policy)" << std::endl;
    std::cout << "\n--- Processing Tracks ---" << std::endl;


    if(play_all){
        std::vector<std::string> playlist_names;
        for(auto const& pair : session_config.playlists) {
            playlist_names.push_back(pair.first);
        }
        
        std::sort(playlist_names.begin(), playlist_names.end());
        
        for(const std::string& pl_name : playlist_names){
            if(!load_playlist(pl_name)){
                std::cout << "[ERROR] failed loading " << pl_name << std::endl;
                continue;
            }
            std::reverse(track_titles.begin(), track_titles.end());
            for(std::string title : track_titles){
                std::cout << "\n--- Processing: " << title << " ---" << std::endl;
                stats.tracks_processed++;
                load_track_to_controller(title);
                
                controller_service.displayCacheStatus();
                
                load_track_to_mixer_deck(title);
                
                mixing_service.displayDeckStatus();
            }
            print_session_summary();
        }
    }
    else{
        bool play=true;
        while(play){
            std::string input = display_playlist_menu_from_config();
            if(input.empty()){
                play=false;
            }
            else{
                if(!load_playlist(input)){
                    std:: cout <<"[ERROR] faild loading"<<std::endl;
                }
                else{
                    std::reverse(track_titles.begin(), track_titles.end());
                    for(std::string title: track_titles){
                        std::cout << "\n--- Processing: " << title << " ---" << std::endl;
                        stats.tracks_processed++;

                        load_track_to_controller(title);
                        controller_service.displayCacheStatus();

                        load_track_to_mixer_deck(title);
                        mixing_service.displayDeckStatus();

                    }
                    print_session_summary();
                    stats=SessionStats();
                }
            }
        }
    }
    std::cout << "Session cancelled by user or all playlists played." << std::endl;
}


bool DJSession::load_configuration() {
    const std::string config_path = "bin/dj_config.txt";
    
    std::cout << "Loading configuration from: " << config_path << std::endl;
    
    if (!SessionFileParser::parse_config_file(config_path, session_config)) {
        std::cerr << "[ERROR] Failed to parse configuration file: " << config_path << std::endl;
        return false;
    }
    
    std::cout << "Configuration loaded successfully." << std::endl;
    std::cout << "BPM Tolerance: " << session_config.bpm_tolerance << " BPM" << std::endl;
    std::cout << "Auto Sync: " << (session_config.auto_sync ? "enabled" : "disabled") << std::endl;
    std::cout << "Cache Size: " << session_config.controller_cache_size << " slots" << std::endl;
    mixing_service.set_auto_sync(session_config.auto_sync);
    mixing_service.set_bpm_tolerance(session_config.bpm_tolerance);
    controller_service.set_cache_size(session_config.controller_cache_size);
    return true;
}

std::string DJSession::display_playlist_menu_from_config() {
    if (session_config.playlists.empty()) {
        return "";
    }
    
    std::cout << "\n=== Available Playlists ===" << std::endl;
    
    std::vector<std::string> playlist_names;
    for (const auto& pair : session_config.playlists) {
        playlist_names.push_back(pair.first);
    }
    std::sort(playlist_names.begin(), playlist_names.end());
    
    for (size_t i = 0; i < playlist_names.size(); ++i) {
        std::cout << (i + 1) << ". " << playlist_names[i] << std::endl;
    }
    std::cout << "0. Cancel" << std::endl;
    
    int selection = -1;
    while (true) {
        std::cout << "\nSelect a playlist (1-" << playlist_names.size() << ", 0 to cancel): ";
        std::string input;
        
        if (!std::getline(std::cin, input)) {
            std::cout << "\n[ERROR] Input error. Cancelling session." << std::endl;
            return "";
        }
        
        std::stringstream ss(input);
        if (ss >> selection && ss.eof()) {
            if (selection == 0) {
                return "";
            } else if (selection >= 1 && selection <= static_cast<int>(playlist_names.size())) {
                std::string selected_name = playlist_names[selection - 1];
                std::cout << "Selected: " << selected_name << std::endl;
                return selected_name;
            }
        }
        
        std::cout << "Invalid selection. Please enter a number between 1 and " 
                  << playlist_names.size() << ", or 0 to cancel." << std::endl;
    }
}

void DJSession::print_session_summary() const {
    std::cout << "\n=== DJ Session Summary ===" << std::endl;
    std::cout << "Session: " << session_name << std::endl;
    std::cout << "Tracks processed: " << stats.tracks_processed << std::endl;
    std::cout << "Cache hits: " << stats.cache_hits << std::endl;
    std::cout << "Cache misses: " << stats.cache_misses << std::endl;
    std::cout << "Cache evictions: " << stats.cache_evictions << std::endl;
    std::cout << "Deck A loads: " << stats.deck_loads_a << std::endl;
    std::cout << "Deck B loads: " << stats.deck_loads_b << std::endl;
    std::cout << "Transitions: " << stats.transitions << std::endl;
    std::cout << "Errors: " << stats.errors << std::endl;
    std::cout << "=== Session Complete ===" << std::endl;
}