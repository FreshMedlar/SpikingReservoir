#ifndef ENCODER_H
#define ENCODER_H

#include <nlohmann/json.hpp>
#include <vector>
#include <string>
#include <map>

class Encoder {
public:
    Encoder(int n_sam);
    // return the spike encoding of a char
    
private:
    // sampling time
    int n_sam;
};

#endif // ENCODER_H
