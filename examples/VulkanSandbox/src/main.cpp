

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "vulkan.h"

#define GLM_FORCE_RADIANS
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

using namespace sandbox;

struct UniformBufferObject {
    alignas(16) glm::mat4 model;
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 proj;
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
        ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        ubo.proj = glm::perspective(glm::radians(45.0f), (float) state.getExtent().width / (float) state.getExtent().height, 0.1f, 10.0f);
        //std::cout << VulkanSwapChainState::get(context).getSwapChain()->getName() << " " << state.getExtent().width << " " << (float) state.getExtent().height << std::endl;
        ubo.proj[1][1] *= -1;
        //VulkanUniformBufferValue<UniformBufferObject>::updateBuffer(context, state, buffer);

        buffer->update(&ubo, sizeof(ubo));
    }
};

class SecondUniformBuffer : public VulkanUniformBufferValue<UniformBufferObject> {
protected:
    void updateBuffer(const GraphicsContext& context, VulkanDeviceState& state, VulkanBuffer* buffer) {
        static auto startTime = std::chrono::high_resolution_clock::now();

        auto currentTime = std::chrono::high_resolution_clock::now();
        float time = 0.0;//std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
        //UniformBufferObject ubo = {};
        UniformBufferObject ubo = {};
        ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        if (VulkanSwapChainState::get(context).getSwapChain()->getName() == "window 2") {
            ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        }
        if (VulkanSwapChainState::get(context).getSwapChain()->getName() == "window 3") {
            ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        }
        ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        ubo.proj = glm::perspective(glm::radians(45.0f), (float) state.getExtent().width / (float) state.getExtent().height, 0.1f, 10.0f);
        //std::cout << VulkanSwapChainState::get(context).getSwapChain()->getName() << " " << state.getExtent().width << " " << (float) state.getExtent().height << std::endl;
        ubo.proj[1][1] *= -1;
        //VulkanUniformBufferValue<UniformBufferObject>::updateBuffer(context, state, buffer);

        buffer->update(&ubo, sizeof(ubo));
    }
};


const int WIDTH = 500;
const int HEIGHT = 400;

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
    GLFWwindow* window2;
    GLFWwindow* window3;

    VkDevice device;
    VkDevice device2;

    VulkanQueue* graphicsQueue;
    VulkanQueue* presentQueue;

    EntityNode vulkanNode;
    EntityNode pipelineNode;
    EntityNode shaderObjects;
    EntityNode graphicsObjects;
    EntityNode descriptorSetGroup;
    EntityNode scene;
    EntityNode images;
    Entity* renderNode0;
    Entity* renderNode2;
    Entity* renderNode3;

    void initWindow() { 
        glfwInit();

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
        glfwSetWindowUserPointer(window, this);
        glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
        glfwSetWindowPos (window, 0, 0);

        window2 = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
        glfwSetWindowUserPointer(window2, this);
        glfwSetFramebufferSizeCallback(window2, framebufferResizeCallback);
        glfwSetWindowPos (window2, WIDTH+5, 0);

        window3 = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
        glfwSetWindowUserPointer(window3, this);
        glfwSetFramebufferSizeCallback(window3, framebufferResizeCallback);
        glfwSetWindowPos (window3, 2*(WIDTH+5), 0);
    }


    static void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
        auto app = reinterpret_cast<HelloTriangleApplication*>(glfwGetWindowUserPointer(window));
    }

    void initVulkan() {

        Entity* mainImage = new EntityNode(&images);
            mainImage->addComponent(new Image("examples/VulkanSandbox/textures/texture.jpg"));
            mainImage->addComponent(new VulkanImage());
            mainImage->addComponent(new VulkanImageView());
        Entity* secondImage = new EntityNode(&images);
            secondImage->addComponent(new Image("examples/VulkanSandbox/textures/test.png"));
            secondImage->addComponent(new VulkanImage());
            secondImage->addComponent(new VulkanImageView());
        images.update();

        EntityNode* mainUniformBuffer = new EntityNode(&shaderObjects);
            mainUniformBuffer->addComponent(new MainUniformBuffer());
        EntityNode* secondUniformBuffer = new EntityNode(&shaderObjects);
            secondUniformBuffer->addComponent(new SecondUniformBuffer());
        EntityNode* samplerNode = new EntityNode(&shaderObjects);
            samplerNode->addComponent(new VulkanSampler());

        EntityNode* mainDescriptorSet = new EntityNode(&descriptorSetGroup);
            mainDescriptorSet->addComponent(new VulkanDescriptorSetLayout());
            mainDescriptorSet->addComponent(new VulkanSwapChainDescriptorPool());
            mainDescriptorSet->addComponent(new VulkanDescriptorSet());
            mainDescriptorSet->addComponent(new VulkanDescriptor(mainUniformBuffer->getComponent<VulkanUniformBuffer>(), VK_SHADER_STAGE_VERTEX_BIT));
            mainDescriptorSet->addComponent(new VulkanDescriptor(new VulkanImageViewDecorator(samplerNode->getComponent<VulkanSampler>(), mainImage->getComponent<VulkanImageView>()), VK_SHADER_STAGE_FRAGMENT_BIT));


        EntityNode* secondDescriptorSet = new EntityNode(&descriptorSetGroup);
            secondDescriptorSet->addComponent(new VulkanDescriptorSetLayout());
            secondDescriptorSet->addComponent(new VulkanSwapChainDescriptorPool());
            secondDescriptorSet->addComponent(new VulkanDescriptorSet());
            secondDescriptorSet->addComponent(new VulkanDescriptor(secondUniformBuffer->getComponent<VulkanUniformBuffer>(), VK_SHADER_STAGE_VERTEX_BIT));
            secondDescriptorSet->addComponent(new VulkanDescriptor(new VulkanImageViewDecorator(samplerNode->getComponent<VulkanSampler>(), secondImage->getComponent<VulkanImageView>()), VK_SHADER_STAGE_FRAGMENT_BIT));


        VertexArray<Vertex>* vertexArray = new VertexArray<Vertex>();
        vertexArray->value = vertices;
        IndexArray* indexArray = new IndexArray();
        indexArray->value = indices;
        graphicsObjects.addComponent(vertexArray);
        graphicsObjects.addComponent(indexArray);

        pipelineNode.addComponent(new VulkanShaderModule("examples/VulkanExample/src/shaders/vert.spv", VK_SHADER_STAGE_VERTEX_BIT));
        pipelineNode.addComponent(new VulkanShaderModule("examples/VulkanExample/src/shaders/frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT));
        VulkanVertexInput* vertexInput = new VulkanVertexInput(sizeof(Vertex));
        vertexInput->addAttribute(VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, pos));
        vertexInput->addAttribute(VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color));
        vertexInput->addAttribute(VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, texCoord));
        pipelineNode.addComponent(new VulkanBasicRenderPass());
        pipelineNode.addComponent(new VulkanSwapChainFramebuffer());
        pipelineNode.addComponent(new VulkanGraphicsPipeline(mainDescriptorSet->getComponent<VulkanDescriptorSetLayout>()));
        pipelineNode.addComponent(vertexInput);

        scene.addComponent(new RenderNode(&pipelineNode, RENDER_ACTION_START));
        EntityNode* drawObject = new EntityNode(&scene);
            drawObject->addComponent(new VulkanCmdBindDescriptorSet(mainDescriptorSet->getComponent<VulkanDescriptorSet>(), pipelineNode.getComponent<VulkanGraphicsPipeline>()));
            drawObject->addComponent(new RenderNode(&graphicsObjects));
            drawObject->addComponent(new VulkanCmdBindDescriptorSet(secondDescriptorSet->getComponent<VulkanDescriptorSet>(), pipelineNode.getComponent<VulkanGraphicsPipeline>()));
            drawObject->addComponent(new RenderNode(&graphicsObjects));
            drawObject->addComponent(new RenderNode(&pipelineNode, RENDER_ACTION_END));

        vulkanNode.addComponent(new VulkanInstance());
        EntityNode* surfaceNode = new EntityNode(&vulkanNode);
            surfaceNode->addComponent(new GlfwSurface(window, &vulkanNode));
        EntityNode* surface2Node = new EntityNode(&vulkanNode);
            surface2Node->addComponent(new GlfwSurface(window2, &vulkanNode));
        EntityNode* surface3Node = new EntityNode(&vulkanNode);
            surface3Node->addComponent(new GlfwSurface(window3, &vulkanNode));
        Entity* deviceNode = new EntityNode(&vulkanNode);
            deviceNode->addComponent(new SharedContext());
            deviceNode->addComponent(new VulkanDevice(&vulkanNode));
            EntityNode* queues = new EntityNode(deviceNode);
                queues->addComponent(new VulkanGraphicsQueue());
                queues->addComponent(new VulkanPresentQueue(surfaceNode));
                queues->addComponent(new VulkanPresentQueue(surface2Node));
                queues->addComponent(new VulkanPresentQueue(surface3Node));
                graphicsQueue = queues->getComponent<VulkanGraphicsQueue>();
                presentQueue = queues->getComponent<VulkanPresentQueue>();
            EntityNode* updateSharedNode = new EntityNode(deviceNode);
                updateSharedNode->addComponent(new AllowRenderModes(VULKAN_RENDER_UPDATE_SHARED | VULKAN_RENDER_OBJECT | VULKAN_RENDER_CLEANUP_SHARED));
                updateSharedNode->addComponent(new VulkanDeviceRenderer());
                updateSharedNode->addComponent(new VulkanCommandPool(graphicsQueue));
                updateSharedNode->addComponent(new RenderNode(&graphicsObjects));
                updateSharedNode->addComponent(new RenderNode(&images));
                EntityNode* renderSpecific = new EntityNode(updateSharedNode);
                    renderSpecific->addComponent(new AllowRenderModes(VULKAN_RENDER_SHARED_ONLY));
                    renderSpecific->addComponent(new RenderNode(&shaderObjects));//, UPDATE_ONLY));
                    renderSpecific->addComponent(new RenderNode(&descriptorSetGroup));
                    renderSpecific->addComponent(new RenderNode(&pipelineNode));//, UPDATE_ONLY));
            EntityNode* windowNodes = new EntityNode(deviceNode);
                renderNode0 = createSwapChain(windowNodes, surfaceNode, "window 1");
                renderNode2 = createSwapChain(windowNodes, surface2Node, "window 2");
                renderNode3 = createSwapChain(windowNodes, surface3Node, "window 5");

        vulkanNode.update();

        VulkanDeviceRenderer* sharedRenderer = updateSharedNode->getComponentRecursive<VulkanDeviceRenderer>();
        sharedRenderer->render(VULKAN_RENDER_UPDATE_SHARED);
        sharedRenderer->render(VULKAN_RENDER_OBJECT);

        VulkanDeviceRenderer* renderer1 = renderNode0->getComponentRecursive<VulkanDeviceRenderer>();
        renderer1->render(VULKAN_RENDER_UPDATE_SHARED);
        renderer1->render(VULKAN_RENDER_UPDATE_DISPLAY);
        VulkanDeviceRenderer* renderer2 = renderNode2->getComponentRecursive<VulkanDeviceRenderer>();
        renderer2->render(VULKAN_RENDER_UPDATE_SHARED);
        renderer2->render(VULKAN_RENDER_UPDATE_DISPLAY);
        VulkanDeviceRenderer* renderer3 = renderNode3->getComponentRecursive<VulkanDeviceRenderer>();
        renderer3->render(VULKAN_RENDER_UPDATE_SHARED);
        renderer3->render(VULKAN_RENDER_UPDATE_DISPLAY);

        std::vector<VulkanDeviceRenderer*> renderers = windowNodes->getComponentsRecursive<VulkanDeviceRenderer>();
        for (int f = 0; f < renderers.size(); f++) {  
            renderers[f]->render(VULKAN_RENDER_UPDATE);
            renderers[f]->render(VULKAN_RENDER_OBJECT);
            renderers[f]->render(VULKAN_RENDER_COMMAND);
        }
    }

    EntityNode* createSwapChain(Entity* parentNode, Entity* surfaceNode, std::string name) {
        EntityNode* renderNode = new EntityNode(parentNode);
            renderNode->addComponent(new VulkanBasicSwapChain(name, surfaceNode));
            renderNode->addComponent(new VulkanFrameRenderer(graphicsQueue, presentQueue));
            EntityNode* updateNode = new EntityNode(renderNode);
                updateNode->addComponent(new AllowRenderModes(VULKAN_RENDER_UPDATE_DISPLAY | VULKAN_RENDER_UPDATE | VULKAN_RENDER_OBJECT | VULKAN_RENDER_CLEANUP_DISPLAY | VULKAN_RENDER_CLEANUP));
                updateNode->addComponent(new RenderNode(&shaderObjects));
                updateNode->addComponent(new RenderNode(&descriptorSetGroup));
                updateNode->addComponent(new RenderNode(&pipelineNode));
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

            // independent windows
            renderNode0->getComponent<VulkanFrameRenderer>()->drawFrame();
            renderNode2->getComponent<VulkanFrameRenderer>()->drawFrame();
            renderNode3->getComponent<VulkanFrameRenderer>()->drawFrame();

            // syncronized windows
            //renderNode0->getComponent<VulkanFrameRenderer>()->drawFrame(true, renderNode0);
            //renderNode0->getComponent<VulkanFrameRenderer>()->drawFrame(true, renderNode2);
            //renderNode0->getComponent<VulkanFrameRenderer>()->drawFrame(true, renderNode3);
            //renderNode0->getComponent<VulkanFrameRenderer>()->incrementFrame();

            frame++;
        }

        vkDeviceWaitIdle(device);
    }


    void cleanup() {
        glfwDestroyWindow(window);
        glfwDestroyWindow(window2);
        glfwDestroyWindow(window3);

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

