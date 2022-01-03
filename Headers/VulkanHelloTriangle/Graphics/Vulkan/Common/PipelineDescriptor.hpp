#pragma once

#include <VulkanHelloTriangle/Graphics/Vulkan/Common/InputAssemblyDescriptor.hpp>
#include <VulkanHelloTriangle/Graphics/Vulkan/Common/ShaderStageDescriptor.hpp>
#include <VulkanHelloTriangle/Graphics/Vulkan/Common/ViewportStateDescriptor.hpp>
#include <VulkanHelloTriangle/Graphics/Vulkan/Common/RasterizerStateDescriptor.hpp>
#include <VulkanHelloTriangle/Graphics/Vulkan/Common/MultisampleDescriptor.hpp>
#include <VulkanHelloTriangle/Graphics/Vulkan/Common/ColorBlendStateDescriptor.hpp>
#include <VulkanHelloTriangle/Graphics/Vulkan/Common/VertexInputDescriptor.hpp>
#include <VulkanHelloTriangle/Graphics/Vulkan/Common/DynamicStateDescriptor.hpp>
#include <VulkanHelloTriangle/Graphics/Vulkan/Common/DepthStencilDescriptor.hpp>
#include <VulkanHelloTriangle/Graphics/Vulkan/PipelineLayout.hpp>
#include <VulkanHelloTriangle/Graphics/Vulkan/RenderPass.hpp>

#include <vulkan/vulkan.h>

namespace Vulkan
{
    struct PipelineDescriptor
    {
        uint32_t stageCount;
        ShaderStageDescriptor* pStages;
        VertexInputDescriptor* pVertexInputDesc;
        InputAssemblyDescriptor* pInputAssemblyDesc;
        ViewportStateDescriptor* pViewportState;
        RasterizerStateDescriptor* pRasterizerStateDesc;
        MultisampleDescriptor* pMultisampleDesc;
        DepthStencilDescriptor* pDepthStencilState;
        ColorBlendStateDescriptor* pColorBlendStateDesc;
        DynamicStateDescriptor* pDynamicStateDesc;
        PipelineLayout* pLayout;
        RenderPass* pRenderPass;
        uint32_t subpass;
    };
}