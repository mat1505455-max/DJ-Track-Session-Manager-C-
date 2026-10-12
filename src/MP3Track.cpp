#include "MP3Track.h"
#include <iostream>
#include <cmath>
#include <algorithm>

MP3Track::MP3Track(const std::string& title, const std::vector<std::string>& artists, 
                   int duration, int bpm, int bitrate, bool has_tags)
    : AudioTrack(title, artists, duration, bpm), bitrate(bitrate), has_id3_tags(has_tags) {

    std::cout << "MP3Track created: " << bitrate << " kbps" << std::endl;
}

void MP3Track::load() {
    std::cout << "[MP3Track::load] Loading MP3: \"" << title
              << "\" at " << bitrate << " kbps...\n";
    if (has_id3_tags){
        std::cout << "  \u2192 Processing ID3 metadata (artist info, album art, etc.)..." << std::endl;
    }
    else{
        std::cout <<"  \u2192 No ID3 tags found." <<std::endl;
    }
    std::cout <<"  \u2192 Decoding MP3 frames..." <<std::endl;
    std::cout <<"  \u2192 Load complete." <<std::endl;
}

void MP3Track::analyze_beatgrid() {
    std::cout << "[MP3Track::analyze_beatgrid] Analyzing beat grid for: \"" << title << "\"\n";
    int beats_estimated = (duration_seconds / 60.0) * bpm;
    double precision_factor = bitrate / 320.0 ; 
    std::cout <<"  \u2192 Estimated beats: "<< beats_estimated <<"  \u2192 Compression precision factor: " << precision_factor << "\n" ;
}

double MP3Track::get_quality_score() const {
    double base_score= (bitrate/ 320.0)*100.0;
    if(has_id3_tags){
        base_score = base_score +5;
    }
    if (bitrate < 128){
         base_score = base_score -10;
    }
    if (base_score <0){
        return 0.0;
    }
    if (base_score > 100){
        return 100.0;
    }
    else{
        return base_score;
    }
}

PointerWrapper<AudioTrack> MP3Track::clone() const {
    return PointerWrapper<AudioTrack>(new MP3Track(*this));
}