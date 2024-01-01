#include "VulkanInstance.h"
 VKAPI_ATTR VkBool32 VKAPI_CALL VulkanDebugCallBack(VkDebugUtilsMessageSeverityFlagBitsEXT messageseverity, VkDebugUtilsMessageTypeFlagsEXT messagetype, const VkDebugUtilsMessengerCallbackDataEXT* pcallbackdata, void* puserData);
 VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);

 VkInstance VulkanInstance::CreateInstance(InstanceDesc& desc, VkDebugUtilsMessengerEXT* out_messenger)
{
    std::vector<const char*> instanceExtensiosn{};
    std::vector<const char*> LayerNames{};

    uint32_t ExtensionCount{};
    const char** glfwExtensions{};

    VkInstance Instance{};

    VkApplicationInfo appinfo{};
    appinfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appinfo.pApplicationName = "Vulkan App";
    appinfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appinfo.pEngineName = "No engine";
    appinfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appinfo.apiVersion = desc.ApiVersion;

 


    glfwExtensions = glfwGetRequiredInstanceExtensions(&ExtensionCount);


    for (uint32_t i = 0; i < ExtensionCount; i++) {
        instanceExtensiosn.push_back(glfwExtensions[i]);
    }
    for (uint64_t i = 0; i < desc.AdditionalExtensions.size(); i++) {
        instanceExtensiosn.push_back(desc.AdditionalExtensions[i]);
    }
    for (uint64_t i = 0; i < desc.LayerNames.size(); i++) {
        LayerNames.push_back(desc.LayerNames[i]);
    }

    if (desc.ValidationLayersEnabled) {
    instanceExtensiosn.push_back("VK_EXT_debug_utils");
        
    }
    LayerNames.push_back("VK_LAYER_KHRONOS_validation");

    if (!IsExtensionsSupported(instanceExtensiosn))
        Core::Log(ErrorType::Error, "Wanted extensions not supported by your machine!");



    VkInstanceCreateInfo createinfo{};
    createinfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createinfo.pApplicationInfo = &appinfo;
    createinfo.enabledExtensionCount = (uint32_t)instanceExtensiosn.size();
    createinfo.ppEnabledExtensionNames = instanceExtensiosn.data();
    //Add check if layers is supported.
    createinfo.ppEnabledLayerNames = LayerNames.data();
    createinfo.enabledLayerCount = 1;


    VkResult result = vkCreateInstance(&createinfo, nullptr, &Instance);

    if (result != VK_SUCCESS)
        Core::Log(ErrorType::Error, "Failed to create instance");
    if (desc.ValidationLayersEnabled)
        InitializeValidationLayers(Instance,out_messenger);
    return Instance;
}

 VkPhysicalDevice VulkanInstance::GetPhysicalDevice(VkInstance instance,VkSurfaceKHR surface)
 {
     uint32_t DeviceCount{};
     std::vector<VkPhysicalDevice> Devices{};

     vkEnumeratePhysicalDevices(instance, &DeviceCount, nullptr);
     Devices.resize(DeviceCount);
     vkEnumeratePhysicalDevices(instance, &DeviceCount, Devices.data());

     for (uint32_t i = 0; i < DeviceCount; i++) {

         if (IsPhysicalDeviceSiutable(Devices[i],surface))
             return Devices[i];

     }

     Core::Log(ErrorType::Info, "Failed to get physical device.");
     return VkPhysicalDevice();
 }

 VkDevice VulkanInstance::CreateLogicalDevice()
 {
     return VkDevice();
 }

bool VulkanInstance::IsExtensionsSupported(std::vector<const char*>& extensions)
{
    uint32_t SupportedExCount{};
    std::vector<VkExtensionProperties> Props{};

    vkEnumerateInstanceExtensionProperties(nullptr, &SupportedExCount, nullptr);
    Props.resize(SupportedExCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &SupportedExCount, Props.data());


    for (int i = 0; i < extensions.size(); i++) {
        bool HasExt{ false };

        for (int j = 0; j < Props.size(); j++) {

            if (strcmp(extensions[i], Props[j].extensionName) == 0)
            {
                HasExt = true;
                break;
            }

        }
        if (!HasExt)
            return false;
    }

    return true;

}

void VulkanInstance::InitializeValidationLayers(VkInstance instance,VkDebugUtilsMessengerEXT* messenger)
{
    VkDebugUtilsMessengerCreateInfoEXT creatinfo{};
    creatinfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    creatinfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
    creatinfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    creatinfo.pfnUserCallback = VulkanDebugCallBack;
    creatinfo.pUserData = nullptr;





    VkResult result = CreateDebugUtilsMessengerEXT(instance, &creatinfo, nullptr,messenger);
    if (result != VK_SUCCESS)
        Core::Log(ErrorType::Error, "Failed to create debug messenger.");
}

bool VulkanInstance::IsPhysicalDeviceSiutable(VkPhysicalDevice device,VkSurfaceKHR surface)
{
    SwapChainDetails swapchainDetails = SwapChain::GetSwapChainCapabilities(device,surface);
    QueueFamilies queueFamilies = GetQueueFamilies(device, surface);

    if (!SwapChain::IsSwapChainValid(swapchainDetails)) {
       Core::Log(ErrorType::Error, "SwapChain doesn't support any present modes!");
       return false;
    }
    if (!queueFamilies.IsValid()) {
        Core::Log(ErrorType::Error, "PhysicalDevice doesn't support the required queue families.");
        return false;
    }
    if (!CheckPhysicalDeviceSupportReqExtensions(device)) {
        Core::Log(ErrorType::Error, "Your GPU doesn't support the neccesary extensions!");
        return false;
    }

    return true;
}

bool VulkanInstance::CheckPhysicalDeviceSupportReqExtensions(VkPhysicalDevice device)
{
    uint32_t ExtCount{};
    std::vector<VkExtensionProperties> DeviceExts{};

    vkEnumerateDeviceExtensionProperties(device, nullptr, &ExtCount, nullptr);
    DeviceExts.resize(ExtCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &ExtCount, DeviceExts.data());

    if (ExtCount == 0)
        Core::Log(ErrorType::Error, "This device doesn't support any extensions.");

    for (uint64_t i = 0; i < g_DeviceExtensions.size(); i++) {
        bool HasExtension{};
        for (uint32_t j = 0; j < ExtCount; j++) {
            if (strcmp(g_DeviceExtensions[i], DeviceExts[j].extensionName))
                HasExtension = true;
        }
        if (!HasExtension) {

            return false;
            Core::Log(ErrorType::Error, "Device doesn't support the required extensions");
        }
    }
    return true;
}

QueueFamilies VulkanInstance::GetQueueFamilies(VkPhysicalDevice device,VkSurfaceKHR surface)
{
    QueueFamilies Families{};

    uint32_t QueueFamCount{};
    std::vector<VkQueueFamilyProperties> Properties{};

    vkGetPhysicalDeviceQueueFamilyProperties(device, &QueueFamCount, nullptr);
    Properties.resize(QueueFamCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &QueueFamCount, Properties.data());


    for (uint32_t i = 0; i < QueueFamCount; i++) {
        if (Properties[i].queueCount > 0 && Properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT);
        {

            Families.Graphics = i;
        }
        VkBool32 Presentation = false;

        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &Presentation);
        if (Properties[i].queueCount > 0 && Presentation)
            Families.Presentation = i;

        //Core::Log(ErrorType::Error, Families.Graphics, " ", Families.Presentation);

        if (Families.IsValid()) {

            return Families;
        }
    }
    Families.Presentation = 0;
    return Families;
}

VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
{
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    }
    else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}
 VKAPI_ATTR VkBool32 VKAPI_CALL VulkanDebugCallBack(VkDebugUtilsMessageSeverityFlagBitsEXT messageseverity, VkDebugUtilsMessageTypeFlagsEXT messagetype, const VkDebugUtilsMessengerCallbackDataEXT* pcallbackdata, void* puserData) {



    switch (messageseverity) {
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT: {
        Core::Log(ErrorType::Info, pcallbackdata->pMessage);

        break;
    }
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT: {
        Core::Log(ErrorType::Error, pcallbackdata->pMessage);

        break;
    }
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT: {
        Core::Log(ErrorType::Warning, pcallbackdata->pMessage);

        break;
    }
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT: {
        Core::Log(ErrorType::Info, pcallbackdata->pMessage);

        break;
    }
    }

    return VK_FALSE;
}