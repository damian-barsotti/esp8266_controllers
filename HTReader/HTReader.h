#ifndef HTREADER_H
#define HTREADER_H

#include "Arduino.h"

#include <DHT.h>

#include "../Controller/Controller.h"
#include "../CircularBuffer/CircularBuffer.h"

class HTReader: public Controller
{
public:
    HTReader(uint8_t pin, uint8_t model, uint16_t sleeping_time,
             float temp_slope, float temp_shift,
             float humid_slope, float humid_shift,
             std::size_t n_reads);
    // To use with deep sleep without beginLoop
    HTReader(uint8_t pin, uint8_t model,
             float temp_slope, float temp_shift,
             float humid_slope, float humid_shift,
             std::size_t n_reads);
    bool begin();
    bool reset();
    bool beginLoop();
    float getTemp();
    float getHumid();
    bool error();
    uint16_t delay_ms();
    int getMinimumSamplingPeriod();

private:
    DHT dht;
    const uint16_t _sleeping_time;
    const float _temp_slope, _temp_shift, _humid_slope, _humid_shift;
    int _last_sensor_read_time;
    float _t, _h;
    bool _error;
    bool _read_sensors(float &t, float &h);
    uint8_t _model;
    std::size_t _n_reads;
    CircularBuffer<float> buffT, buffH;
};

#endif