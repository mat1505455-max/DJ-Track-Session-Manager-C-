#include "WAVTrack.h"
#include <iostream>

WAVTrack::WAVTrack(const std::string& title, const std::vector<std::string>& artists, 
                   int duration, int bpm, int sample_rate, int bit_depth)
    : AudioTrack(title, artists, duration, bpm), sample_rate(sample_rate), bit_depth(bit_depth) {

    std::cout << "WAVTrack created: " << sample_rate << "Hz/" << bit_depth << "bit" << std::endl;
}

void WAVTrack::load() {
    std::cout << "[WAVTrack::load] Loading WAV: \"" << title << "\" at " << sample_rate << "Hz/" << bit_depth << "bit (uncompressed)..." << "\n";
    long long size = (long long) duration_seconds * sample_rate * (bit_depth / 8) *2;
    std::cout << "  \u2192 Estimated file size: " <<size<< " bytes" << "\n";
    std::cout << "  \u2192 Fast loading due to uncompressed format." << "\n";
}

void WAVTrack::analyze_beatgrid() {
    std::cout << "[WAVTrack::analyze_beatgrid] Analyzing beat grid for: \"" << title << "\"\n"; 
    int beats = (duration_seconds / 60.0) * bpm;
    std::cout << "  \u2192 Estimated beats: " << beats << "  \u2192 Precision factor: 1 (uncompressed audio)" << "\n";
}

double WAVTrack::get_quality_score() const {
    double score = 70.0;
    if(sample_rate >= 44100){
        score = score + 10;
    }
    if(sample_rate >= 96000){
        score = score + 5;
    }
    if(bit_depth >= 16){
        score = score + 10;
    }
    if(bit_depth >= 24){
        score = score + 5;
    }
    if(score > 100.0){
        score = 100.0;
    }
    return score;
}

PointerWrapper<AudioTrack> WAVTrack::clone() const {
    return PointerWrapper<AudioTrack>(new WAVTrack(*this));
}