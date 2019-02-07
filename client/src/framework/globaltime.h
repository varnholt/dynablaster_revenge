// global time interface
// whoever inherits this is responsible to make "getTime" return a constant time for each frame

#pragma once

class GlobalTime
{
public:
    GlobalTime();
    ~GlobalTime();

    static GlobalTime* Instance();

    virtual float getTime() const = 0;

private:
    static GlobalTime* mInstance;
};
