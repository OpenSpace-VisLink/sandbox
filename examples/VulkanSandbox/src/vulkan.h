#ifndef SANDBOX_VULCAN_H_
#define SANDBOX_VULCAN_H_

#include "sandbox/Component.h"
#include "sandbox/graphics/GraphicsContext.h"
#include "sandbox/graphics/RenderNode.h"
#include "sandbox/graphics/GraphicsRenderer.h"

#include "sandbox/graphics/vulkan/Vulkan.h"
#include "sandbox/graphics/vulkan/VulkanInstance.h"
#include "sandbox/graphics/vulkan/VulkanSurface.h"
#include "sandbox/graphics/vulkan/VulkanQueue.h"
#include "sandbox/graphics/vulkan/VulkanDevice.h"
#include "sandbox/graphics/vulkan/VulkanSwapChain.h"
#include "sandbox/graphics/vulkan/render/VulkanRenderPass.h"
#include "sandbox/graphics/vulkan/render/VulkanCommandPool.h"
#include "sandbox/graphics/vulkan/render/VulkanCommandBuffer.h"
#include "sandbox/graphics/vulkan/render/impl/VulkanBasicRenderPass.h"
#include "sandbox/graphics/vulkan/render/impl/VulkanSwapChainFramebuffer.h"
#include "sandbox/graphics/vulkan/render/VulkanFramebuffer.h"
#include "sandbox/graphics/vulkan/VulkanDeviceState.h"
#include "sandbox/graphics/vulkan/VulkanFrameRenderer.h"
#include "sandbox/graphics/vulkan/shader/VulkanShaderModule.h"
#include "sandbox/graphics/vulkan/image/VulkanImage.h"
#include "sandbox/graphics/vulkan/image/VulkanImageView.h"
#include "sandbox/graphics/vulkan/buffer/VulkanBuffer.h"
#include "sandbox/graphics/vulkan/buffer/VulkanVertexInput.h"
#include "sandbox/graphics/vulkan/shader/VulkanShaderObject.h"
#include "sandbox/graphics/vulkan/buffer/VulkanBufferTypes.h"
#include "sandbox/graphics/vulkan/image/VulkanSampler.h"



#include <iostream>
#include <fstream>


namespace sandbox {














class VulkanDescriptor : public VulkanComponent { //
	    //binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
public:
	VulkanDescriptor(VulkanShaderObject* shaderObject, VkShaderStageFlags shaderStageFlags) : shaderObject(shaderObject), shaderStageFlags(shaderStageFlags) { addType<VulkanDescriptor>(); }
	virtual ~VulkanDescriptor() {}

	void setBinding(VkDescriptorSetLayoutBinding& binding) {
		shaderObject->setBinding(binding);
		binding.stageFlags = shaderStageFlags;
	}

	void setPoolSize(VkDescriptorPoolSize& poolSize) {
		shaderObject->setPoolSize(poolSize);
	}

	void writeDescriptor(const GraphicsContext& context, VkWriteDescriptorSet& descriptorWrite, std::vector<VulkanShaderObject::DescriptorObject*>& descriptorObjects) {
	    descriptorWrite.dstArrayElement = 0;
		shaderObject->writeDescriptor(context, descriptorWrite, descriptorObjects);
	}

private:
	VulkanShaderObject* shaderObject;
	VkShaderStageFlags shaderStageFlags;
};


class VulkanDescriptorSetLayout : public VulkanRenderObject {
public:
	VulkanDescriptorSetLayout() { addType<VulkanDescriptorSetLayout>(); }
	virtual ~VulkanDescriptorSetLayout() {}

	VkDescriptorSetLayout getDescriptorSetLayout(const GraphicsContext& context) const { return contextHandler.getSharedState(context)->descriptorSetLayout; }

protected:
	void startRender(const GraphicsContext& context, VulkanDeviceState& state) {
		if (state.getRenderMode().get() == VULKAN_RENDER_UPDATE_SHARED) {
			DescriptorSetLayoutState* layoutState = contextHandler.getSharedState(context);
			std::vector<VulkanDescriptor*> descriptors = getEntity().getComponents<VulkanDescriptor>();
			std::cout << descriptors.size() << std::endl;
			std::vector<VkDescriptorSetLayoutBinding> bindings;
			for (int f = 0; f < descriptors.size(); f++) {
				VkDescriptorSetLayoutBinding binding = {};
				binding.binding = f;
				descriptors[f]->setBinding(binding);
				bindings.push_back(binding);
			}

	        VkDescriptorSetLayoutCreateInfo layoutInfo = {};
	        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	        layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
	        layoutInfo.pBindings = bindings.data();

	        if (vkCreateDescriptorSetLayout(state.getDevice()->getDevice(), &layoutInfo, nullptr, &layoutState->descriptorSetLayout) != VK_SUCCESS) {
	            throw std::runtime_error("failed to create descriptor set layout!");
	        }
			std::cout << "created descriptor layout "  << layoutState->descriptorSetLayout << std::endl;
		}
	}

	void finishRender(const GraphicsContext& context, VulkanDeviceState& state) {
		if (state.getRenderMode().get() == VULKAN_RENDER_CLEANUP_SHARED) {
			DescriptorSetLayoutState* layoutState = contextHandler.getSharedState(context);
			vkDestroyDescriptorSetLayout(state.getDevice()->getDevice(), layoutState->descriptorSetLayout, nullptr);
			std::cout << "destroy descriptor layout" << std::endl;
		}
	}


private:
	struct DescriptorSetLayoutState : public ContextState {
		VkDescriptorSetLayout descriptorSetLayout;
	};

	GraphicsContextHandler<DescriptorSetLayoutState,ContextState> contextHandler;
};

class VulkanDescriptorPool : public VulkanRenderObject {
public:
	VulkanDescriptorPool() { addType<VulkanDescriptorPool>(); }
	virtual ~VulkanDescriptorPool() {}

	virtual VkDescriptorPool getDescriptorPool(const GraphicsContext& context) const = 0;
};

class VulkanSwapChainDescriptorPool : public VulkanDescriptorPool {
public:
	VulkanSwapChainDescriptorPool() { addType<VulkanSwapChainDescriptorPool>(); }
	virtual ~VulkanSwapChainDescriptorPool() {}

	VkDescriptorPool getDescriptorPool(const GraphicsContext& context) const { return contextHandler.getDisplayState(context)->descriptorPool; }

protected:
	void startRender(const GraphicsContext& context, VulkanDeviceState& state) {
		if (state.getRenderMode().get() == VULKAN_RENDER_UPDATE_DISPLAY) {
			DescriptorPoolState* descriptorPoolState = contextHandler.getDisplayState(context);
			std::vector<VulkanDescriptor*> descriptors = getEntity().getComponents<VulkanDescriptor>();
			VulkanSwapChainState& swapChainState = VulkanSwapChainState::get(context);
			int numImages = swapChainState.getNumImages();
			if (numImages == 0) { numImages = 1; }

			std::vector<VkDescriptorPoolSize> poolSizes;
			for (int f = 0; f < descriptors.size(); f++) {
				VkDescriptorPoolSize poolSize = {};
				descriptors[f]->setPoolSize(poolSize);
				poolSize.descriptorCount = static_cast<uint32_t>(numImages);
				poolSizes.push_back(poolSize);
			}

			VkDescriptorPoolCreateInfo poolInfo = {};
	        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	        poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	        poolInfo.pPoolSizes = poolSizes.data();
	        poolInfo.maxSets = static_cast<uint32_t>(numImages);

			if (vkCreateDescriptorPool(state.getDevice()->getDevice(), &poolInfo, nullptr, &contextHandler.getDisplayState(context)->descriptorPool) != VK_SUCCESS) {
            	throw std::runtime_error("failed to create descriptor pool!");
        	}

        	std::cout << "created descriptor pool" << std::endl;
		}
	}

	void finishRender(const GraphicsContext& context, VulkanDeviceState& state) {
		if (state.getRenderMode().get() == VULKAN_RENDER_CLEANUP_DISPLAY) {
			std::cout << "destroy descriptor pool" << std::endl;
			vkDestroyDescriptorPool(state.getDevice()->getDevice(), getDescriptorPool(context), nullptr);
		}
	}

private:
	struct DescriptorPoolState : public ContextState {
		VkDescriptorPool descriptorPool;
	};

	DisplayContextHandler<ContextState,DescriptorPoolState,ContextState> contextHandler;
};


class VulkanDescriptorSet : public VulkanRenderObject {
public:
	VulkanDescriptorSet() { addType<VulkanDescriptorSet>(); }
	virtual ~VulkanDescriptorSet() {}

	VkDescriptorSet getDescriptorSet(const GraphicsContext& context) const { return contextHandler.getState(context)->descriptorSet; }

protected:
	void startRender(const GraphicsContext& context, VulkanDeviceState& state) {
		if (state.getRenderMode().get() == VULKAN_RENDER_UPDATE) {
			DescriptorSetState* descriptorSetState = contextHandler.getState(context);
			std::vector<VulkanDescriptor*> descriptors = getEntity().getComponents<VulkanDescriptor>();

			VkDescriptorSetLayout layout = getEntity().getComponent<VulkanDescriptorSetLayout>()->getDescriptorSetLayout(context);
	        VkDescriptorSetAllocateInfo allocInfo = {};
	        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	        allocInfo.descriptorPool = getEntity().getComponent<VulkanDescriptorPool>()->getDescriptorPool(context);
	        allocInfo.descriptorSetCount = 1;
	        allocInfo.pSetLayouts = &layout;

	        std::cout << "Layout " <<   layout << std::endl;

	        if (vkAllocateDescriptorSets(state.getDevice()->getDevice(), &allocInfo, &descriptorSetState->descriptorSet) != VK_SUCCESS) {
	            throw std::runtime_error("failed to allocate descriptor sets!");
	        }

	        std::cout << "Created desc set: " << descriptorSetState->descriptorSet << std::endl;

	        std::vector<VkWriteDescriptorSet> descriptorWrites;
	        std::vector<VulkanShaderObject::DescriptorObject*> descriptorObjects;

			for (int f = 0; f < descriptors.size(); f++) {
				VkWriteDescriptorSet descriptorWrite = {};
				descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	            descriptorWrite.dstSet = descriptorSetState->descriptorSet;
	            descriptorWrite.dstBinding = f;
	            descriptors[f]->writeDescriptor(context, descriptorWrite, descriptorObjects);
	            descriptorWrites.push_back(descriptorWrite);
			}

			vkUpdateDescriptorSets(state.getDevice()->getDevice(), static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);

			for (int f = 0; f < descriptorObjects.size(); f++) {
				delete descriptorObjects[f];
			}
		}
	}

	void finishRender(const GraphicsContext& context, VulkanDeviceState& state) {
		/*if (state.getRenderMode().get() == VULKAN_RENDER_CLEANUP_SHARED) {
			DescriptorSetState* descriptorSetState = contextHandler.getState(context);
			//vkDestroyDescriptorPool(state.getDevice()->getDevice(), descriptorSetState->descriptorSet, nullptr);
			// Doesn't appear necessary to destroy?
		}*/
	}


private:
	struct DescriptorSetState : public ContextState {
		VkDescriptorSet descriptorSet;
	};

	GraphicsContextHandler<ContextState,DescriptorSetState> contextHandler;
};

class VulkanGraphicsPipeline : public VulkanRenderObject {
public:
	VulkanGraphicsPipeline(const std::vector<VulkanDescriptorSetLayout*>& descriptorSetLayouts) : descriptorSetLayouts(descriptorSetLayouts) { addType<VulkanGraphicsPipeline>(); }
	virtual ~VulkanGraphicsPipeline() {}

	VkPipeline getGraphicsPipeline(const GraphicsContext& context) const { return contextHandler.getDisplayState(context)->graphicsPipeline; }
	VkPipelineLayout getPipelineLayout(const GraphicsContext& context) const { return contextHandler.getDisplayState(context)->pipelineLayout; }

protected:
	void startRender(const GraphicsContext& context, VulkanDeviceState& state) {
		if (state.getRenderMode().get() == VULKAN_RENDER_UPDATE_DISPLAY) {
			GraphicsPipelineState* pipelineState = contextHandler.getDisplayState(context);
			VkDevice device = state.getDevice()->getDevice();

			std::vector<VkPipelineShaderStageCreateInfo> shaderStages;

	        std::vector<VulkanShaderModule*> shaderModules = getEntity().getComponents<VulkanShaderModule>();
	        for (int f = 0; f < shaderModules.size(); f++) {
	            VkPipelineShaderStageCreateInfo shaderStageInfo = {};
	            shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	            shaderStageInfo.stage = shaderModules[f]->getShaderStage();
	            shaderStageInfo.module = shaderModules[f]->getShaderModule(context);
	            shaderStageInfo.pName = "main";
	            shaderStages.push_back(shaderStageInfo);
	        }

	        VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
	        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

	        //auto bindingDescription = Vertex::getBindingDescription();
	        //auto attributeDescriptions = Vertex::getAttributeDescriptions();

	        std::vector<VulkanVertexInput*> vertexInputList = getEntity().getComponents<VulkanVertexInput>();
	        std::vector<VkVertexInputBindingDescription> bindingDescriptions;
	        std::vector<VkVertexInputAttributeDescription> attributeDescriptions;
	        for (int f = 0; f < vertexInputList.size(); f++) {
	        	bindingDescriptions.push_back(vertexInputList[f]->getBindingDescription(f));
	        	std::vector<VkVertexInputAttributeDescription> bindingAttributeDescriptions = vertexInputList[f]->getAttributeDescriptions(f);
	        	attributeDescriptions.insert(attributeDescriptions.end(), bindingAttributeDescriptions.begin(), bindingAttributeDescriptions.end() );
	        }				

	        vertexInputInfo.vertexBindingDescriptionCount = bindingDescriptions.size();
	        vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
	        vertexInputInfo.pVertexBindingDescriptions = bindingDescriptions.data();
	        vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

	        VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
	        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	        inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	        inputAssembly.primitiveRestartEnable = VK_FALSE;

	        VkViewport viewport = {};
	        viewport.x = 0.0f;
	        viewport.y = 0.0f;
	        viewport.width = (float) state.getExtent().width;
	        viewport.height = (float) state.getExtent().height;
	        viewport.minDepth = 0.0f;
	        viewport.maxDepth = 1.0f;

	        VkRect2D scissor = {};
	        scissor.offset = {0, 0};
	        scissor.extent = state.getExtent();

	        VkPipelineViewportStateCreateInfo viewportState = {};
	        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	        viewportState.viewportCount = 1;
	        viewportState.pViewports = &viewport;
	        viewportState.scissorCount = 1;
	        viewportState.pScissors = &scissor;

	        VkPipelineRasterizationStateCreateInfo rasterizer = {};
	        rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	        rasterizer.depthClampEnable = VK_FALSE;
	        rasterizer.rasterizerDiscardEnable = VK_FALSE;
	        rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	        rasterizer.lineWidth = 1.0f;
	        rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
	        rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	        rasterizer.depthBiasEnable = VK_FALSE;

	        VkPipelineMultisampleStateCreateInfo multisampling = {};
	        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	        multisampling.sampleShadingEnable = VK_FALSE;
	        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

	        VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
	        colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	        colorBlendAttachment.blendEnable = VK_FALSE;

	        VkPipelineColorBlendStateCreateInfo colorBlending = {};
	        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	        colorBlending.logicOpEnable = VK_FALSE;
	        colorBlending.logicOp = VK_LOGIC_OP_COPY;
	        colorBlending.attachmentCount = 1;
	        colorBlending.pAttachments = &colorBlendAttachment;
	        colorBlending.blendConstants[0] = 0.0f;
	        colorBlending.blendConstants[1] = 0.0f;
	        colorBlending.blendConstants[2] = 0.0f;
	        colorBlending.blendConstants[3] = 0.0f;

	        VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
	        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	        pipelineLayoutInfo.setLayoutCount = descriptorSetLayouts.size();
	        std::vector<VkDescriptorSetLayout> descLayouts;
	        for (int f = 0; f < descriptorSetLayouts.size(); f++) {
	        	descLayouts.push_back(descriptorSetLayouts[f]->getDescriptorSetLayout(context));
	        }
	        pipelineLayoutInfo.pSetLayouts = &descLayouts[0];

	        if (vkCreatePipelineLayout(state.getDevice()->getDevice(), &pipelineLayoutInfo, nullptr, &pipelineState->pipelineLayout) != VK_SUCCESS) {
	            throw std::runtime_error("failed to create pipeline layout!");
	        }

	        VkGraphicsPipelineCreateInfo pipelineInfo = {};
	        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	        pipelineInfo.stageCount = shaderStages.size();
	        pipelineInfo.pStages = shaderStages.data();
	        pipelineInfo.pVertexInputState = &vertexInputInfo;
	        pipelineInfo.pInputAssemblyState = &inputAssembly;
	        pipelineInfo.pViewportState = &viewportState;
	        pipelineInfo.pRasterizationState = &rasterizer;
	        pipelineInfo.pMultisampleState = &multisampling;
	        pipelineInfo.pColorBlendState = &colorBlending;
	        pipelineInfo.layout = pipelineState->pipelineLayout;
	        pipelineInfo.renderPass = state.getRenderPass().get()->getRenderPass(context);
	        pipelineInfo.subpass = 0;
	        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

	        if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipelineState->graphicsPipeline) != VK_SUCCESS) {
	            throw std::runtime_error("failed to create graphics pipeline!");
	        }			
		}

		if ((state.getRenderMode().get() & VULKAN_RENDER_COMMAND) == VULKAN_RENDER_COMMAND) {
			GraphicsPipelineState* pipelineState = contextHandler.getDisplayState(context);
			vkCmdBindPipeline(state.getCommandBuffer().get()->getCommandBuffer(context), VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineState->graphicsPipeline);
			std::cout << "bind pipeline" << std::endl;
		}
	}
	void finishRender(const GraphicsContext& context, VulkanDeviceState& state) {
		if (state.getRenderMode().get() == VULKAN_RENDER_CLEANUP_DISPLAY) {
			GraphicsPipelineState* pipelineState = contextHandler.getDisplayState(context);
			vkDestroyPipeline(state.getDevice()->getDevice(), pipelineState->graphicsPipeline, nullptr);
	        vkDestroyPipelineLayout(state.getDevice()->getDevice(), pipelineState->pipelineLayout, nullptr);
			std::cout << "Destroy pipeline." << std::endl;
		}
	}

private:
	struct GraphicsPipelineState : public ContextState {
		VkPipeline graphicsPipeline;
		VkPipelineLayout pipelineLayout;
	};

	DisplayContextHandler<ContextState,GraphicsPipelineState,ContextState> contextHandler;
	std::vector<VulkanDescriptorSetLayout*> descriptorSetLayouts;
};


class VulkanCmdBindDescriptorSet : public VulkanRenderObject {
public:
	VulkanCmdBindDescriptorSet(VulkanDescriptorSet* descriptorSet, VulkanGraphicsPipeline* graphicsPipeline, int setBinding) : descriptorSet(descriptorSet), graphicsPipeline(graphicsPipeline), setBinding(setBinding) { addType<VulkanCmdBindDescriptorSet>(); }
	virtual ~VulkanCmdBindDescriptorSet() {}

protected:
	void startRender(const GraphicsContext& context, VulkanDeviceState& state) {
		if ((state.getRenderMode().get() & VULKAN_RENDER_COMMAND) == VULKAN_RENDER_COMMAND) {
			VkDescriptorSet descSet = descriptorSet->getDescriptorSet(context);
            std::cout << VulkanSwapChainState::get(context).getSwapChain()->getName() << " " << descSet << std::endl;
            vkCmdBindDescriptorSets(state.getCommandBuffer().get()->getCommandBuffer(context), VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline->getPipelineLayout(context), setBinding, 1, &descSet, 0, nullptr);
		}
	}

private:
	VulkanDescriptorSet* descriptorSet;
	VulkanGraphicsPipeline* graphicsPipeline;
	int setBinding;
};





}

#endif
