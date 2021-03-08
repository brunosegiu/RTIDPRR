#pragma once

#include <vulkan/vulkan.hpp>

#include "../Geometry/IndexedVertexBuffer.h"
#include "ComputePipeline.h"
#include "Pipeline.h"
#include "Shaders/ShaderParameterGroup.h"

namespace RTIDPRR {
namespace Graphics {

enum class ResourceTransition {
  GraphicsToGraphics,
  ComputeToCompute,
  ComputeToGraphics,
  GraphicsToCompute
};

class Command : public vk::CommandBuffer {
 public:
  Command() = default;
  Command(vk::CommandBuffer& other) : vk::CommandBuffer(other) {}

  void bindPipeline(const Pipeline& pipeline) {
    vk::CommandBuffer::bindPipeline(vk::PipelineBindPoint::eGraphics,
                                    pipeline.getPipelineHandle());
  }

  void bindPipeline(const ComputePipeline& pipeline) {
    vk::CommandBuffer::bindPipeline(vk::PipelineBindPoint::eCompute,
                                    pipeline.getPipelineHandle());
  }

  void bindInlineShaderParameters() {}

  template <typename... T>
  void bindShaderParameterGroup(
      uint32_t slot, const Pipeline& pipeline,
      const ShaderParameterGroup<T...>& shaderParameterGroup) {
    bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
                       pipeline.getPipelineLayout(), slot,
                       shaderParameterGroup.getDescriptorSet(), nullptr);
  }

  template <typename ShaderParameter>
  void bindShaderParameterGroup(uint32_t slot, const ComputePipeline& pipeline,
                                const ShaderParameter& shaderParameter) {
    bindDescriptorSets(vk::PipelineBindPoint::eCompute,
                       pipeline.getPipelineLayout(), slot,
                       shaderParameter.getDescriptorSet(), nullptr);
  }

  template <typename... T>
  void bindShaderParameterGroup(
      uint32_t slot, const ComputePipeline& pipeline,
      const ShaderParameterGroup<T...>& shaderParameterGroup) {
    bindDescriptorSets(vk::PipelineBindPoint::eCompute,
                       pipeline.getPipelineLayout(), slot,
                       shaderParameterGroup.getDescriptorSet(), nullptr);
  }

  void bindMesh(const IndexedVertexBuffer& mesh) {
    vk::DeviceSize offset{0};
    bindVertexBuffers(0, mesh.getVertexBuffer().getBufferHandle(), offset);
    bindIndexBuffer(mesh.getIndexBuffer().getBufferHandle(), offset,
                    IndexedVertexBuffer::getIndexType());
  }

  void drawMesh(const IndexedVertexBuffer& mesh) {
    drawIndexed(mesh.getFaceCount(), 1, 0, 0, 0);
  }

  void transitionTextures(ResourceTransition transitionType,
                          vk::PipelineStageFlags srcStage,
                          vk::PipelineStageFlags dstStage,
                          vk::ImageLayout prevLayout, vk::ImageLayout newLayout,
                          vk::AccessFlags newAccess,
                          const std::vector<const Texture*>& textures) {
    uint32_t srcFamilyIndex =
        transitionType == ResourceTransition::ComputeToGraphics
            ? Context::get().getDevice().getComputeQueue().getFamilyIndex()
            : Context::get().getDevice().getGraphicsQueue().getFamilyIndex();

    uint32_t dstFamilyIndex =
        transitionType == ResourceTransition::ComputeToGraphics
            ? Context::get().getDevice().getGraphicsQueue().getFamilyIndex()
            : Context::get().getDevice().getComputeQueue().getFamilyIndex();

    std::vector<vk::ImageMemoryBarrier> barriers(textures.size());
    for (size_t index = 0; index < barriers.size(); ++index) {
      vk::ImageMemoryBarrier& barrier = barriers[index];
      const Texture* texture = textures[index];
      barrier.setOldLayout(prevLayout)
          .setNewLayout(newLayout)
          .setImage(texture->getImage())
          .setDstAccessMask(newAccess)
          .setSubresourceRange(
              vk::ImageSubresourceRange()
                  .setAspectMask(
                      texture->getUsage() &
                              vk::ImageUsageFlagBits::eDepthStencilAttachment
                          ? vk::ImageAspectFlagBits::eDepth
                          : vk::ImageAspectFlagBits::eColor)
                  .setBaseMipLevel(0)
                  .setLevelCount(1)
                  .setLayerCount(1));
      if (transitionType != ResourceTransition::GraphicsToGraphics &&
          transitionType != ResourceTransition::ComputeToCompute) {
        barrier.setSrcQueueFamilyIndex(srcFamilyIndex)
            .setDstQueueFamilyIndex(dstFamilyIndex);
      }
    }

    pipelineBarrier(srcStage, dstStage, {}, nullptr, nullptr, barriers);
  }

  void transitionTexture(ResourceTransition transitionType,
                         vk::PipelineStageFlags srcStage,
                         vk::PipelineStageFlags dstStage,
                         vk::ImageLayout prevLayout, vk::ImageLayout newLayout,
                         vk::AccessFlags newAccess, const Texture* texture) {
    transitionTextures(transitionType, srcStage, dstStage, prevLayout,
                       newLayout, newAccess, {texture});
  }
};
}  // namespace Graphics
}  // namespace RTIDPRR
