#include "DJLibraryService.h"
#include "SessionFileParser.h"
#include "MP3Track.h"
#include "WAVTrack.h"
#include <iostream>
#include <memory>
#include <filesystem>

DJLibraryService::DJLibraryService(const Playlist& playlist) 
    : playlist(playlist), library() {}

void DJLibraryService::buildLibrary(const std::vector<SessionConfig::TrackInfo>& library_tracks) {
    for(size_t i = 0; i< library_tracks.size(); ++i){
        AudioTrack* track = nullptr;
        if(library_tracks[i].type == "MP3"){
            track = new MP3Track(library_tracks[i].title, library_tracks[i].artists, library_tracks[i].duration_seconds,
            library_tracks[i].bpm, library_tracks[i].extra_param1, library_tracks[i].extra_param2);
        }
        else if(library_tracks[i].type == "WAV"){
            track = new WAVTrack(library_tracks[i].title, library_tracks[i].artists, library_tracks[i].duration_seconds,
            library_tracks[i].bpm, library_tracks[i].extra_param1, library_tracks[i].extra_param2);
        }
        if (track != nullptr){
            library.push_back(track);
        }
    }
    std::cout << "[INFO] Track library built: " << library_tracks.size() << " tracks loaded" << std::endl;
}

DJLibraryService::~DJLibraryService() {
    for(AudioTrack* at : library) {
        delete at;
    }
}

void DJLibraryService::displayLibrary() const {
    std::cout << "=== DJ Library Playlist: " 
              << playlist.get_name() << " ===" << std::endl;

    if (playlist.is_empty()) {
        std::cout << "[INFO] Playlist is empty.\n";
        return;
    }
    playlist.display();
    std::cout << "Total duration: " << playlist.get_total_duration() << " seconds" << std::endl;
}

Playlist& DJLibraryService::getPlaylist() {
    return playlist;
}

AudioTrack* DJLibraryService::findTrack(const std::string& track_title) {
    return playlist.find_track(track_title);
}

void DJLibraryService::loadPlaylistFromIndices(const std::string& playlist_name, 
                                               const std::vector<int>& track_indices) {
    std::cout<< "[INFO] Loading playlist: " << playlist_name<<std::endl;
    playlist=Playlist(playlist_name);
    int counter=0;
    for(size_t index : track_indices){
        if(index<= library.size() && index>=1){
            AudioTrack* track=library[index-1];
            PointerWrapper <AudioTrack> clone = track->clone();
            if(!clone){
                std:: cout << "[ERROR] Track:" <<track->get_title() << "failed to clone"<<std::endl;
                continue;
            }
            clone.get()->load();
            clone.get()->analyze_beatgrid();
            std::string loaded_title = clone.get()->get_title();
            playlist.add_track(clone.release());
            counter++;
        }
        else{
            std::cout << "[WARNING] Invalid track index: " <<index<<std::endl;
            continue;
        }
    }
    std::cout << "[INFO] Playlist loaded: " << playlist_name << " (" << counter << " tracks)" << std::endl;
    
}

std::vector<std::string> DJLibraryService::getTrackTitles() const {
    std::vector<std::string> titles;
     const std::vector<AudioTrack*>& tracks = playlist.getTracks();
     for(size_t i = 0; i < tracks.size(); ++i){
        if (tracks[i] != nullptr){
            titles.push_back(tracks[i]->get_title());
        }
    }
    return titles; 
}