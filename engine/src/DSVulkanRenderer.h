#pragma once
#include "DSBaseRenderer.h"
#include <vulkan/vulkan.h>
#include <vector>
#include <memory>
#include <optional>

namespace DSEngine {
    class DSVulkanRenderer : public DSBaseRenderer {
    public:
        ~DSVulkanRenderer() override;
        bool Init(const RendererConfig& config) override;
        void Clear(float r, float g, float b, float a, bool clearDepth) override;
        void Frame() override;
        void Resize(uint32_t width, uint32_t height) override;

    private:
        // Core Vulkan Objects
        VkInstance m_instance = VK_NULL_HANDLE;
        VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
        VkDevice m_device = VK_NULL_HANDLE;
        VkQueue m_graphicsQueue = VK_NULL_HANDLE;
        VkQueue m_presentQueue = VK_NULL_HANDLE;
        VkSurfaceKHR m_surface = VK_NULL_HANDLE;
        VkSwapchainKHR m_swapchain = VK_NULL_HANDLE;
        VkCommandPool m_commandPool = VK_NULL_HANDLE;
        std::vector<VkCommandBuffer> m_commandBuffers;
        VkRenderPass m_renderPass = VK_NULL_HANDLE;
        std::vector<VkFramebuffer> m_swapchainFramebuffers;
        VkSemaphore m_imageAvailableSemaphore = VK_NULL_HANDLE;
        VkSemaphore m_renderFinishedSemaphore = VK_NULL_HANDLE;
        VkFence m_inFlightFence = VK_NULL_HANDLE;

        // Debug
        VkDebugUtilsMessengerEXT m_debugMessenger = VK_NULL_HANDLE;

        // Helper Structs
        struct QueueFamilyIndices {
            std::optional<uint32_t> graphicsFamily;
            std::optional<uint32_t> presentFamily;
            bool isComplete() const {
                return graphicsFamily.has_value() && presentFamily.has_value();
            }
        };

        struct SwapchainSupportDetails {
            VkSurfaceCapabilitiesKHR capabilities;
            std::vector<VkSurfaceFormatKHR> formats;
            std::vector<VkPresentModeKHR> presentModes;
        };

        // Internal Methods
        void _CreateInstance();
        void _SetupDebugMessenger();
        void _CreateSurface();
        void _PickPhysicalDevice();
        void _CreateLogicalDevice();
        void _CreateSwapchain();
        void _CreateImageViews();
        void _CreateRenderPass();
        void _CreateFramebuffers();
        void _CreateCommandPool();
        void _CreateCommandBuffers();
        void _CreateSyncObjects();
        void _RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
        void _CleanupSwapchain();
        void _RecreateSwapchain();

        QueueFamilyIndices _FindQueueFamilies(VkPhysicalDevice device);
        SwapchainSupportDetails _QuerySwapchainSupport(VkPhysicalDevice device);
        VkSurfaceFormatKHR _ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
        VkPresentModeKHR _ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
        VkExtent2D _ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

        // Configuration
        RendererConfig m_config;
        uint32_t m_currentFrame = 0;
    };
}