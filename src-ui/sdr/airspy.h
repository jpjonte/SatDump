#pragma once

#include "sdr.h"

#ifdef BUILD_LIVE
#include <libairspy/airspy.h>

class SDRAirspy : public SDRDevice
{
private:
    struct airspy_device *dev;
    int gain = 10;
    bool bias = false;
    char frequency[100];

    static int _rx_callback(airspy_transfer *t);

public:
    SDRAirspy(uint64_t id = 0);
    void start();
    void stop();
    void drawUI();
    static void init();
    virtual void setFrequency(int frequency);
    static std::vector<std::tuple<std::string, sdr_device_type, uint64_t>> getDevices();
};
#endif