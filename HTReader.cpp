#include "HTReader.h"

// Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
#define SENSOR_DELAY 2000 // In msec

HTReader::HTReader(uint8_t pin, uint8_t type, uint16_t sleeping_time,
                   float temp_slope, float temp_shift, float humid_slope, float humid_shift)
    : dht(pin, type), _sleeping_time(sleeping_time),
      _temp_slope(temp_slope), _temp_shift(temp_shift),
      _humid_slope(humid_slope), _humid_shift(humid_shift)
{
    dht.begin();
    reset();
}

HTReader::HTReader(uint8_t pin, uint8_t type,
                   float temp_slope, float temp_shift, float humid_slope, float humid_shift)
    : dht(pin, type), _sleeping_time(0),
      _temp_slope(temp_slope), _temp_shift(temp_shift),
      _humid_slope(humid_slope), _humid_shift(humid_shift)
{
    dht.begin();
    reset();
}

bool HTReader::reset()
{
    _last_sensor_read_time = 0;
    _error = !_read_sensors(_t, _h);
    return !_error;
}

// To run inside loop.
// Return true if new data ready.
bool HTReader::beginLoop()
{
    float t, h;

    _error = false;

    if (_last_sensor_read_time >= delay_ms())
    {
        _error = !_read_sensors(t, h);
        if (!_error)
        {
            _t = t;
            _h = h;
        }

        _last_sensor_read_time = 0;
    }

    _last_sensor_read_time += _sleeping_time;

    return !_error;
}

float HTReader::getTemp()
{
    return _t;
}

float HTReader::getHumid()
{
    return _h;
}

bool HTReader::error()
{
    return _error;
}

uint16_t HTReader::delay_ms() { return SENSOR_DELAY; }

bool HTReader::_read_sensors(float &t, float &h)
{
    // Reading temperature or humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    // Read temperature as Celsius (the default)
    t = dht.readTemperature();
    h = dht.readHumidity();

    if (isnan(h) || isnan(t))
    {
        h = 0.0;
        t = 0.0;
        return false;
    }
    else
    {
        // adjust DHT11
        h = _humid_slope * h + _humid_shift;
        t = _temp_slope * t + _temp_shift; // Read temperature as C
        return true;
    }
}
