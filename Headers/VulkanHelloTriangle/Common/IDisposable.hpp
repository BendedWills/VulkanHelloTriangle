#pragma once

class IDisposable
{
public:
    ~IDisposable();

    void Dispose();
protected:
    virtual void OnDispose() {}

    bool initialized = false;
};