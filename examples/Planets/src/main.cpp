

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
#include <sandbox/geometry/loaders/ShapeLoader.h>
#include <sandbox/base/Transform.h>
#include <sandbox/input/interaction/MouseInteraction.h>
#include <sandbox/input/glfw/GLFWInput.h>
#include <sandbox/input/touch/MouseTouchEmulator.h>
#include <sandbox/input/touch/TUIOTouchInput.h>
#include <sandbox/input/interaction/TouchTranslate.h>
#include <sandbox/input/interaction/TouchScale.h>

using namespace sandbox;

struct UniformBufferObject {
    alignas(16) glm::mat4 model;
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 proj;
};

struct TransformUniformBufferObject {
    alignas(16) glm::mat4 transform;
};


class MainUniformBuffer : public VulkanUniformBufferValue<UniformBufferObject> {
protected:
    void updateBuffer(const GraphicsContext& context, VulkanDeviceState& state, VulkanBuffer* buffer) {
        static auto startTime = std::chrono::high_resolution_clock::now();

        auto currentTime = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
        //UniformBufferObject ubo = {};
        UniformBufferObject ubo = {};
        ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        if (VulkanSwapChainState::get(context).getSwapChain()->getName() == "window 2") {
            ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        }
        if (VulkanSwapChainState::get(context).getSwapChain()->getName() == "window 3") {
            ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        }
        //ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        ubo.view = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        ubo.proj = glm::perspective(glm::radians(45.0f), (float) state.getExtent().width / (float) state.getExtent().height, 0.1f, 100.0f);
        ubo.model = glm::mat4(1.0f);
        if (VulkanSwapChainState::get(context).getSwapChain()->getName() == "window 2") {
            ubo.model = glm::translate(ubo.model, glm::vec3(0.25,0,0.0));
        }
        //ubo.model = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        //std::cout << VulkanSwapChainState::get(context).getSwapChain()->getName() << " " << state.getExtent().width << " " << (float) state.getExtent().height << std::endl;
        ubo.proj[1][1] *= -1;
        //VulkanUniformBufferValue<UniformBufferObject>::updateBuffer(context, state, buffer);

        buffer->update(&ubo, sizeof(ubo));
    }
};

class MainUniformBuffer2 : public VulkanUniformBufferValue<UniformBufferObject> {
protected:
    void updateBuffer(const GraphicsContext& context, VulkanDeviceState& state, VulkanBuffer* buffer) {
        static auto startTime = std::chrono::high_resolution_clock::now();

        auto currentTime = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
        //UniformBufferObject ubo = {};
        UniformBufferObject ubo = {};
        ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        if (VulkanSwapChainState::get(context).getSwapChain()->getName() == "window 2") {
            ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        }
        if (VulkanSwapChainState::get(context).getSwapChain()->getName() == "window 3") {
            ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        }
        //ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        ubo.view = glm::lookAt(glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        ubo.proj = glm::perspective(glm::radians(45.0f), (float) state.getExtent().width / (float) state.getExtent().height, 0.1f, 100.0f);
        ubo.model = glm::translate(glm::mat4(1.0f), glm::vec3(0.25,0,0.0));
        if (VulkanSwapChainState::get(context).getSwapChain()->getName() == "window 2") {
            ubo.model = glm::translate(ubo.model, glm::vec3(0.25,0,0.0));
        }
        //ubo.model = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        //std::cout << VulkanSwapChainState::get(context).getSwapChain()->getName() << " " << state.getExtent().width << " " << (float) state.getExtent().height << std::endl;
        ubo.proj[1][1] *= -1;
        //VulkanUniformBufferValue<UniformBufferObject>::updateBuffer(context, state, buffer);

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
    TransformUniformBuffer() : VulkanAlignedUniformArrayBuffer< TransformUniformBufferObject >(256,true) {}
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

struct Vertex {
    glm::vec2 pos;
    glm::vec3 color;
    glm::vec2 texCoord;
};


const std::vector<Vertex> vertices = {
    {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
    {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
    {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
    {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}
};

const std::vector<uint16_t> indices = {
    0, 1, 2, 2, 3, 0
};

const std::vector<Vertex> tri_vertices = {
    {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
    {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
    {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
    {{-1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}
};
const std::vector<uint16_t> tri_indices = {
    0, 1, 2
};


class HelloTriangleApplication {
public:
    void run() {
        initWindow();
        initVulkan();
        mainLoop();
        cleanup();
    }

private:
    GLFWwindow* window;
    EntityNode vulkanNode;
    EntityNode renderPassNode;
    EntityNode pipelineNode;
    EntityNode shaderObjects;
    EntityNode graphicsObjects;
    EntityNode descriptorSetGroup;
    EntityNode scene;
    EntityNode input;
    EntityNode images;
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


    void initVulkan() {

        Entity* mainImage = new EntityNode(&images);
            mainImage->addComponent(new Image("examples/VulkanSandbox/textures/texture.jpg"));
            mainImage->addComponent(new VulkanImage());
            mainImage->addComponent(new VulkanImageView());
        images.update();

        EntityNode* mainUniformBuffer = new EntityNode(&shaderObjects);
            mainUniformBuffer->addComponent(new MainUniformBuffer());
        EntityNode* samplerNode = new EntityNode(&shaderObjects);
            samplerNode->addComponent(new VulkanSampler());
        EntityNode* transformUniformBuffer = new EntityNode(&shaderObjects);
            transformUniformBuffer->addComponent(new TransformUniformBuffer());

        EntityNode* mainDescriptorSet = new EntityNode(&descriptorSetGroup);
            mainDescriptorSet->addComponent(new VulkanDescriptorSetLayout()); 
            mainDescriptorSet->addComponent(new VulkanSwapChainDescriptorPool());
            mainDescriptorSet->addComponent(new VulkanDescriptorSet());
            mainDescriptorSet->addComponent(new VulkanDescriptor(mainUniformBuffer->getComponent<VulkanUniformBuffer>(), VK_SHADER_STAGE_VERTEX_BIT));
            mainDescriptorSet->addComponent(new VulkanDescriptor(new VulkanImageViewDecorator(samplerNode->getComponent<VulkanSampler>(), mainImage->getComponent<VulkanImageView>()), VK_SHADER_STAGE_FRAGMENT_BIT));

        EntityNode* transformDescriptorSet = new EntityNode(&descriptorSetGroup);
            transformDescriptorSet->addComponent(new VulkanDescriptorSetLayout());
            transformDescriptorSet->addComponent(new VulkanSwapChainDescriptorPool());
            transformDescriptorSet->addComponent(new VulkanDescriptorSet());
            transformDescriptorSet->addComponent(new VulkanDescriptor(transformUniformBuffer->getComponent<VulkanUniformBuffer>(), VK_SHADER_STAGE_VERTEX_BIT));

        EntityNode* quad = new EntityNode(&graphicsObjects);
            Mesh* mesh = new Mesh();
            quad->addComponent(mesh);
            quad->addComponent(new ShapeLoader(SHAPE_SPHERE, 50));
            //quad->addComponent(new ShapeLoader(SHAPE_CIRCLE, 20));
            //quad->addComponent(new ShapeLoader(SHAPE_QUAD));
            quad->update();
            VertexArray<glm::vec3>* quad_vertexArray = new VertexArray<glm::vec3>(0);
            quad_vertexArray->value = mesh->getNodes();
            quad->addComponent(quad_vertexArray);
            quad_vertexArray = new VertexArray<glm::vec3>(1);
            quad_vertexArray->value = mesh->getNormals();
            quad->addComponent(quad_vertexArray);
            VertexArray<glm::vec2>* quad_coords = new VertexArray<glm::vec2>(2);
            quad_coords->value = mesh->getCoords();
            quad->addComponent(quad_coords);
            IndexArray* quad_indexArray = new IndexArray();
            quad_indexArray->value = mesh->getIndices();
            quad->addComponent(quad_indexArray);


        pipelineNode.addComponent(new VulkanShaderModule("examples/VulkanSandbox/src/shaders/vert2.spv", VK_SHADER_STAGE_VERTEX_BIT));
        pipelineNode.addComponent(new VulkanShaderModule("examples/VulkanSandbox/src/shaders/frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT));
        std::vector<VulkanDescriptorSetLayout*> layouts;
        layouts.push_back(mainDescriptorSet->getComponent<VulkanDescriptorSetLayout>());
        layouts.push_back(transformDescriptorSet->getComponent<VulkanDescriptorSetLayout>());
        pipelineNode.addComponent(new VulkanGraphicsPipeline(layouts));
        VulkanVertexInput* vertexInput = new VulkanVertexInput(sizeof(glm::vec3), 0);
        vertexInput->addAttribute(VK_FORMAT_R32G32B32_SFLOAT, 0);
        pipelineNode.addComponent(vertexInput);
        vertexInput = new VulkanVertexInput(sizeof(glm::vec3), 1);
        vertexInput->addAttribute(VK_FORMAT_R32G32B32_SFLOAT, 0);
        pipelineNode.addComponent(vertexInput);
        vertexInput = new VulkanVertexInput(sizeof(glm::vec2), 2);
        vertexInput->addAttribute(VK_FORMAT_R32G32_SFLOAT, 0);
        pipelineNode.addComponent(vertexInput);

        renderPassNode.addComponent(new VulkanBasicRenderPass());
        renderPassNode.addComponent(new VulkanSwapChainFramebuffer());

        UniformBufferIterator bufferTransform(transformUniformBuffer->getComponent<VulkanUniformBuffer>(), transformDescriptorSet->getComponent<VulkanDescriptorSet>(), 1);

        sceneGraph = new EntityNode(&graphicsObjects, "sceneGraph"); 
            sceneGraph->addComponent(new TransformRoot());
            //sceneGraph->addComponent(new Transform(glm::rotate(glm::scale(glm::mat4(1.0f), glm::vec3(0.25f)), glm::radians(-45.0f), glm::vec3(1.0f, 0.0f, 0.0f))));
            sceneGraph->addComponent(new Transform());
            sceneGraph->addComponent(new Transform());
            bufferTransform.apply(sceneGraph);
            sceneGraph->addComponent(new RenderNode(quad));

        scene.addComponent(new MouseInteraction(&input));
        scene.addComponent(new TouchTranslate(&input));
        scene.addComponent(new TouchScale(&input));
        EntityNode* drawObject = new EntityNode(&scene);
            drawObject->addComponent(new RenderNode(&renderPassNode, RENDER_ACTION_START));
            drawObject->addComponent(new RenderNode(&pipelineNode, RENDER_ACTION_START));
            drawObject->addComponent(new VulkanCmdBindDescriptorSet(mainDescriptorSet->getComponent<VulkanDescriptorSet>(), 0));
            drawObject->addComponent(new RenderNode(sceneGraph));
            drawObject->addComponent(new RenderNode(&pipelineNode, RENDER_ACTION_END));
            drawObject->addComponent(new RenderNode(&renderPassNode, RENDER_ACTION_END));

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
                updateSharedNode->addComponent(new RenderNode(&images));
                EntityNode* renderSpecific = new EntityNode(updateSharedNode);
                    renderSpecific->addComponent(new AllowRenderModes(VULKAN_RENDER_SHARED_ONLY));
                    renderSpecific->addComponent(new RenderNode(&shaderObjects));
                    renderSpecific->addComponent(new RenderNode(&descriptorSetGroup));
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

        input.addComponent(new GLFWInput(window));
        input.addComponent(new TUIOTouchInput());
        input.update();
    }


    EntityNode* createSwapChain(Entity* parentNode, Entity* surfaceNode, EntityNode* queues, std::string name) {
        VulkanGraphicsQueue* graphicsQueue = queues->getComponent<VulkanGraphicsQueue>();
        VulkanPresentQueue* presentQueue = queues->getComponent<VulkanPresentQueue>();
        EntityNode* renderNode = new EntityNode(parentNode);
            renderNode->addComponent(new VulkanBasicSwapChain(name, surfaceNode));
            renderNode->addComponent(new VulkanFrameRenderer(graphicsQueue, presentQueue));
            EntityNode* updateNode = new EntityNode(renderNode);
                updateNode->addComponent(new AllowRenderModes(VULKAN_RENDER_UPDATE_DISPLAY | VULKAN_RENDER_UPDATE | VULKAN_RENDER_OBJECT | VULKAN_RENDER_CLEANUP_DISPLAY | VULKAN_RENDER_CLEANUP));
                updateNode->addComponent(new RenderNode(&shaderObjects));
                updateNode->addComponent(new RenderNode(&descriptorSetGroup));
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

    void mainLoop() {
        while (!glfwWindowShouldClose(window)) {
            static int frame = 0;
            glfwPollEvents();
            input.update();
            scene.update();

            // independent windows
            renderNode->getComponent<VulkanFrameRenderer>()->drawFrame();

            if (frame>=0) {   
                static auto startTime = std::chrono::high_resolution_clock::now();
                auto currentTime = std::chrono::high_resolution_clock::now();
                float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

                //sceneGraph->getComponents<Transform>()[1]->setTransform(glm::rotate(glm::mat4(1.0f), 0.0f * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)));

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

int main() {
    HelloTriangleApplication app;

    try {
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

