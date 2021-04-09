#include "Instance.h"

#include <iostream>
#include <string>

#include "../../Misc/DebugUtils.h"

using namespace RTIDPRR::Graphics;

const std::string APP_NAME = "RTDIRR";

#if defined(_DEBUG)
static VKAPI_ATTR VkBool32 VKAPI_CALL vkDebugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* callbackData, void* userData) {
  LOG(callbackData->pMessage);
  RTIDPRR_ASSERT_MSG(
      messageSeverity != VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT &&
          messageSeverity != VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT,
      callbackData->pMessage);
  return VK_FALSE;
}
#endif

Instance::Instance(const Window& window) {
  vk::ApplicationInfo appInfo = vk::ApplicationInfo()
                                    .setPApplicationName(APP_NAME.data())
                                    .setApplicationVersion(1)
                                    .setPEngineName(APP_NAME.data())
                                    .setEngineVersion(1)
                                    .setApiVersion(VK_API_VERSION_1_0);

  std::vector<const char*> layers {
#if defined(_DEBUG)
    "VK_LAYER_KHRONOS_validation"
#endif
  };

  mAreMarkersSupported = false;

  std::vector<const char*> extensions = window.getRequiredVkExtensions();

  {
    const std::vector<const char*> debugExtensions{
        VK_EXT_DEBUG_REPORT_EXTENSION_NAME, VK_EXT_DEBUG_UTILS_EXTENSION_NAME};

    extensions.insert(extensions.end(), debugExtensions.begin(),
                      debugExtensions.end());
    mAreMarkersSupported = true;
  }

  std::vector<vk::ExtensionProperties> supportedExtensions =
      RTIDPRR_ASSERT_VK(vk::enumerateInstanceExtensionProperties());

  auto _layers = RTIDPRR_ASSERT_VK(vk::enumerateInstanceLayerProperties());
  for (auto& layer : _layers) {
      LOG(layer.layerName);
  }

  for (const char* extensionName : extensions) {
    const bool isExtensionsSupported =
        std::find_if(
            supportedExtensions.begin(), supportedExtensions.end(),
            [&extensionName](const vk::ExtensionProperties& presentExtension) {
              return strcmp(extensionName, presentExtension.extensionName) == 0;
            }) != supportedExtensions.end();
    RTIDPRR_ASSERT(isExtensionsSupported);
  }

  vk::InstanceCreateInfo instanceInfo =
      vk::InstanceCreateInfo()
          .setFlags(vk::InstanceCreateFlags())
          .setPApplicationInfo(&appInfo)
          .setEnabledExtensionCount(static_cast<uint32_t>(extensions.size()))
          .setPpEnabledExtensionNames(extensions.data())
          .setEnabledLayerCount(static_cast<uint32_t>(layers.size()))
          .setPpEnabledLayerNames(layers.data());

  mInstanceHandle = RTIDPRR_ASSERT_VK(vk::createInstance(instanceInfo));

  mDynamicDispatcher =
      vk::DispatchLoaderDynamic(mInstanceHandle, vkGetInstanceProcAddr);
  mDynamicDispatcher.init(mInstanceHandle, vkGetInstanceProcAddr);

#if defined(_DEBUG)

  vk::DebugUtilsMessengerCreateInfoEXT debugCallbackCreateInfo =
      vk::DebugUtilsMessengerCreateInfoEXT()
          .setMessageSeverity(
              vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
              vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
              vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
              vk::DebugUtilsMessageSeverityFlagBitsEXT::eError)
          .setMessageType(vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
                          vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
                          vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance)
          .setPfnUserCallback(vkDebugCallback);

  mDebugMessenger =
      RTIDPRR_ASSERT_VK(mInstanceHandle.createDebugUtilsMessengerEXT(
          debugCallbackCreateInfo, nullptr, mDynamicDispatcher));
#endif
}

Instance::~Instance() {
#if defined(_DEBUG)
  mInstanceHandle.destroyDebugUtilsMessengerEXT(mDebugMessenger, nullptr,
                                                mDynamicDispatcher);
#endif
  mInstanceHandle.destroy();
}
