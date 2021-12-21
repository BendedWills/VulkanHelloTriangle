#include <VulkanHelloTriangle/Graphics/Vulkan/Instance.hpp>

using namespace Vulkan;

bool Device::Init(
    Instance* pInstance,
    VkPhysicalDevice physicalDevice,
    VkDeviceQueueCreateInfo* queueFamilies, 
    uint64_t queueFamilyCount, 
    VkPhysicalDeviceFeatures features,
    const char *const * extentions,
    uint64_t extentionCount
)
{
    if (initialized)
        return false;
    
    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    createInfo.pQueueCreateInfos = queueFamilies;
    createInfo.queueCreateInfoCount = queueFamilyCount;
    createInfo.pEnabledFeatures = &features;

    createInfo.enabledExtensionCount = extentionCount;
    createInfo.ppEnabledExtensionNames = extentions;

    if (pInstance->IsDebugMode())
    {
        // Enable validation layers

        createInfo.enabledLayerCount   = pInstance->validationLayers.size();
        createInfo.ppEnabledLayerNames = pInstance->validationLayers.data();
    }
    else
        createInfo.enabledLayerCount = 0;

    // Create the device
    if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device)
        != VK_SUCCESS)
        return false;
    
    initialized = true;
    return true;
}

Queue Device::GetDeviceQueue(uint32_t familyIndex, uint32_t queueIndex)
{
    VkQueue queue;
    vkGetDeviceQueue(device, familyIndex, queueIndex, &queue);

    return Queue(&queue);
}

VkDevice* Device::GetDevice()
{
    return &device;
}

void Device::OnDispose()
{
    vkDestroyDevice(device, nullptr);
}