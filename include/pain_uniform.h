#pragma once
#include "pain_device.h"
#include <vulkan/vulkan.h>

namespace Pain {

    // Layout of our UBO — 3 x mat4 (model, view, projection)
    struct CameraUBO {
        float model[16];
        float view[16];
        float projection[16];
    };

    class PainUniform {
    public:
        PainUniform(PainDevice& device, VkDescriptorSetLayout setLayout);
        ~PainUniform();

        PainUniform(const PainUniform&) = delete;
        PainUniform& operator=(const PainUniform&) = delete;

        const VkDescriptorSet& descriptorSet() const { return m_DescriptorSet; }

    private:
        void createUniformBuffer();
        void createDescriptorPool();
        void allocateAndWriteDescriptorSet(VkDescriptorSetLayout setLayout);

        PainDevice&       painDevice;

        VkBuffer          m_UniformBuffer       = VK_NULL_HANDLE;
        VkDeviceMemory    m_UniformBufferMemory  = VK_NULL_HANDLE;

        VkDescriptorPool  m_DescriptorPool      = VK_NULL_HANDLE;
        VkDescriptorSet   m_DescriptorSet       = VK_NULL_HANDLE;
    };

} // namespace Pain