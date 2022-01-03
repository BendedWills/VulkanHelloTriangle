#include <VulkanHelloTriangle/Graphics/Vulkan/Pipeline.hpp>

using namespace Vulkan;

bool Pipeline::Init(Device* pDevice, PipelineDescriptor* pDesc)
{
    if (initialized)
		return false;
    
    this->pDevice = pDevice;

    if (!pDesc 
        || !pDesc->pVertexInputDesc
        || !pDesc->pInputAssemblyDesc
        || !pDesc->pViewportState
        || !pDesc->pRasterizerStateDesc
        || !pDesc->pMultisampleDesc
        || !pDesc->pColorBlendStateDesc
        || !pDesc->pLayout
        || !pDesc->pRenderPass
        || !pDesc->pStages
    )
        return false;

    VkPipelineShaderStageCreateInfo* shaderStages = 
        (VkPipelineShaderStageCreateInfo*)malloc(
            sizeof(VkPipelineShaderStageCreateInfo) * pDesc->stageCount);
    if (!shaderStages)
        return false;
    
    VkViewport* viewports = (VkViewport*)malloc(sizeof(VkViewport) 
        * pDesc->pViewportState->viewportCount);
    if (!viewports)
    {
        free(shaderStages);
        return false;
    }

    VkRect2D* scissors = (VkRect2D*)malloc(sizeof(VkRect2D) 
        * pDesc->pViewportState->scissorCount);
    if (!scissors)
    {
        free(shaderStages);
        free(viewports);
        return false;
    }

    memset(shaderStages, 0, sizeof(VkPipelineShaderStageCreateInfo) * pDesc->stageCount);
    memset(viewports, 0, sizeof(VkViewport) * pDesc->pViewportState->viewportCount);
    memset(scissors, 0, sizeof(VkRect2D) * pDesc->pViewportState->scissorCount);

    // Populate stages
    for (uint64_t i = 0; i < pDesc->stageCount; i++)
    {
        VkPipelineShaderStageCreateInfo* pStage = &shaderStages[i];
        ShaderStageDescriptor* pStageDesc = &pDesc->pStages[i];

        pStage->sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        pStage->stage = pStageDesc->stage;
        pStage->module = *pStageDesc->pModule->GetModule();
        pStage->pName = pStageDesc->entrypoint;
    }

    // Populate viewports
    for (uint64_t i = 0; i < pDesc->pViewportState->viewportCount; i++)
    {
        VkViewport* pVkViewport = &viewports[i];
        Viewport* pViewport = &pDesc->pViewportState->pViewports[i];

        pVkViewport->x = pViewport->x;
        pVkViewport->y = pViewport->y;
        pVkViewport->width = pViewport->width;
        pVkViewport->height = pViewport->height;
        pVkViewport->minDepth = pViewport->minDepth;
        pVkViewport->maxDepth = pViewport->maxDepth;
    }

    // Populate scissors
    for (uint64_t i = 0; i < pDesc->pViewportState->scissorCount; i++)
    {
        VkRect2D* pVkRect = &scissors[i];
        Rect2D* pRect = &pDesc->pViewportState->pScissors[i];

        pVkRect->offset.x = (int)pRect->x;
        pVkRect->offset.y = (int)pRect->y;
        pVkRect->extent.width = (uint32_t)pRect->width;
        pVkRect->extent.height = (uint32_t)pRect->height;
    }

    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 
        pDesc->pVertexInputDesc->vertexBindingDescriptionCount;
    vertexInputInfo.pVertexBindingDescriptions = 
        pDesc->pVertexInputDesc->pVertexBindingDescriptions;
    vertexInputInfo.vertexAttributeDescriptionCount = 
        pDesc->pVertexInputDesc->vertexAttributeDescriptionCount;
    vertexInputInfo.pVertexAttributeDescriptions = 
        pDesc->pVertexInputDesc->pVertexAttributeDescriptions;

    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = pDesc->pInputAssemblyDesc->topology;
    inputAssembly.primitiveRestartEnable = 
        pDesc->pInputAssemblyDesc->primitiveRestartEnable;
    
    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = pDesc->pViewportState->viewportCount;
    viewportState.scissorCount = pDesc->pViewportState->scissorCount;
    viewportState.pViewports = viewports;
    viewportState.pScissors = scissors;

    VkPipelineRasterizationStateCreateInfo rasterizerState{};
    rasterizerState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizerState.depthClampEnable = 
        pDesc->pRasterizerStateDesc->depthClampEnable;
    rasterizerState.rasterizerDiscardEnable = 
        pDesc->pRasterizerStateDesc->rasterizerDiscardEnable;
    rasterizerState.polygonMode = 
        pDesc->pRasterizerStateDesc->polygonMode;
    rasterizerState.lineWidth = 
        pDesc->pRasterizerStateDesc->lineWidth;
    rasterizerState.cullMode = 
        pDesc->pRasterizerStateDesc->cullMode;
    rasterizerState.frontFace = 
        pDesc->pRasterizerStateDesc->frontFace;
    rasterizerState.depthBiasEnable = 
        pDesc->pRasterizerStateDesc->depthBiasEnable;
    rasterizerState.depthBiasConstantFactor = 
        pDesc->pRasterizerStateDesc->depthBiasConstantFactor;
    rasterizerState.depthBiasClamp = 
        pDesc->pRasterizerStateDesc->depthBiasClamp;
    rasterizerState.depthBiasSlopeFactor = 
        pDesc->pRasterizerStateDesc->depthBiasSlopeFactor;
    
    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = 
        pDesc->pMultisampleDesc->sampleShadingEnable;
    multisampling.rasterizationSamples = 
        pDesc->pMultisampleDesc->rasterizationSamples;
    multisampling.minSampleShading = 
        pDesc->pMultisampleDesc->minSampleShading;
    multisampling.pSampleMask = 
        pDesc->pMultisampleDesc->pSampleMask;
    multisampling.alphaToCoverageEnable = 
        pDesc->pMultisampleDesc->alphaToCoverageEnable;
    multisampling.alphaToOneEnable = 
        pDesc->pMultisampleDesc->alphaToOneEnable;
    
    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = 
        pDesc->pColorBlendStateDesc->logicOpEnable;
    colorBlending.logicOp = 
        pDesc->pColorBlendStateDesc->logicOp;
    colorBlending.attachmentCount = 
        pDesc->pColorBlendStateDesc->attachmentCount;
    colorBlending.pAttachments = 
        pDesc->pColorBlendStateDesc->pAttachments;

    // Copy blend constants manually
    memcpy(
        colorBlending.blendConstants, 
        pDesc->pColorBlendStateDesc->blendConstants, 
        sizeof(float) * 4
    );
    
    VkGraphicsPipelineCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    createInfo.stageCount = pDesc->stageCount;
    createInfo.pStages = shaderStages;
    createInfo.pVertexInputState = &vertexInputInfo;
    createInfo.pInputAssemblyState = &inputAssembly;
    createInfo.pViewportState = &viewportState;
    createInfo.pRasterizationState = &rasterizerState;
    createInfo.pMultisampleState = &multisampling;
    createInfo.pColorBlendState = &colorBlending;
    createInfo.layout = *pDesc->pLayout->GetPipelineLayout();
    createInfo.renderPass = *pDesc->pRenderPass->GetRenderPass();
    createInfo.subpass = pDesc->subpass;

    VkPipelineDepthStencilStateCreateInfo depthStencilState{};
    if (pDesc->pDepthStencilState)
    {
        depthStencilState.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        depthStencilState.flags = pDesc->pDepthStencilState->flags;
        depthStencilState.depthTestEnable = 
            pDesc->pDepthStencilState->depthTestEnable;
        depthStencilState.depthWriteEnable = 
            pDesc->pDepthStencilState->depthWriteEnable;
        depthStencilState.depthCompareOp = 
            pDesc->pDepthStencilState->depthCompareOp;
        depthStencilState.depthBoundsTestEnable = 
            pDesc->pDepthStencilState->depthBoundsTestEnable;
        depthStencilState.stencilTestEnable = 
            pDesc->pDepthStencilState->stencilTestEnable;
        depthStencilState.front = 
            pDesc->pDepthStencilState->front;
        depthStencilState.back = 
            pDesc->pDepthStencilState->back;
        depthStencilState.minDepthBounds = 
            pDesc->pDepthStencilState->minDepthBounds;
        depthStencilState.maxDepthBounds = 
            pDesc->pDepthStencilState->maxDepthBounds;

        createInfo.pDepthStencilState = &depthStencilState;
    }
    else
        createInfo.pDepthStencilState = nullptr;
    
    VkPipelineDynamicStateCreateInfo dynamicState{};
    if (pDesc->pDynamicStateDesc)
    {
        dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicState.dynamicStateCount = pDesc->pDynamicStateDesc->dynamicStateCount;
        dynamicState.pDynamicStates = pDesc->pDynamicStateDesc->pDynamicStates;

        createInfo.pDynamicState = &dynamicState;
    }
    else
        createInfo.pDynamicState = nullptr;

    if (vkCreateGraphicsPipelines(*pDevice->GetDevice(), VK_NULL_HANDLE,
        1, &createInfo, nullptr, &pipeline) != VK_SUCCESS)
    {
        free(shaderStages);
        free(viewports);
        free(scissors);
        return false;
    }
    
    free(shaderStages);
    free(viewports);
    free(scissors);
    
    initialized = true;
    return true;
}

VkPipeline* Pipeline::GetPipeline()
{
    return &pipeline;
}

void Pipeline::OnDispose()
{
    vkDestroyPipeline(*pDevice->GetDevice(), pipeline, nullptr);
}