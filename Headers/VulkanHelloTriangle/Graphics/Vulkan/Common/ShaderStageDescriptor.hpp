#pragma once

#include <VulkanHelloTriangle/Graphics/Vulkan/ShaderModule.hpp>
#include <vulkan/vulkan.h>

namespace Vulkan
{
    struct ShaderStageDescriptor
    {
        VkShaderStageFlagBits stage;
        ShaderModule* pModule;
        const char* entrypoint;
    };
}