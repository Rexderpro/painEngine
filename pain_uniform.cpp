#include "pain_uniform.h"
#include "pain_debug.h"
#include <cstring>

namespace Pain {

// Identity matrix (column-major, same as GLM's default)
static const float kIdentity[16] = {
  1,0,0,0,
  0,1,0,0,
  0,0,1,0,
  0,0,0,1
};

PainUniform::PainUniform(PainDevice& device, VkDescriptorSetLayout setLayout)
  : painDevice(device)
{
  createUniformBuffer();
  createDescriptorPool();
  allocateAndWriteDescriptorSet(setLayout);
}

// ── 1. Buffer: 3 × mat4 = 3 × 64 = 192 bytes ─────────────────────────────
void PainUniform::createUniformBuffer() {
  constexpr VkDeviceSize bufferSize = sizeof(float) * 16 * 3; // model + view + proj

  painDevice.createBuffer(
    bufferSize,
    VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
    m_UniformBuffer,
    m_UniformBufferMemory
  );

  // ── 2. Map and copy three identity matrices ──────────────────────────────
  void* data;
  vkMapMemory(painDevice.device(), m_UniformBufferMemory, 0, bufferSize, 0, &data);
    memcpy(static_cast<char*>(data) + 0,   kIdentity, 64); // model
    memcpy(static_cast<char*>(data) + 64,  kIdentity, 64); // view
    memcpy(static_cast<char*>(data) + 128, kIdentity, 64); // projection
  vkUnmapMemory(painDevice.device(), m_UniformBufferMemory);
  // HOST_COHERENT means no explicit flush needed
}

// ── 3. Descriptor Pool ────────────────────────────────────────────────────
void PainUniform::createDescriptorPool() {
  VkDescriptorPoolSize poolSize{};
  poolSize.type            = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  poolSize.descriptorCount = 1;

  VkDescriptorPoolCreateInfo poolInfo{};
  poolInfo.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  poolInfo.poolSizeCount = 1;
  poolInfo.pPoolSizes    = &poolSize;
  poolInfo.maxSets       = 1;

  VkResult result = vkCreateDescriptorPool(painDevice.device(), &poolInfo, nullptr, &m_DescriptorPool);
  ensure(result, "failed to create descriptor pool!");
}

// ── 4. Allocate Set + vkUpdateDescriptorSets ─────────────────────────────
void PainUniform::allocateAndWriteDescriptorSet(VkDescriptorSetLayout setLayout) {
  VkDescriptorSetAllocateInfo allocInfo{};
  allocInfo.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  allocInfo.descriptorPool     = m_DescriptorPool;
  allocInfo.descriptorSetCount = 1;
  allocInfo.pSetLayouts        = &setLayout;

  VkResult result = vkAllocateDescriptorSets(painDevice.device(), &allocInfo, &m_DescriptorSet);
  ensure(result, "failed to allocate descriptor set!");

  // Point the descriptor at our buffer
  VkDescriptorBufferInfo bufferInfo{};
  bufferInfo.buffer = m_UniformBuffer;
  bufferInfo.offset = 0;
  bufferInfo.range  = sizeof(float) * 16 * 3; // whole buffer

  VkWriteDescriptorSet write{};
  write.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  write.dstSet          = m_DescriptorSet;
  write.dstBinding      = 1;                              // matches binding=1 in layout
  write.dstArrayElement = 0;
  write.descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  write.descriptorCount = 1;
  write.pBufferInfo     = &bufferInfo;

  vkUpdateDescriptorSets(painDevice.device(), 1, &write, 0, nullptr);
}

PainUniform::~PainUniform() {
  // Pool destruction implicitly frees the set allocated from it
  vkDestroyDescriptorPool(painDevice.device(), m_DescriptorPool, nullptr);
  vkDestroyBuffer(painDevice.device(), m_UniformBuffer, nullptr);
  vkFreeMemory(painDevice.device(), m_UniformBufferMemory, nullptr);
}

} // namespace Pain