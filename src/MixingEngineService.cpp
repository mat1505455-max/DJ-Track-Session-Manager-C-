#include "MixingEngineService.h"
#include <iostream>
#include <memory>


/**
 * MixingEngineService constructor
 */
MixingEngineService::MixingEngineService()
    : decks(), active_deck(0), auto_sync(false), bpm_tolerance(0)
{
    decks[0]=nullptr;
    decks[1]=nullptr;
    std::cout <<"[MixingEngineService] Initialized with 2 empty decks."<<std::endl;
}

/**
 * MixingEngineService destructor
 */
MixingEngineService::~MixingEngineService() {
    std:: cout << "[MixingEngineService] Cleaning up decks..." << std::endl;
   for (size_t i = 0; i < 2; ++i) { 
        if (decks[i] != nullptr) {
            delete decks[i];
            decks[i] = nullptr;
        }
    }
}

MixingEngineService::MixingEngineService(const MixingEngineService& other) 
: decks(), active_deck(other.active_deck), auto_sync(other.auto_sync), bpm_tolerance(other.bpm_tolerance) {
    for (size_t i = 0; i < 2; ++i){
        if(other.decks[i] != nullptr){
            decks[i] = other.decks[i]->clone().release();
        } else {
            decks[i] = nullptr;
        }
    }
}

MixingEngineService& MixingEngineService :: operator= (const MixingEngineService& other){
    if(this == &other){
        return *this;
    }
    for (size_t i = 0; i < 2; ++i){
        if(decks[i] != nullptr){
            delete decks[i];
            decks[i] = nullptr;
        }
    }
    this->active_deck = other.active_deck;
    this->auto_sync = other.auto_sync;
    this->bpm_tolerance = other.bpm_tolerance;
    for (size_t i = 0; i < 2; ++i){
        if(other.decks[i] != nullptr){
            decks[i]= other.decks[i]->clone().release();
        }
    }
    return *this;
}

/**
 * loadTrackToDeck method
 */
int MixingEngineService::loadTrackToDeck(const AudioTrack& track) {
    std::cout << "\n=== Loading Track to Deck ===\n";
    PointerWrapper<AudioTrack> clone_track = track.clone();
    if(!clone_track){
        std::cerr << "[ERROR] Track:" <<track.get_title()<< " failed to clone " << std::endl;
        return -1;
    }
    
    if (decks[0]== nullptr && decks[1]== nullptr){
        std::cout << "[Deck Switch] Target deck: 0" << std::endl; // Match Wanted Output
        clone_track -> load();
        clone_track-> analyze_beatgrid();
        
        decks[0] = clone_track.release();
        active_deck = 0;
        
        std::cout << "[Sync BPM] Cannot sync - one of the decks is empty." << std::endl;
        std::cout << "[Load Complete] '" << decks[0]->get_title() << "' is now loaded on deck 0" << "\n";
        std::cout << "[Active Deck] Switched to deck 0" << std::endl;
        return 0;
    }
    
    int target_deck = 1 - active_deck;
    std::cout << "[Deck Switch] Target deck: " << target_deck << std::endl;

    clone_track -> load();
    clone_track-> analyze_beatgrid();

    if (decks[active_deck] != nullptr && auto_sync && !can_mix_tracks(clone_track)){ 
        sync_bpm(clone_track);
    } else if (decks[active_deck] == nullptr) {
        std::cout << "[Sync BPM] Cannot sync - one of the decks is empty." << std::endl;
    }

    if (decks[target_deck]!= nullptr){
        delete decks[target_deck];
        decks[target_deck]= nullptr;
    }
    
    AudioTrack* raw_track= clone_track.release();
    decks[target_deck] = raw_track;
    
    std::cout << "[Load Complete] '" <<raw_track->get_title()<< "' is now loaded on deck " << target_deck << "\n";
    active_deck = target_deck;
    std::cout<< "[Active Deck] Switched to deck " << target_deck << std::endl;

    return target_deck;
}

/**
 * Display current deck status
 */
void MixingEngineService::displayDeckStatus() const {
    std::cout << "\n=== Deck Status ===\n";
    for (size_t i = 0; i < 2; ++i) {
        if (decks[i])
            std::cout << "Deck " << i << ": " << decks[i]->get_title() << "\n";
        else
            std::cout << "Deck " << i << ": [EMPTY]\n";
    }
    std::cout << "Active Deck: " << active_deck << "\n";
    std::cout << "===================\n";
}

bool MixingEngineService::can_mix_tracks(const PointerWrapper<AudioTrack>& track) const {
    if(decks[active_deck] == nullptr){
        return false;
    }
    int curr_bpm = decks[active_deck]->get_bpm();
    int new_bpm = track->get_bpm();
    double difference = curr_bpm - new_bpm;
    if (difference < 0) {
        difference = -difference;
    }
    if (difference <= bpm_tolerance) {
        return true; 
    } 
    return false; 
}

void MixingEngineService::sync_bpm(const PointerWrapper<AudioTrack>& track) const {
    if(decks[active_deck]!=nullptr){
        if(track){
            int newtrackbpm=track.get()->get_bpm();
            int currtrackbpm=decks[active_deck]->get_bpm();
            int avgbpm= (newtrackbpm+currtrackbpm)/2;
            track.get()->set_bpm(avgbpm);
            std::cout << "[Sync BPM] Syncing BPM from " << newtrackbpm << " to " << avgbpm << std::endl;
        }
    }
}