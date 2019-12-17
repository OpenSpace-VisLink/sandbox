#ifndef SANDBOX_GRAPHICS_VULKAN_IMPL_VULKAN_APP_BASE_H_
#define SANDBOX_GRAPHICS_VULKAN_IMPL_VULKAN_APP_BASE_H_

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "vulkan.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <algorithm>
#include <chrono>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <cstdint>
#include <array>
#include <optional>
#include <set>

#include <sandbox/image/Image.h>
#include <sandbox/geometry/Mesh.h>
#include <sandbox/graphics/vulkan/mesh/VulkanMeshRenderer.h>
#include <sandbox/geometry/loaders/ShapeLoader.h>
#include <sandbox/base/Transform.h>
#include <sandbox/input/interaction/MouseInteraction.h>
#include <sandbox/input/glfw/GLFWInput.h>
#include <sandbox/input/touch/MouseTouchEmulator.h>
#include <sandbox/input/touch/TUIOTouchInput.h>
#include <sandbox/input/interaction/TouchTranslate.h>
#include <sandbox/input/interaction/TouchScale.h>

using namespace sandbox;

struct ViewBufferObject {
    alignas(16) glm::mat4 model;
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 proj;
};

struct TransformUniformBufferObject {
    alignas(16) glm::mat4 transform;
};


class MainUniformBuffer : public VulkanUniformBufferValue<ViewBufferObject> {
protected:
    void updateBuffer(const GraphicsContext& context, VulkanDeviceState& state, VulkanBuffer* buffer) {
        ViewBufferObject ubo = {};
        ubo.view = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        ubo.proj = glm::perspective(glm::radians(45.0f), (float) state.getExtent().width / (float) state.getExtent().height, 0.01f, 100.0f);
        ubo.model = glm::mat4(1.0f);
        if (VulkanSwapChainState::get(context).getSwapChain()->getName() == "window 2") {
            ubo.model = glm::translate(ubo.model, glm::vec3(0.25,0,0.0));
        }
        ubo.proj[1][1] *= -1;

        buffer->update(&ubo, sizeof(ubo));
    }
};

class ObjectState : public StateContainerItem {
public:
    ObjectState() { objectIndex = 0; }
    virtual ~ObjectState() {}
    static ObjectState& get(const GraphicsContext& context) { return context.getRenderState()->getItem<ObjectState>(); }
    int objectIndex;

};

class TransformUniformBuffer : public VulkanAlignedUniformArrayBuffer< TransformUniformBufferObject > {
public:
    TransformUniformBuffer() : VulkanAlignedUniformArrayBuffer< TransformUniformBufferObject >(256,true) { addType<TransformUniformBuffer>(); }
};

class UpdateTransform : public RenderObject {
public:
    UpdateTransform(TransformUniformBuffer* transformBuffer) : transformBuffer(transformBuffer) {}

    virtual void startRender(const GraphicsContext& context) {
        TransformState& state = TransformState::get(context);
        if (state.calculateTransform) {
            transformBuffer->getItem(ObjectState::get(context).objectIndex)->transform = state.getTransform().get();
        }

        ObjectState::get(context).objectIndex++;
    }

    virtual void finishRender(const GraphicsContext& context) {
        //ObjectState::get(context).objectIndex--;
    }

private:
    TransformUniformBuffer* transformBuffer;
};

class TransformRoot : public RenderObject {
public:
    TransformRoot() {}
    virtual void startRender(const GraphicsContext& context) {
        ObjectState::get(context).objectIndex = 0;
    }

private:
    TransformUniformBuffer* transformBuffer;
};

class VulkanCmdBindDynamicDescriptorSet2 : public VulkanRenderObject {
public:
    VulkanCmdBindDynamicDescriptorSet2(VulkanDescriptorSet* descriptorSet, VulkanUniformBuffer* uniformBuffer, int setBinding) : descriptorSet(descriptorSet), uniformBuffer(uniformBuffer), setBinding(setBinding) { addType<VulkanCmdBindDynamicDescriptorSet>(); }
    virtual ~VulkanCmdBindDynamicDescriptorSet2() {}

protected:
    void startRender(const GraphicsContext& context, VulkanDeviceState& state) {
        if ((state.getRenderMode().get() & VULKAN_RENDER_COMMAND) == VULKAN_RENDER_COMMAND) {
            uint32_t offset = ObjectState::get(context).objectIndex * uniformBuffer->getRange();
            VkDescriptorSet descSet = descriptorSet->getDescriptorSet(context);
            std::cout << VulkanSwapChainState::get(context).getSwapChain()->getName() << " " << descSet << std::endl;
            vkCmdBindDescriptorSets(state.getCommandBuffer().get()->getCommandBuffer(context), VK_PIPELINE_BIND_POINT_GRAPHICS, state.getGraphicsPipeline().get()->getPipelineLayout(context), setBinding, 1, &descSet, 1, &offset);
        }
    }

private:
    VulkanDescriptorSet* descriptorSet;
    VulkanUniformBuffer* uniformBuffer;
    int setBinding;
};

class UniformBufferIterator {
public:
    UniformBufferIterator(VulkanUniformBuffer* uniformBuffer, VulkanDescriptorSet* descriptorSet, int setBinding) : currentIndex(0), uniformBuffer(uniformBuffer), descriptorSet(descriptorSet), setBinding(setBinding) {}

    void apply(EntityNode* entity) {
        std::cout << "index" << currentIndex << std::endl;
        entity->addComponent(new VulkanCmdBindDynamicDescriptorSet2(descriptorSet, uniformBuffer, setBinding)); 
        entity->addComponent(new UpdateTransform(static_cast<TransformUniformBuffer*>(uniformBuffer))); 
        //entity->addComponent(new VulkanCmdBindDynamicDescriptorSet(descriptorSet, uniformBuffer, setBinding, currentIndex)); 
        currentIndex++;
        //return component;
    }

private:
    VulkanUniformBuffer* uniformBuffer;
    VulkanDescriptorSet* descriptorSet;
    int setBinding;
    int currentIndex;
};


const int WIDTH = 512;
const int HEIGHT = 512;


class VulkanAppBase {
public:
    void run() {
        initWindow();
        initVulkan();
        mainLoop();
        cleanup();
    }

protected:
    struct PlanetAppInfo {
        EntityNode images;
        EntityNode descriptorSetGroup;
        EntityNode shaderObjects;
        EntityNode* sphere;
        EntityNode* viewDescriptorSet;
        EntityNode* transformDescriptorSet;
    } appInfo;

    GLFWwindow* window;
    EntityNode vulkanNode;
    EntityNode renderPassNode;
    EntityNode pipelineNode;
    EntityNode graphicsObjects;
    EntityNode scene;
    EntityNode input;
    //EntityNode images;
    EntityNode* updateSharedNode;
    EntityNode* sceneGraph;
    Entity* renderNode;

    void initWindow() { 
        glfwInit();

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
        glfwSetWindowUserPointer(window, this);
        glfwSetWindowPos (window, 0, 0);
    }


    virtual void initScene() {}

    void initVulkan() {

        EntityNode* viewUniformBuffer = new EntityNode(&appInfo.shaderObjects);
            viewUniformBuffer->addComponent(new MainUniformBuffer());
        EntityNode* samplerNode = new EntityNode(&appInfo.shaderObjects);
            samplerNode->addComponent(new VulkanSampler());
        EntityNode* transformUniformBuffer = new EntityNode(&appInfo.shaderObjects);
            transformUniformBuffer->addComponent(new TransformUniformBuffer());

        appInfo.viewDescriptorSet = new EntityNode(&appInfo.descriptorSetGroup);
            appInfo.viewDescriptorSet->addComponent(new VulkanDescriptorSetLayout()); 
            appInfo.viewDescriptorSet->addComponent(new VulkanSwapChainDescriptorPool());
            appInfo.viewDescriptorSet->addComponent(new VulkanDescriptorSet());
            appInfo.viewDescriptorSet->addComponent(new VulkanDescriptor(viewUniformBuffer->getComponent<VulkanUniformBuffer>(), VK_SHADER_STAGE_VERTEX_BIT));

        appInfo.transformDescriptorSet = new EntityNode(&appInfo.descriptorSetGroup);
            appInfo.transformDescriptorSet->addComponent(new VulkanDescriptorSetLayout());
            appInfo.transformDescriptorSet->addComponent(new VulkanSwapChainDescriptorPool());
            appInfo.transformDescriptorSet->addComponent(new VulkanDescriptorSet());
            appInfo.transformDescriptorSet->addComponent(new VulkanDescriptor(transformUniformBuffer->getComponent<VulkanUniformBuffer>(), VK_SHADER_STAGE_VERTEX_BIT));

        initScene();

        createVulkanNode();

        input.addComponent(new GLFWInput(window));
        input.addComponent(new TUIOTouchInput());
        input.update();
    }

    void createVulkanNode() {
        vulkanNode.addComponent(new VulkanInstance());
        EntityNode* surfaceNode = new EntityNode(&vulkanNode);
            surfaceNode->addComponent(new GlfwSurface(window, &vulkanNode));
        Entity* deviceNode = new EntityNode(&vulkanNode);
            deviceNode->addComponent(new SharedContext());
            deviceNode->addComponent(new VulkanDevice(&vulkanNode));
            EntityNode* queues = new EntityNode(deviceNode);
                queues->addComponent(new VulkanGraphicsQueue());
                queues->addComponent(new VulkanPresentQueue(surfaceNode));
                VulkanGraphicsQueue* graphicsQueue = queues->getComponent<VulkanGraphicsQueue>();
                VulkanPresentQueue* presentQueue = queues->getComponent<VulkanPresentQueue>();
            updateSharedNode = new EntityNode(deviceNode);
                updateSharedNode->addComponent(new AllowRenderModes(VULKAN_RENDER_UPDATE_SHARED | VULKAN_RENDER_OBJECT | VULKAN_RENDER_CLEANUP_SHARED));
                updateSharedNode->addComponent(new VulkanDeviceRenderer());
                updateSharedNode->addComponent(new VulkanCommandPool(graphicsQueue));
                updateSharedNode->addComponent(new RenderNode(&graphicsObjects));
                updateSharedNode->addComponent(new RenderNode(&appInfo.images));
                EntityNode* renderSpecific = new EntityNode(updateSharedNode);
                    renderSpecific->addComponent(new AllowRenderModes(VULKAN_RENDER_SHARED_ONLY));
                    renderSpecific->addComponent(new RenderNode(&appInfo.shaderObjects));
                    renderSpecific->addComponent(new RenderNode(&appInfo.descriptorSetGroup));
                    renderSpecific->addComponent(new RenderNode(&renderPassNode, RENDER_ACTION_START));
                    renderSpecific->addComponent(new RenderNode(&pipelineNode));
                    renderSpecific->addComponent(new RenderNode(&renderPassNode, RENDER_ACTION_END));
            EntityNode* windowNodes = new EntityNode(deviceNode);
                renderNode = createSwapChain(windowNodes, surfaceNode, queues, "Window");

        vulkanNode.update();

        VulkanDeviceRenderer* sharedRenderer = updateSharedNode->getComponentRecursive<VulkanDeviceRenderer>();
        sharedRenderer->render(VULKAN_RENDER_UPDATE_SHARED);
        sharedRenderer->render(VULKAN_RENDER_OBJECT);

        VulkanDeviceRenderer* renderer = renderNode->getComponentRecursive<VulkanDeviceRenderer>();
        renderer->render(VULKAN_RENDER_UPDATE_SHARED);
        renderer->render(VULKAN_RENDER_UPDATE_DISPLAY);

        std::vector<VulkanDeviceRenderer*> renderers = windowNodes->getComponentsRecursive<VulkanDeviceRenderer>();
        for (int f = 0; f < renderers.size(); f++) {  
            renderers[f]->render(VULKAN_RENDER_UPDATE);
            renderers[f]->render(VULKAN_RENDER_OBJECT);
            renderers[f]->render(VULKAN_RENDER_COMMAND); 
        }
    }


    EntityNode* createSwapChain(Entity* parentNode, Entity* surfaceNode, EntityNode* queues, std::string name) {
        VulkanGraphicsQueue* graphicsQueue = queues->getComponent<VulkanGraphicsQueue>();
        VulkanPresentQueue* presentQueue = queues->getComponent<VulkanPresentQueue>();
        EntityNode* renderNode = new EntityNode(parentNode);
            renderNode->addComponent(new VulkanBasicSwapChain(name, surfaceNode));
            renderNode->addComponent(new VulkanFrameRenderer(graphicsQueue, presentQueue));
            EntityNode* updateNode = new EntityNode(renderNode);
                updateNode->addComponent(new AllowRenderModes(VULKAN_RENDER_UPDATE_DISPLAY | VULKAN_RENDER_UPDATE | VULKAN_RENDER_OBJECT | VULKAN_RENDER_CLEANUP_DISPLAY | VULKAN_RENDER_CLEANUP));
                updateNode->addComponent(new RenderNode(&appInfo.shaderObjects));
                updateNode->addComponent(new RenderNode(&appInfo.descriptorSetGroup));
                updateNode->addComponent(new RenderNode(&renderPassNode, RENDER_ACTION_START));
                updateNode->addComponent(new RenderNode(&pipelineNode));
                updateNode->addComponent(new RenderNode(&renderPassNode, RENDER_ACTION_END));
            EntityNode* commandNode = new EntityNode(renderNode);
                commandNode->addComponent(new VulkanCommandPool(graphicsQueue));
                commandNode->addComponent(new VulkanCommandBuffer());
                EntityNode* commands = new EntityNode(commandNode);
                    commands->addComponent(new AllowRenderModes(VULKAN_RENDER_COMMAND));
                    commands->addComponent(new RenderNode(&scene));
        return renderNode;
    }

    virtual void update() {}

    void mainLoop() {
        while (!glfwWindowShouldClose(window)) {
            static int frame = 0;
            glfwPollEvents();
            input.update();
            scene.update();

            update();

            // independent windows
            renderNode->getComponent<VulkanFrameRenderer>()->drawFrame();

            if (frame>=0) {   
                static GraphicsContext updateContext;
                TransformState::get(updateContext).calculateTransform = true;
                static RenderNode sceneUpdate(&scene);
                sceneUpdate.render(updateContext);
            }

            frame++;
        }

        vkDeviceWaitIdle(vulkanNode.getComponentRecursive<VulkanDevice>()->getDevice());
    }


    void cleanup() {
        glfwDestroyWindow(window);
        glfwTerminate();
    }
};


#endif