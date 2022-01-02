#pragma once

#include <vulkan/vulkan.h>

namespace Vulkan
{
    struct MultisampleDescriptor
    {
        bool sampleShadingEnable = false;
        VkSampleCountFlagBits rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        float minSampleShading = 1.0f;
        VkSampleMask* pSampleMask = nullptr;
        bool alphaToCoverageEnable = false;
        bool alphaToOneEnable = false;
    };
}