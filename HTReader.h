#ifndef HTREADER_H
#define HTREADER_H

#include "Arduino.h"

#include <DHT.h>

class HTReader {
    public:
    HTReader(uint8_t pin, uint8_t type, uint16_t sleeping_time, uint16_t n_avg, 
        float temp_slope, float temp_shift, float humid_slope, float humid_shift);
    // To use with deep sleep without beginLoop  
    HTReader(uint8_t pin, uint8_t type, 
        float temp_slope, float temp_shift, float humid_slope, float humid_shift);
    bool reset();
    bool beginLoop();
    float getTemp();
    float getHumid();
    bool error();
    uint16_t delay_ms();
    private:
        DHT dht;
        const uint16_t _sleeping_time;
        const uint16_t _n_avg;
        const float _temp_slope, _temp_shift, _humid_slope, _humid_shift;
        int _last_sensor_read_time;
        int _n_sensor_reads;
        float _ac_t, _ac_h;
        float _t, _h;
        bool _error;
        bool _read_sensors(float &t, float &h);
};

#endif