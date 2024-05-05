#ifndef AUDIOFORMAT_H
#define AUDIOFORMAT_H


namespace SE {


struct AudioFormat {
    int sampleRate = 44100;
    int numChannels = 1;

    explicit AudioFormat(int sr, int channels) :
        sampleRate(sr),
        numChannels(channels)
    { }
};


}


#endif // AUDIOFORMAT_H
