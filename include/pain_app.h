#pragma once
#include "pain_window.h"
#include "pain_pipeline.h"
#include "pain_device.h"
#include "pain_swapchain.h"
#include "pain_model.h"
#include <memory>
#include <vector>
#include "pain_uniform.h"

namespace Pain {
  class TestApp {
    public:
      static constexpr int WIDTH = 800;
      static constexpr int HEIGHT = 600;

      TestApp();
      ~TestApp();

      TestApp(const TestApp&) = delete;
      TestApp &operator=(const TestApp&) = delete;

      void run();

    private:
      void loadModels();
      void createPipelineLayout();
      void createPipeline();
      void createCommandBuffers();
      void draw();

      PainWindow painWindow{WIDTH, HEIGHT, "Vulkan"};
      PainDevice painDevice{painWindow};
      PainSwapchain painSwapchain{painDevice, painWindow.getExtent()};
      std::unique_ptr<PainPipeline> painPipeline;
      VkPipelineLayout pipelineLayout;
      std::vector<VkCommandBuffer> commandBuffers;
      std::unique_ptr<PainModel> painModel;
      VkDescriptorSetLayout        descriptorSetLayout = VK_NULL_HANDLE; // promoted from local
      std::unique_ptr<PainUniform> painUniform;
  };
}
