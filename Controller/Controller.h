#ifndef CONTROLLER_H
#define CONTROLLER_H

// Abstract controller class

class Controller
{
public:
    virtual bool begin() = 0;
    virtual bool reset() = 0;
    virtual bool beginLoop() = 0;
    virtual bool error() = 0;
};

#endif