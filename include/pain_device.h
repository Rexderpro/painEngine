#pragma once

#include "pain_window.h"

#include <vector>
#include <iostream>
#include <optional>
#include <vulkan/vulkan_core.h>
#include <string>

namespace Pain
{
  struct QueueInfo
  {
    uint32_t graphicsFamily;
    uint32_t presentFamily;
    bool graphicsFamilyHasValue = false;
    bool presentFamilyHasValue = false;
    bool isComplete() { return graphicsFamilyHasValue && presentFamilyHasValue; }
  };

  struct SwapchainSupportDetails
  {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentMode;
  };

  // TODO create surface without glfw.
  // struct WindowParams {
  // #ifdef VK_USE_PLATFORM_WIN32_KHR
  //   HINSTANCE HInstance;
  //   HWND HWnd;
  // #elif defined VK_USE_PLATFORM_XLIB_KHR
  //   Display* Dpy;
  //   Window Window;
  // #elif defined VK_USE_PLATFORM_XCB_KHR
  //   xcb_connection_t* Connection;
  //   xcb_window_t window;
  // #endif
  // };

  class PainDevice
  {
  public:
    PainDevice(PainWindow& window);
    ~PainDevice();

    VkDevice device() { return m_Device; }
    VkPhysicalDevice pdevice() { return m_PhysicalDevice; }
    VkSurfaceKHR surface() { return m_Surface; }
    SwapchainSupportDetails getSwapChainSupport() { return querySwapchainSupport(); }
    VkCommandPool getCommandPool() { return m_CommandPool; }
    VkQueue graphicsQueue() { return m_GraphicsQueue; }
    VkQueue presentQueue() { return m_PresentQueue; }
    QueueInfo findPhysicalQueueFamilies() { return pickQueueFamily(m_PhysicalDevice); }
    VkFormat findSupportedFormat(const std::vector<VkFormat> &candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
    void createImageWithInfo(const VkImageCreateInfo &imageInfo, VkMemoryPropertyFlags properties, VkImage &image, VkDeviceMemory &imageMemory);
    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

    void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);

  private:
    void createInstance();

    void pickPhysicalDevice();
    bool isDeviceGud(VkPhysicalDevice device);
    bool isDeviceSupportingDesiredExtensions(VkPhysicalDevice device);
    QueueInfo pickQueueFamily(VkPhysicalDevice device);

    void setupDebugMessenger();
    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

    void createLogicalDevice();
    bool checkDeviceExtensionSupport();

    void createCommandPool();
    void createSurface();

    void createSwapchain();
    SwapchainSupportDetails querySwapchainSupport();
    VkSurfaceFormatKHR chooseSwapSurfaceFormat();
    VkPresentModeKHR chooseSwapPresentMode();
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
    VkSurfaceCapabilitiesKHR getSurfaceCapabilities();

    void createImageViews();
    void createDepthResources();
    void createRenderPass();
    void createFramebuffers();
    void createSyncObjects();

    // DBG
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
      VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
      VkDebugUtilsMessageTypeFlagsEXT messageType,
      const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
      void* UserData
    )
    {
      std::cerr << "Validation Layer: " << pCallbackData->pMessage << std::endl;
      return VK_FALSE;
    }

    VkResult CreateDebugUtilsMessengerEXT(
      VkInstance instance,
      const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
      const VkAllocationCallbacks* pAllocator,
      VkDebugUtilsMessengerEXT* pDebugMessenger
    );

    void DestroyDebugUtilsMessengerEXT(
      VkInstance instance,
      VkDebugUtilsMessengerEXT debugMessenger,
      const VkAllocationCallbacks* pAllocator
    );

    // === Members ===
    PainWindow& m_Window;

    // DBG
    VkDebugUtilsMessengerEXT debugMessenger;

    // Important Vulkan Context members
    const char* m_Layers[1] = {"VK_LAYER_KHRONOS_validation"};
    std::vector<std::string> m_DeviceExtensions = {
      VK_KHR_SWAPCHAIN_EXTENSION_NAME,
      "VK_KHR_portability_subset"
  };
    VkInstance m_Instance{};
    VkPhysicalDevice m_PhysicalDevice{}; // The device which vulkan is going to use.
    VkDevice m_Device{};
    VkQueue m_GraphicsQueue{};
    VkQueue m_PresentQueue{};
    VkSurfaceKHR m_Surface{};

    VkSwapchainKHR m_Swapchain;
    VkFormat m_SwapchainImageFormat;
    VkExtent2D m_SwapchainExtent;

    std::vector<VkImage> depthImages;
    std::vector<VkDeviceMemory> depthDeviceMemorys;
    std::vector<VkImageView> depthImageMemorys;
    std::vector<VkImage> m_SwapchainImages;
    std::vector<VkImageView> m_ImageViews;

    VkCommandPool m_CommandPool{};
  };
}
