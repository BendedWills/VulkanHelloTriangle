#pragma once

#include <iostream>

class Resource
{
public:
    Resource(const uint8_t* start, const uint8_t* end)
        :
        data(start),
        size(end - start)
    {}

    Resource(const Resource& ref)
    {
        data = ref.data;
        size = ref.size;
    }

    const uint8_t* GetData() const { return data; }
    const uint64_t GetSize() const { return size; } 
private:
    const uint8_t* data;
    uint64_t size;
};

#define LOAD_RESOURCE(resource) ([]() { \
    extern const char _binary_##resource##_start, _binary_##resource##_end; \
    return Resource((uint8_t*)&_binary_##resource##_start, (uint8_t*)&_binary_##resource##_end); \
})()