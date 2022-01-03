#include <VulkanHelloTriangle/Application.hpp>
#include <VulkanHelloTriangle/Common/Resource.hpp>
#include <set>
#include <string>

void Application::DebugCallback::OnDebugLog(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData
)
{
    std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
}

void Application::Start()
{
    if (!InitWindow(1280, 720))
    {
        std::cout << "Failed to initialize window!" << std::endl;
        return;
    }

    if (!InitVulkan())
    {
        glfwDestroyWindow(window);
        std::cout << "Failed to initialize Vulkan!" << std::endl;
        return;
    }

    std::cout << "Started application" << std::endl;

    // glfwWindowShouldClose returns if the user has closed the window
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        Render();
    }

    Dispose();
    std::cout << "User closed applocation" << std::endl;
}

bool Application::InitWindow(uint64_t width, uint64_t height)
{
    // The GLFW_CLIENT_API hint in short tells GLFW to not use OpenGL.
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    window = glfwCreateWindow(width, height, "Vulkan Hello Triangle", nullptr, 
        nullptr);
    if (!window)
        return false;
    
    return true;
}

bool Application::InitVulkan()
{
    glslang::InitializeProcess();

    vulkan.AddDebugMessenger(&callback);
    if (!vulkan.Init(nullptr, nullptr, true))
        return false;
    if (!surface.Init(&vulkan, window))
        return false;
    
    VkPhysicalDevice physicalDevice;
    if (!vulkan.PickDevice(&physicalDevice, &surface))
        return false;
    
    Vulkan::QueueFamilyIndices indices = 
        vulkan.FindQueueFamilies(physicalDevice, &surface);
    
    if (!InitDevice(physicalDevice, indices))
    {
        std::cout << "Failed to initialize device!" << std::endl;
        return false;
    }

    if (!InitSwapchain(physicalDevice, indices))
    {
        std::cout << "Failed to initialize swapchain!" << std::endl;
        return false;
    }
    
    if (!InitShaders())
    {
        std::cout << "Failed to initialize shaders!" << std::endl;
        return false;
    }

    if (!InitRenderPass())
    {
        std::cout << "Failed to initialize render pass!" << std::endl;
        return false;
    }

    if (!InitPipeline())
    {
        std::cout << "Failed to initialize pipeline!" << std::endl;
        return false;
    }

    if (!InitFramebuffers())
    {
        std::cout << "Failed to initialize framebuffers!" << std::endl;
        return false;
    }
    
    return true;
}

bool Application::InitDevice(VkPhysicalDevice physicalDevice, 
    Vulkan::QueueFamilyIndices indices)
{
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = {
        indices.graphicsFamilyIndex,
        indices.presentFamilyIndex
    };
    
    float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies)
    {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;

        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures features{};

    if (!device.Init(
            &vulkan, 
            physicalDevice, 
            queueCreateInfos.data(), 
            queueCreateInfos.size(), 
            features, 
            vulkan.deviceExtensions.data(),
            vulkan.deviceExtensions.size()
        ))
        return false;
    
    graphicsQueue = device.GetDeviceQueue(indices.graphicsFamilyIndex, 0);
    presentQueue = device.GetDeviceQueue(indices.presentFamilyIndex, 0);
    
    return true;
}

bool Application::InitSwapchain(VkPhysicalDevice physicalDevice, 
    Vulkan::QueueFamilyIndices indices)
{
    Vulkan::SwapchainDetails details = 
        vulkan.QuerySwapchainSupport(physicalDevice, &surface);
    VkSurfaceFormatKHR surfaceFormat = swapchain.ChooseSurfaceFormat(
        details);

    Vulkan::SwapchainDescriptor swapchainDesc{};
    swapchainDesc.imageFormat = surfaceFormat.format;
    swapchainDesc.imageColorSpace = surfaceFormat.colorSpace;
    swapchainDesc.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    
    swapchainDesc.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    if (indices.graphicsFamilyIndex != indices.presentFamilyIndex)
        swapchainDesc.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    
    swapchainDesc.preTransform = details.capabilities.currentTransform;
    swapchainDesc.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchainDesc.familyIndices = indices;
    swapchainDesc.details = details;

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    if (!swapchain.Init(&surface, &device, width, height, &swapchainDesc))
        return false;
    
    if (!swapchain.GetImageViews(&swapchainImageViews))
        return false;
    
    return true;
}

bool Application::InitShaders()
{
    vertexModule.Init(&device);
    fragmentModule.Init(&device);

    Resource solidVertResource = LOAD_RESOURCE(Assets_solidVert_glsl);
    Resource solidFragResource = LOAD_RESOURCE(Assets_solidFrag_glsl);

    std::string solidVertString = std::string(
        solidVertResource.GetData(),
        solidVertResource.GetData() + solidVertResource.GetSize()
    );
    std::string solidFragString = std::string(
        solidFragResource.GetData(),
        solidFragResource.GetData() + solidFragResource.GetSize()
    );

    std::string vertexInfo;
    std::string vertexDebug;
    if (!vertexModule.FromGLSL(solidVertString.c_str(), 
        Vulkan::ShaderType::VERTEX, &vertexInfo, &vertexDebug))
    {
        std::cout << "Vertex info log:\n" << vertexInfo;
        std::cout << "Vertex debug log:\n" << vertexDebug;
        std::cout << "Vertex compilation failed!" << std::endl;

        return false;
    }
    
    std::string fragInfo;
    std::string fragDebug;
    if (!fragmentModule.FromGLSL(solidFragString.c_str(), 
        Vulkan::ShaderType::FRAG, &fragInfo, &fragDebug))
    {
        std::cout << "Fragment info log:\n" << fragInfo;
        std::cout << "Fragment debug log:\n" << fragDebug;
        std::cout << "Fragment compilation failed!" << std::endl;

        return false;
    }
    
    return true;
}

bool Application::InitRenderPass()
{
    Vulkan::AttachmentDescriptor colorAttachment{};
    colorAttachment.format = swapchain.GetImageFormat();
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    Vulkan::AttachmentReference colorAttachmentReference{};
    colorAttachmentReference.attachment = 0;
    colorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    Vulkan::SubpassDescriptor subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentReference;

    Vulkan::RenderPassDescriptor desc{};
    desc.attachmentCount = 1;
    desc.pAttachments = &colorAttachment;
    desc.subpassCount = 1;
    desc.pSubpasses = &subpass;

    if (!renderPass.Init(&device, &desc))
        return false;

    return true;
}

bool Application::InitPipeline()
{
    Vulkan::ShaderStageDescriptor vertexStage{};
    vertexStage.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertexStage.pModule = &vertexModule;
    vertexStage.entrypoint = "main";

    Vulkan::ShaderStageDescriptor fragmentStage{};
    vertexStage.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    vertexStage.pModule = &fragmentModule;
    vertexStage.entrypoint = "main";

    Vulkan::PipelineLayoutDescriptor pipelineLayoutDesc{};
    pipelineLayoutDesc.setLayoutCount = 0; 
    pipelineLayoutDesc.pSetLayouts = nullptr; 
    pipelineLayoutDesc.pushConstantRangeCount = 0; 
    pipelineLayoutDesc.pPushConstantRanges = nullptr; 

    if (!pipelineLayout.Init(&device, &pipelineLayoutDesc))
        return false;
    
    Vulkan::VertexInputDescriptor vertexInputInfo{};
    vertexInputInfo.vertexBindingDescriptionCount = 0;
    vertexInputInfo.pVertexBindingDescriptions = nullptr; 
    vertexInputInfo.vertexAttributeDescriptionCount = 0;
    vertexInputInfo.pVertexAttributeDescriptions = nullptr; 

    Vulkan::InputAssemblyDescriptor inputAssembly{};
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    VkExtent2D swapchainExtent = swapchain.GetExtent();

    Vulkan::Viewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)swapchainExtent.width;
    viewport.height = (float)swapchainExtent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    Vulkan::Rect2D scissor{};
    scissor.x = 0;
    scissor.y = 0;
    scissor.width = swapchainExtent.width;
    scissor.height = swapchainExtent.height;

    Vulkan::ViewportStateDescriptor viewportState{};
    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;

    Vulkan::RasterizerStateDescriptor rasterizer{};
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;
    rasterizer.depthBiasConstantFactor = 0.0f; 
    rasterizer.depthBiasClamp = 0.0f; 
    rasterizer.depthBiasSlopeFactor = 0.0f; 

    Vulkan::MultisampleDescriptor multisampling{};
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampling.minSampleShading = 1.0f; 
    multisampling.pSampleMask = nullptr; 
    multisampling.alphaToCoverageEnable = VK_FALSE; 
    multisampling.alphaToOneEnable = VK_FALSE; 

    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;
    colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; 
    colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; 
    colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; 
    colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; 
    colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; 
    colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; 

    Vulkan::ColorBlendStateDescriptor colorBlending{};
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY; 
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;

    Vulkan::ShaderStageDescriptor vertShaderStageInfo{};
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.pModule = &vertexModule;
    vertShaderStageInfo.entrypoint = "main";

    Vulkan::ShaderStageDescriptor fragShaderStageInfo{};
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.pModule = &fragmentModule;
    fragShaderStageInfo.entrypoint = "main";

    Vulkan::ShaderStageDescriptor stages[] =
    {
        vertShaderStageInfo, fragShaderStageInfo
    };

    Vulkan::PipelineDescriptor pipelineDesc{};
    pipelineDesc.stageCount = 2;
    pipelineDesc.pStages = stages;
    pipelineDesc.pVertexInputDesc = &vertexInputInfo;
    pipelineDesc.pInputAssemblyDesc = &inputAssembly;
    pipelineDesc.pViewportState = &viewportState;
    pipelineDesc.pRasterizerStateDesc = &rasterizer;
    pipelineDesc.pMultisampleDesc = &multisampling;
    pipelineDesc.pDepthStencilState = nullptr; 
    pipelineDesc.pColorBlendStateDesc = &colorBlending;
    pipelineDesc.pDynamicStateDesc = nullptr; 
    pipelineDesc.pLayout = &pipelineLayout;
    pipelineDesc.pRenderPass = &renderPass;
    pipelineDesc.subpass = 0;

    if (!pipeline.Init(&device, &pipelineDesc))
        return false;

    return true;
}

bool Application::InitFramebuffers()
{
    VkExtent2D swapchainExtent = swapchain.GetExtent();

    uint64_t imageViewCount = swapchainImageViews.size();
    
    framebuffers.resize(imageViewCount);

    for (uint64_t i = 0; i < imageViewCount; i++)
    {
        Vulkan::ImageView* attachments[] = 
        {
            swapchainImageViews[i].get()
        };

        Vulkan::FramebufferDescriptor framebufferDesc{};
        framebufferDesc.pRenderPass = &renderPass;
        framebufferDesc.attachmentCount = 1;
        framebufferDesc.ppAttachments = attachments;
        framebufferDesc.width = swapchainExtent.width;
        framebufferDesc.height = swapchainExtent.height;
        framebufferDesc.layers = 1;

        framebuffers[i] = RefTools::Create<Vulkan::Framebuffer>();
        if (!framebuffers[i]->Init(&device, &framebufferDesc))
            return false;
    }

    return true;
}

void Application::Render()
{
    
}

void Application::Dispose()
{
    for (auto framebuffer : framebuffers)
        framebuffer->Dispose();
    
    pipeline.Dispose();
    pipelineLayout.Dispose();
    renderPass.Dispose();
    vertexModule.Dispose();
    fragmentModule.Dispose();

    for (auto imageView : swapchainImageViews)
        imageView->Dispose();

    swapchain.Dispose();
    device.Dispose();
    surface.Dispose();
    vulkan.Dispose();
    glfwDestroyWindow(window);

    glslang::FinalizeProcess();
}
