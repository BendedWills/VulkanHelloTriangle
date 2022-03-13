/*
 * ----------------- VULKAN HELLO TRIANGLE -----------------
 *
 * Hello! This is the main file for the vulkan hello triangle.
 * I've tried to keep it as clean as possible since I'm doing it (mostly) in a
 * main.
 * 
 * I've purposely created some other files in this project though and that is
 * because they are just objects in Vulkan that either are painful to set up,
 * or they have some utility functions in them that make them easier to use.
 * A good example would be the ShaderModule class I created.
 * It uses GLSLANG to compile GLSL shaders into SPIR-V bytecode.
 * I've also done it this way so that it is easy for you to copy all the
 * utility files into your programs.
 * 
 * So yeah, enjoy!
 */

#include <VulkanHelloTriangle/Graphics/Vulkan/Instance.hpp>
#include <VulkanHelloTriangle/Graphics/Vulkan/Surface.hpp>
#include <VulkanHelloTriangle/Graphics/Vulkan/Device.hpp>
#include <VulkanHelloTriangle/Graphics/Vulkan/Swapchain.hpp>
#include <VulkanHelloTriangle/Graphics/Vulkan/ShaderModule.hpp>
#include <VulkanHelloTriangle/Common/Resource.hpp>

#include <iostream>
#include <vector>

class DebugCallback : public Vulkan::DebugCallback
{
public:
    void OnDebugLog(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData
    )
    {
        if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
            || messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
            std::cerr << "validation layer: " << pCallbackData->pMessage 
                << std::endl;
    }
};

struct Vertex
{
    float position[2];
    float color[3];

    static VkVertexInputBindingDescription GetBindingDescription()
    {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescription;
    }

    static std::array<VkVertexInputAttributeDescription, 2> GetAttributeDescriptions()
    {
        std::array<VkVertexInputAttributeDescription, 2> attributeDescs{};
        // Position
        attributeDescs[0].location = 0;
        attributeDescs[0].binding = 0;
        attributeDescs[0].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescs[0].offset = offsetof(Vertex, position);
        // Color
        attributeDescs[1].location = 1;
        attributeDescs[1].binding = 0;
        attributeDescs[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescs[1].offset = offsetof(Vertex, color);

        return attributeDescs;
    }
};

static const std::vector<const char*> deviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

static GLFWwindow* window = nullptr;

static Vulkan::Instance vulkan;
static Vulkan::Surface surface;
static Vulkan::Device device;
static Vulkan::Swapchain swapchain;

static VkQueue graphicsQueue;
static VkQueue presentQueue;
static VkPipelineLayout pipelineLayout;
static VkRenderPass renderPass;
static VkPipeline pipeline;
static VkCommandPool commandPool;
static VkBuffer vertexBuffer;
static VkDeviceMemory vertexBufferMemory;

static Vulkan::QueueFamilyIndices indices;
static VkPhysicalDevice physicalDevice;

// Shader stuff
static Vulkan::ShaderModule vertexModule;
static Vulkan::ShaderModule fragmentModule;

static std::vector<VkSemaphore> imageAvailableSemaphores;
static std::vector<VkSemaphore> renderFinishedSemaphores;
static std::vector<VkFence> inFlightFences;
static std::vector<VkFence> imagesInFlight;
static std::vector<VkFramebuffer> framebuffers;
static std::vector<VkImage> swapchainImages;
static std::vector<VkImageView> swapchainImageViews;
static std::vector<VkCommandBuffer> commandBuffers;

static DebugCallback callback;

static const int MAX_FRAMES_IN_FLIGHT = 2;
static uint64_t currentFrame = 0;

static constexpr const uint64_t width = 1280;
static constexpr const uint64_t height = 720;

static bool InitWindow();
static bool InitVulkan();
static bool PickDevice(VkPhysicalDevice* pDevice, Vulkan::Surface* pSurface);
static bool IsDeviceSuitable(VkPhysicalDevice device, Vulkan::Surface* pSurface);
static bool InitDevice();
static bool InitSwapchain(uint64_t width, uint64_t height);
static bool InitShaders();
static bool InitRenderPass();
static bool InitPipeline();
static bool InitFramebuffers();
static bool InitCommandPool();
static bool InitVertexBuffers();
static bool InitCommandBuffers();
static bool InitSyncObjects();
static void DisposeSwapchain();
static bool RecreateSwapchain(uint64_t width, uint64_t height);

static bool FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties, uint32_t* pMemoryType);

static void ResizeCallback(GLFWwindow* window, int width, int height)
{
    if (!RecreateSwapchain(width, height))
    {
        std::cout << "Failed to recreate swapchain!" << std::endl;
        exit(EXIT_FAILURE);
    }
}

static bool Init()
{
    if (!glfwInit())
    {
        std::cout << "Failed to initialize GLFW!" << std::endl;
        return false;
    }

    if (!InitWindow())
    {
        std::cout << "Failed to initialize window!" << std::endl;
        return false;
    }

    if (!InitVulkan())
    {
        std::cout << "Failed to initialize vulkan!" << std::endl;
        return false;
    }

    glfwShowWindow(window);

    return true;
}

static bool InitWindow()
{
    if (!glfwInit())
    {
        std::cout << "Failed to initialize GLFW!" << std::endl;
        return false;
    }

    // The GLFW_CLIENT_API hint in short tells GLFW to not use OpenGL.
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_VISIBLE, false);
    window = glfwCreateWindow(width, height, "Vulkan Hello Triangle", nullptr, 
        nullptr);
    if (!window)
        return false;
    
    glfwSetFramebufferSizeCallback(window, ResizeCallback);
    
    return true;
}

static bool InitVulkan()
{
    glslang::InitializeProcess();

#ifdef DEBUG_ENABLED
    bool enableValidationLayers = true;
#else
    bool enableValidationLayers = false;
#endif

    if (!vulkan.Init("VulkanHelloTriangle", "uhh", enableValidationLayers))
        return false;
    vulkan.AddDebugMessenger(&callback);

    if (!surface.Init(&vulkan, window))
        return false;
    
    if (!PickDevice(&physicalDevice, &surface))
        return false;
    
    indices = vulkan.FindQueueFamilies(physicalDevice, &surface);

    if (!InitDevice())
    {
        std::cout << "Failed to initialize GLFW!" << std::endl;
        return false;
    }

    if (!InitSwapchain(width, height))
    {
        std::cout << "Failed to initialize swapchain!" << std::endl;
        return false;
    }

    if (!InitRenderPass())
    {
        std::cout << "Failed to initialize render pass!" << std::endl;
        return false;
    }

    if (!InitShaders())
    {
        std::cout << "Failed to initialize shaders!" << std::endl;
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

    if (!InitCommandPool())
    {
        std::cout << "Failed to initialize command pool!" << std::endl;
        return false;
    }

    if (!InitVertexBuffers())
    {
        std::cout << "Failed to initialize vertex buffers!" << std::endl;
        return false;
    }

    if (!InitCommandBuffers())
    {
        std::cout << "Failed to initialize command buffers!" << std::endl;
        return false;
    }

    if (!InitSyncObjects())
    {
        std::cout << "Failed to initialize sync objects!" << std::endl;
        return false;
    }
    
    return true;
}

static bool PickDevice(VkPhysicalDevice* pDevice, Vulkan::Surface* pSurface)
{
    uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(vulkan.Get(), &deviceCount, nullptr);
	if (deviceCount == 0)
		return false;
	
	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(vulkan.Get(), &deviceCount, 
        devices.data());

	for (VkPhysicalDevice device : devices)
		if (IsDeviceSuitable(device, pSurface))
		{
			*pDevice = device;
			return true;
		}
	
	return false;
}

static bool IsDeviceSuitable(VkPhysicalDevice device, Vulkan::Surface* pSurface)
{
	VkPhysicalDeviceProperties deviceProperties;
	VkPhysicalDeviceFeatures deviceFeatures;
	vkGetPhysicalDeviceProperties(device, &deviceProperties);
	vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

	Vulkan::QueueFamilyIndices families = 
        vulkan.FindQueueFamilies(device, pSurface);

	bool extentionsSupported = vulkan.CheckDeviceExtentionSupport(device,
        deviceExtensions.data(), deviceExtensions.size());

	bool swapChainGood = false;
	if (extentionsSupported)
	{
		Vulkan::SwapchainDetails details = vulkan.QuerySwapchainSupport(device, 
            pSurface);
		swapChainGood = !details.formats.empty() 
			&& !details.presentModes.empty();
	}

	return 
		deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU
		&& deviceFeatures.geometryShader
		&& families.hasGraphicsFamily
		&& families.hasPresentFamily
		&& swapChainGood
		&& extentionsSupported;
}

static bool InitDevice()
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
            deviceExtensions.data(),
            deviceExtensions.size()
        ))
        return false;
    
    graphicsQueue = device.GetDeviceQueue(indices.graphicsFamilyIndex, 0);
    presentQueue = device.GetDeviceQueue(indices.presentFamilyIndex, 0);

    return true;
}

static VkSurfaceFormatKHR ChooseSurfaceFormat(Vulkan::SwapchainDetails details) 
{
    for (const auto& availableFormat : details.formats)
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB 
			&& availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            return availableFormat;

    return details.formats[0];
}

static bool InitSwapchain(uint64_t width, uint64_t height)
{
    Vulkan::SwapchainDetails details = 
        vulkan.QuerySwapchainSupport(physicalDevice, &surface);
    VkSurfaceFormatKHR surfaceFormat = ChooseSurfaceFormat(details);

    uint32_t imageCount = details.capabilities.minImageCount + 1;
    if (details.capabilities.maxImageCount > 0 && 
        imageCount > details.capabilities.maxImageCount)
        imageCount = details.capabilities.maxImageCount;

    VkSwapchainCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = surface.Get();
	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = swapchain.ChooseExtent(details.capabilities, 
        width, height);
	createInfo.imageArrayLayers = 1;

	// This will probably be changed later on to take in a parameter in this
	// function.
	// This parameter in the swapchain specifies what the images in the
	// swapchain are used for.
	// This may seem familiar if you have done computer graphics before,
	// as this is only the color attachment. There are others too, such as
	// the depth buffer.
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	createInfo.preTransform = details.capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;
	createInfo.clipped = true;
	createInfo.oldSwapchain = VK_NULL_HANDLE;

    indices = vulkan.FindQueueFamilies(physicalDevice, &surface);
    if (indices.graphicsFamilyIndex != indices.presentFamilyIndex)
    {
        if (!indices.hasPresentFamily)
			return false;
			
		uint32_t queueFamilyIndices[] = 
		{
			indices.graphicsFamilyIndex, 
			indices.presentFamilyIndex
		};

		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    
    if (!swapchain.Init(&surface, &device, details, 
        &createInfo))
        return false;
    
    swapchainImages = swapchain.GetImages();
    if (!swapchain.GetImageViews(&swapchainImageViews))
        return false;
    
    return true;
}

static bool InitShaders()
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

static bool InitRenderPass()
{
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = swapchain.GetImageFormat();
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentReference{};
    colorAttachmentReference.attachment = 0;
    colorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentReference;

    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo desc{};
    desc.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    desc.attachmentCount = 1;
    desc.pAttachments = &colorAttachment;
    desc.subpassCount = 1;
    desc.pSubpasses = &subpass;
    desc.dependencyCount = 1;
    desc.pDependencies = &dependency;

    if (vkCreateRenderPass(device.Get(), &desc, nullptr, &renderPass)
        != VK_SUCCESS)
        return false;

    return true;
}

static bool InitPipeline()
{
    VkPipelineLayoutCreateInfo pipelineLayoutDesc{};
    pipelineLayoutDesc.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutDesc.setLayoutCount = 0; 
    pipelineLayoutDesc.pushConstantRangeCount = 0; 
    
    if (vkCreatePipelineLayout(device.Get(), &pipelineLayoutDesc, 
        nullptr, &pipelineLayout) != VK_SUCCESS)
        return false;

    auto bindingDescription = Vertex::GetBindingDescription();
    auto attributeDescriptions = Vertex::GetAttributeDescriptions();
    
    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
    vertexInputInfo.vertexAttributeDescriptionCount = attributeDescriptions.size();
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
    
    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    VkExtent2D swapchainExtent = swapchain.GetExtent();

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)swapchainExtent.width;
    viewport.height = (float)swapchainExtent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor{};
    scissor.offset.x = 0;
    scissor.offset.y = 0;
    scissor.extent.width = swapchainExtent.width;
    scissor.extent.height = swapchainExtent.height;

    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;

    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;
    
    VkPipelineMultisampleStateCreateInfo multisampleState{};
    multisampleState.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampleState.sampleShadingEnable = VK_FALSE;
    multisampleState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampleState.minSampleShading = 0.0f; 
    multisampleState.pSampleMask = nullptr; 
    multisampleState.alphaToCoverageEnable = VK_FALSE; 
    multisampleState.alphaToOneEnable = VK_FALSE; 

    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;
    colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ZERO; 
    colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; 
    colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; 
    colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; 
    colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; 
    colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; 

    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY; 
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;

    // Oh, yes, cool thing about Vulkan, you can actually have multiple shader
    // stages for one shader module. That means that you can have a VSMain and
    // a PSMain in one shader module (aka a glsl file in this case).
    VkPipelineShaderStageCreateInfo vertexStage{};
    vertexStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertexStage.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertexStage.module = vertexModule.Get();
    vertexStage.pName = "main";

    VkPipelineShaderStageCreateInfo fragmentStage{};
    fragmentStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragmentStage.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragmentStage.module = fragmentModule.Get();
    fragmentStage.pName = "main";

    VkPipelineShaderStageCreateInfo stages[] =
    {
        vertexStage, fragmentStage
    };

    VkGraphicsPipelineCreateInfo pipelineDesc{};
    pipelineDesc.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineDesc.stageCount = 2;
    pipelineDesc.pStages = stages;
    pipelineDesc.pVertexInputState = &vertexInputInfo;
    pipelineDesc.pInputAssemblyState = &inputAssembly;
    pipelineDesc.pViewportState = &viewportState;
    pipelineDesc.pRasterizationState = &rasterizer;
    pipelineDesc.pMultisampleState = &multisampleState;
    pipelineDesc.pColorBlendState = &colorBlending;
    pipelineDesc.layout = pipelineLayout;
    pipelineDesc.renderPass = renderPass;
    pipelineDesc.subpass = 0;
    pipelineDesc.basePipelineHandle = VK_NULL_HANDLE;

    if (vkCreateGraphicsPipelines(device.Get(), VK_NULL_HANDLE,
        1, &pipelineDesc, nullptr, &pipeline) != VK_SUCCESS)
        return false;
    
    vertexModule.Dispose();
    fragmentModule.Dispose();

    return true;
}

static bool InitFramebuffers()
{
    VkExtent2D swapchainExtent = swapchain.GetExtent();
    uint64_t imageViewCount = swapchainImageViews.size();
    
    framebuffers.resize(imageViewCount);

    for (uint64_t i = 0; i < imageViewCount; i++)
    {
        VkFramebufferCreateInfo framebufferDesc{};
        framebufferDesc.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferDesc.renderPass = renderPass;
        framebufferDesc.attachmentCount = 1;
        framebufferDesc.pAttachments = &swapchainImageViews[i];
        framebufferDesc.width = swapchainExtent.width;
        framebufferDesc.height = swapchainExtent.height;
        framebufferDesc.layers = 1;

        if (vkCreateFramebuffer(device.Get(), &framebufferDesc, nullptr,
            &framebuffers[i]) != VK_SUCCESS)
            return false;
    }

    return true;
}

static bool InitCommandPool()
{
    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = indices.graphicsFamilyIndex;
    
    return vkCreateCommandPool(device.Get(), &poolInfo, nullptr, &commandPool)
        == VK_SUCCESS;
}

static bool InitVertexBuffers()
{
    Vertex vertices[] = 
    {
          // Position       // Color
        { {  0.0f, -0.5f }, { 1.0f, 0.0f, 0.0f } },
        { {  0.5f,  0.5f }, { 0.0f, 1.0f, 0.0f } },
        { { -0.5f,  0.5f }, { 0.0f, 0.0f, 1.0f } }
    };
    uint64_t vertexCount = sizeof(vertices) / sizeof(vertices[0]);

    VkBufferCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    createInfo.size = sizeof(vertices[0]) * vertexCount;
    createInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(device.Get(), &createInfo, nullptr, &vertexBuffer)
        != VK_SUCCESS)
        return false;
    
    VkMemoryRequirements memoryRequirements;
    vkGetBufferMemoryRequirements(device.Get(), vertexBuffer, &memoryRequirements);

    uint32_t memoryType;
    if (!FindMemoryType(memoryRequirements.memoryTypeBits,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        &memoryType))
    {
        std::cout << "Failed to find suitable memory type!" << std::endl;
        return false;
    }

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memoryRequirements.size;
    allocInfo.memoryTypeIndex = memoryType;
    
    if (vkAllocateMemory(device.Get(), &allocInfo, nullptr, &vertexBufferMemory)
        != VK_SUCCESS)
    {
        std::cout << "Failed to allocate vertex buffer memory!" << std::endl;
        return false;
    }

    vkBindBufferMemory(device.Get(), vertexBuffer, vertexBufferMemory, 0);

    void* data;
    vkMapMemory(device.Get(), vertexBufferMemory, 0, createInfo.size, 0, &data);
        memcpy(data, vertices, sizeof(vertices));
    vkUnmapMemory(device.Get(), vertexBufferMemory);
    
    return true;
}

static bool InitCommandBuffers()
{
    VkExtent2D swapchainExtent = swapchain.GetExtent();

    commandBuffers.resize(framebuffers.size());

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();

    if (vkAllocateCommandBuffers(device.Get(), &allocInfo, 
        commandBuffers.data()) != VK_SUCCESS)
        return false;
    
    for (uint64_t i = 0; i < commandBuffers.size(); i++)
    {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS)
            return false;
        
        VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = renderPass;
        renderPassInfo.framebuffer = framebuffers[i];
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = swapchainExtent;
        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues = &clearColor;

        vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, 
            VK_SUBPASS_CONTENTS_INLINE);
        {
            vkCmdBindPipeline(commandBuffers[i], 
                VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

            VkBuffer vertexBuffers[] = { vertexBuffer };
            VkDeviceSize offsets[] = { 0 };
            vkCmdBindVertexBuffers(commandBuffers[i], 0, 1, vertexBuffers, offsets);
            
            vkCmdDraw(commandBuffers[i], 3, 1, 0, 0);
        }
        vkCmdEndRenderPass(commandBuffers[i]);
        
        if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS)
            return false;
    }
    
    return true;
}

static bool InitSyncObjects()
{
    imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
    imagesInFlight.resize(swapchainImages.size(), VK_NULL_HANDLE);

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (uint64_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        if (vkCreateSemaphore(device.Get(), &semaphoreInfo, nullptr, 
            &imageAvailableSemaphores[i]) != VK_SUCCESS)
            return false;
        if (vkCreateSemaphore(device.Get(), &semaphoreInfo, nullptr, 
            &renderFinishedSemaphores[i]) != VK_SUCCESS)
            return false;
        if (vkCreateFence(device.Get(), &fenceInfo, nullptr, 
            &inFlightFences[i]) != VK_SUCCESS)
            return false;
    }

    return true;
}

static bool DrawFrame()
{
    vkWaitForFences(device.Get(), 1, &inFlightFences[currentFrame], VK_TRUE, 
        UINT64_MAX);
    
    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(device.Get(), 
        swapchain.Get(), UINT64_MAX, imageAvailableSemaphores[currentFrame], 
        VK_NULL_HANDLE, &imageIndex);
    if (result != VK_SUCCESS)
        return result == VK_SUBOPTIMAL_KHR 
            || result == VK_ERROR_OUT_OF_DATE_KHR;
    
    if (imagesInFlight[imageIndex] != VK_NULL_HANDLE)
        vkWaitForFences(device.Get(), 1, &imagesInFlight[imageIndex], VK_TRUE,
            UINT64_MAX);
    imagesInFlight[imageIndex] = inFlightFences[currentFrame];

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = { imageAvailableSemaphores[currentFrame] };
    VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[currentFrame] };
    VkPipelineStageFlags waitStages[] = {
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffers[imageIndex];
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;
    
    vkResetFences(device.Get(), 1, &inFlightFences[currentFrame]);
    if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, 
        inFlightFences[currentFrame]) != VK_SUCCESS)
        return false;
    
    VkSwapchainKHR swapchains[] = { swapchain.Get() };
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapchains;
    presentInfo.pImageIndices = &imageIndex;

    if (vkQueuePresentKHR(presentQueue, &presentInfo) 
        != VK_SUCCESS)
        return true;
    
    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    return true;
}

static bool RecreateSwapchain(uint64_t width, uint64_t height)
{
    if (width == 0 || height == 0)
        return true;
    
    device.WaitIdle();

    DisposeSwapchain();
    if (!InitSwapchain(width, height)
        || !InitRenderPass()
        || !InitShaders()
        || !InitPipeline()
        || !InitFramebuffers()
        || !InitCommandBuffers())
        return false;

    imagesInFlight.resize(swapchainImages.size(), VK_NULL_HANDLE);
    return true;
}

static void DisposeSwapchain()
{
    for (VkFramebuffer framebuffer : framebuffers)
        vkDestroyFramebuffer(device.Get(), framebuffer, nullptr);
    
    vkFreeCommandBuffers(device.Get(), commandPool, commandBuffers.size(), 
        commandBuffers.data());
    
    vkDestroyPipeline(device.Get(), pipeline, nullptr);
    vkDestroyPipelineLayout(device.Get(), pipelineLayout, nullptr);
    vkDestroyRenderPass(device.Get(), renderPass, nullptr);

    for (VkImageView imageView : swapchainImageViews)
        vkDestroyImageView(device.Get(), imageView, nullptr);

    swapchain.Dispose();
}

static void Dispose()
{
    DisposeSwapchain();

    vkDestroyBuffer(device.Get(), vertexBuffer, nullptr);
    vkFreeMemory(device.Get(), vertexBufferMemory, nullptr);

    for (uint64_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        vkDestroySemaphore(device.Get(), renderFinishedSemaphores[i], nullptr);
        vkDestroySemaphore(device.Get(), imageAvailableSemaphores[i], nullptr);
        vkDestroyFence(device.Get(), inFlightFences[i], nullptr);
    }

    vkDestroyCommandPool(device.Get(), commandPool, nullptr);

    device.Dispose();
    surface.Dispose();
    vulkan.Dispose();

    glfwDestroyWindow(window);
    glfwTerminate();

    glslang::FinalizeProcess();
}

static bool FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties, uint32_t* pMemoryType)
{
    if (!pMemoryType)
        return false;
    
    VkPhysicalDeviceMemoryProperties memoryProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);

    for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++)
        if (typeFilter & (1 << i) 
            && (memoryProperties.memoryTypes[i].propertyFlags & properties) == properties)
        {
            *pMemoryType = i;
            return true;
        }
    
    return false;
}

int main()
{
    if (!Init())
        return 1;

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        if (!DrawFrame())
        {
            std::cout << "Failed to draw frame" << std::endl;
            return 1;
        }
    }
    
    device.WaitIdle();

    Dispose();
    return 0;
}