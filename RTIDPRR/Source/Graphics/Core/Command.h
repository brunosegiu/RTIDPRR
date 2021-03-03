#pragma once

#include <vulkan/vulkan.hpp>

#include "../Geometry/IndexedVertexBuffer.h"
#include "ComputePipeline.h"
#include "Pipeline.h"
#include "Shaders/ShaderParameterGroup.h"

namespace RTIDPRR {
namespace Graphics {
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
};
}  // namespace Graphics
}  // namespace RTIDPRR
