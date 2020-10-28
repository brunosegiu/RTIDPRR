#include "Instance.h"

#include <string>
#include <cassert>
#include <iostream>

using namespace RTIDPRR::Graphics;

const std::string APP_NAME = "RTDIRR";

#if defined(_DEBUG)
static VKAPI_ATTR VkBool32 VKAPI_CALL vkDebugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
	void* userData) {
	std::cerr << callbackData->pMessage << std::endl;
	return VK_FALSE;
}

VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pCallback) {
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr) {
		return func(instance, pCreateInfo, pAllocator, pCallback);
	}
	else {
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT callback, const VkAllocationCallbacks* pAllocator) {
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr) {
		func(instance, callback, pAllocator);
	}
}
#endif

Instance::Instance(const SDL_Window* window) {

	vk::ApplicationInfo appInfo =
		vk::ApplicationInfo()
		.setPApplicationName(APP_NAME.data())
		.setApplicationVersion(1)
		.setPEngineName(APP_NAME.data())
		.setEngineVersion(1)
		.setApiVersion(VK_API_VERSION_1_0);

	std::vector<const char*> layers{
#if defined(_DEBUG)
	"VK_LAYER_KHRONOS_validation"
#endif
	};

	std::vector<const char*> extensions = Instance::getRequiredExtensions(window);

#if defined(_DEBUG)
	const std::vector<const char*> debugExtensions{
		VK_EXT_DEBUG_REPORT_EXTENSION_NAME,
		VK_EXT_DEBUG_UTILS_EXTENSION_NAME
	};

	extensions.insert(extensions.end(), debugExtensions.begin(), debugExtensions.end());
#endif

	vk::InstanceCreateInfo instanceInfo =
		vk::InstanceCreateInfo()
		.setFlags(vk::InstanceCreateFlags())
		.setPApplicationInfo(&appInfo)
		.setEnabledExtensionCount(static_cast<uint32_t>(extensions.size()))
		.setPpEnabledExtensionNames(extensions.data())
		.setEnabledLayerCount(static_cast<uint32_t>(layers.size()))
		.setPpEnabledLayerNames(layers.data());

	mInstanceHandle = vk::createInstance(instanceInfo);

#if defined(_DEBUG)
	vk::DispatchLoaderDynamic dynamicDispatcher = vk::DispatchLoaderDynamic(mInstanceHandle, vkGetInstanceProcAddr);

	dynamicDispatcher.init(mInstanceHandle, vkGetInstanceProcAddr);

	vk::DebugUtilsMessengerCreateInfoEXT debugCallbackCreateInfo =
		vk::DebugUtilsMessengerCreateInfoEXT()
		.setMessageSeverity(
			vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose
			| vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose
			| vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning
			| vk::DebugUtilsMessageSeverityFlagBitsEXT::eError)
		.setMessageType(
			vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral
			| vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation
			| vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance)
		.setPfnUserCallback(vkDebugCallback);

	mDebugMessenger = mInstanceHandle.createDebugUtilsMessengerEXT(debugCallbackCreateInfo, nullptr, dynamicDispatcher);
#endif
}

const std::vector<const char*> Instance::getRequiredExtensions(const SDL_Window* window) {
	unsigned int requiredExtensionsCount = 0;
	assert(SDL_Vulkan_GetInstanceExtensions(const_cast<SDL_Window*>(window), &requiredExtensionsCount, NULL));
	std::vector<const char*> extensions(requiredExtensionsCount);
	assert(SDL_Vulkan_GetInstanceExtensions(const_cast<SDL_Window*>(window), &requiredExtensionsCount, extensions.data()));
	return extensions;
}

const vk::SurfaceKHR Instance::initSurface(const SDL_Window* window) {
	VkSurfaceKHR surface;
	assert(SDL_Vulkan_CreateSurface(const_cast<SDL_Window*>(window), static_cast<VkInstance>(mInstanceHandle), &surface));
	return vk::SurfaceKHR(surface);
}

Instance::~Instance() {
#if defined(_DEBUG)
	//mInstanceHandle.destroyDebugUtilsMessengerEXT(mDebugMessenger, nullptr);
#endif
	mInstanceHandle.destroy();
}
