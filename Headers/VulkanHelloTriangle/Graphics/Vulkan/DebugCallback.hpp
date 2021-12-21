/*
 * This header contains the definition of a class to be passed to Vulkan
 * for a debug callback.
 */

#pragma once

#include <vulkan/vulkan.h>

namespace Vulkan
{
    class DebugCallback
    {
    public:
        virtual void OnDebugLog(
            VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
            VkDebugUtilsMessageTypeFlagsEXT messageType,
            const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData
        )
        {}
    };
}
