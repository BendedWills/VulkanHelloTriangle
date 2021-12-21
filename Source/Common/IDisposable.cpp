#include <VulkanHelloTriangle/Common/IDisposable.hpp>
#include <iostream>

IDisposable::~IDisposable()
{
    Dispose();
}

void IDisposable::Dispose()
{
    if (!initialized)
        return;
    
    OnDispose();
    
    initialized = false;
}