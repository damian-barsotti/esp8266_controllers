#include "HTReader.h"

HTReader::HTReader(uint8_t pin, uint8_t model, uint16_t sleeping_time,
                   float temp_slope, float temp_shift, float humid_slope, float humid_shift)
    : dht(pin, model), _model(model), _sleeping_time(sleeping_time),
      _temp_slope(temp_slope), _temp_shift(temp_shift),
      _humid_slope(humid_slope), _humid_shift(humid_shift),
      _n_reads(3), buffT(_n_reads), buffH(_n_reads)
{
    dht.begin();
    reset();
}

HTReader::HTReader(uint8_t pin, uint8_t model,
                   float temp_slope, float temp_shift, float humid_slope, float humid_shift)
    : dht(pin, model), _model(model), _sleeping_time(0),
      _temp_slope(temp_slope), _temp_shift(temp_shift),
      _humid_slope(humid_slope), _humid_shift(humid_shift),
      _n_reads(3), buffT(_n_reads), buffH(_n_reads)
{
    dht.begin();
    reset();
}

bool HTReader::reset()
{
    _last_sensor_read_time = 0;
    buffT.reset();
    buffH.reset();
    _error = !_read_sensors(_t, _h);
    if (!_error)
    {
        buffT.enQueue(_t);
        buffH.enQueue(_h);
    }
    return !_error;
}

// To run inside loop.
// Return true if new data ready.
bool HTReader::beginLoop()
{
    auto sum = [](float m, float n)
    {
        return m + n;
    };
    float t, h;
    bool updated,

        _error = false;

    if (_last_sensor_read_time >= delay_ms())
    {
        _error = !_read_sensors(t, h);
        if (!_error)
        {
            buffT.enQueue(t);
            buffH.enQueue(h);
            _t = buffT.reduce(sum) / buffT.len();
            _h = buffH.reduce(sum) / buffH.len();
        }
        updated = !_error;
        _last_sensor_read_time = 0;
    }
    else
        updated = false;

    _last_sensor_read_time += _sleeping_time;

    return updated;
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

uint16_t HTReader::delay_ms() { return getMinimumSamplingPeriod() * 2; }

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

int HTReader::getMinimumSamplingPeriod() { return _model == DHT11 ? 1000 : 2000; }
