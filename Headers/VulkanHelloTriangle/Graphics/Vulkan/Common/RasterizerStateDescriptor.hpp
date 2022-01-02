#pragma once

#include <vulkan/vulkan.h>

namespace Vulkan
{
    struct RasterizerStateDescriptor
    {
        bool depthClampEnable = false;
        bool rasterizerDiscardEnable = false;
        VkPolygonMode polygonMode = VK_POLYGON_MODE_FILL;
        float lineWidth = 1.0f;
        VkCullModeFlagBits cullMode = VK_CULL_MODE_BACK_BIT;
        VkFrontFace frontFace = VK_FRONT_FACE_CLOCKWISE;
        bool depthBiasEnable = false;
        float depthBiasConstantFactor = 0.0f;
        float depthBiasClamp = 0.0f;
        float depthBiasSlopeFactor = 0.0f;
    };
}