

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

const int WIDTH = 500;
const int HEIGHT = 400;

const int MAX_FRAMES_IN_FLIGHT = 2;

/*struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};*/


struct Vertex {
    glm::vec2 pos;
    glm::vec3 color;
    glm::vec2 texCoord;

    static VkVertexInputBindingDescription getBindingDescription() {
        VkVertexInputBindingDescription bindingDescription = {};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescription;
    }

    static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions() {
        std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions = {};

        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex, pos);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, color);

        attributeDescriptions[2].binding = 0;
        attributeDescriptions[2].location = 2;
        attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

        return attributeDescriptions;
    }
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

    VkInstance instance;
    VkSurfaceKHR surface;

    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice device;

    VulkanQueue* graphicsQueue;
    VulkanQueue* presentQueue;

    /*VkSwapchainKHR swapChain;
    std::vector<VkImage> swapChainImages;
    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;
    std::vector<VkImageView> swapChainImageViews;*/
    VulkanFramebuffer* framebuffer;

    /*VkRenderPass renderPass;
    VkDescriptorSetLayout descriptorSetLayout;
    VkPipelineLayout pipelineLayout;
    VkPipeline graphicsPipeline;

    VkCommandPool commandPool;

    VkImage textureImage;
    VkDeviceMemory textureImageMemory;
    VkImageView textureImageView;
    VkSampler textureSampler;
    VulkanSampler* sampler;

    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;
    VkBuffer indexBuffer;
    VkDeviceMemory indexBufferMemory;*/
    VertexArray<Vertex>* vertexArray;
    IndexArray* indexArray;

    VulkanSampler* sampler;

    //std::vector<VkBuffer> uniformBuffers;
    //std::vector<VkDeviceMemory> uniformBuffersMemory;
    VulkanUniformBuffer* uniformBuffer;

    //VkDescriptorPool descriptorPool;
    //std::vector<VkDescriptorSet> descriptorSets;
    VulkanDescriptorPool* vulkanDescriptorPool;
    VulkanDescriptorSet* descriptorSet;

    //std::vector<VkCommandBuffer> commandBuffers;
    VulkanCommandBuffer* commandBuffer;

    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;
    size_t currentFrame = 0;

    bool framebufferResized = false;

    EntityNode vulkanNode;
    EntityNode pipelineNode;
    EntityNode shaderObjects;
    EntityNode graphicsObjects;
    EntityNode descriptorSetGroup;
    EntityNode scene;
    EntityNode images;
    Entity* mainImage;
    Entity* renderNode0;
    Entity* renderNode2;
    Entity* renderNode3;
    Entity* deviceNode;
    EntityNode* objectNode;

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

        mainImage = new EntityNode(&images);
            mainImage->addComponent(new Image("examples/VulkanExample/textures/texture.jpg"));
            mainImage->addComponent(new VulkanImage());
            mainImage->addComponent(new VulkanImageView());
        images.update();

        //shaderObjects.addComponent(uniformBuffer);
        EntityNode* mainUniformBuffer = new EntityNode(&shaderObjects);
            uniformBuffer = new MainUniformBuffer();
            mainUniformBuffer->addComponent(uniformBuffer);
        EntityNode* samplerNode = new EntityNode(&shaderObjects);
            sampler = new VulkanSampler();
            samplerNode->addComponent(sampler);


        EntityNode* mainDescriptorSet = new EntityNode(&descriptorSetGroup);
            mainDescriptorSet->addComponent(new VulkanDescriptorSetLayout());
            vulkanDescriptorPool = new VulkanSwapChainDescriptorPool();
            mainDescriptorSet->addComponent(vulkanDescriptorPool);
            descriptorSet = new VulkanDescriptorSet();
            mainDescriptorSet->addComponent(descriptorSet);
            mainDescriptorSet->addComponent(new VulkanDescriptor(uniformBuffer, VK_SHADER_STAGE_VERTEX_BIT));
            mainDescriptorSet->addComponent(new VulkanDescriptor(new VulkanImageViewDecorator(sampler, mainImage->getComponent<VulkanImageView>()), VK_SHADER_STAGE_FRAGMENT_BIT));

        vertexArray = new VertexArray<Vertex>();
        vertexArray->value = vertices;
        indexArray = new IndexArray();
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
            drawObject->addComponent(new RenderNode(&pipelineNode, RENDER_ACTION_END));

        vulkanNode.addComponent(new VulkanInstance());
        EntityNode* surfaceNode = new EntityNode(&vulkanNode);
            surfaceNode->addComponent(new GlfwSurface(window, &vulkanNode));
        EntityNode* surface2Node = new EntityNode(&vulkanNode);
            surface2Node->addComponent(new GlfwSurface(window2, &vulkanNode));
        EntityNode* surface3Node = new EntityNode(&vulkanNode);
            surface3Node->addComponent(new GlfwSurface(window3, &vulkanNode));
        deviceNode = new EntityNode(&vulkanNode);
            deviceNode->addComponent(new SharedContext());
            deviceNode->addComponent(new VulkanDevice(&vulkanNode));
            EntityNode* queues = new EntityNode(deviceNode);
                graphicsQueue = new VulkanGraphicsQueue();
                queues->addComponent(graphicsQueue);
                queues->addComponent(new VulkanPresentQueue(surfaceNode));
                queues->addComponent(new VulkanPresentQueue(surface2Node));
                queues->addComponent(new VulkanPresentQueue(surface3Node));
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

        std::vector<VulkanDeviceRenderer*> renderers = windowNodes->getComponentsRecursive<VulkanDeviceRenderer>();
        //renderers[0]->render(VULKAN_RENDER_UPDATE_SHARED);
        //renderers[0]->render(VULKAN_RENDER_OBJECT);
        VulkanDeviceRenderer* renderer1 = renderNode0->getComponentRecursive<VulkanDeviceRenderer>();
        renderer1->render(VULKAN_RENDER_UPDATE_SHARED);
        renderer1->render(VULKAN_RENDER_UPDATE_DISPLAY);
        VulkanDeviceRenderer* renderer2 = renderNode2->getComponentRecursive<VulkanDeviceRenderer>();
        renderer2->render(VULKAN_RENDER_UPDATE_SHARED);
        renderer2->render(VULKAN_RENDER_UPDATE_DISPLAY);
        VulkanDeviceRenderer* renderer3 = renderNode3->getComponentRecursive<VulkanDeviceRenderer>();
        renderer3->render(VULKAN_RENDER_UPDATE_SHARED);
        renderer3->render(VULKAN_RENDER_UPDATE_DISPLAY);
        for (int f = 0; f < renderers.size(); f++) {  
            renderers[f]->render(VULKAN_RENDER_UPDATE);
            renderers[f]->render(VULKAN_RENDER_OBJECT);
            renderers[f]->render(VULKAN_RENDER_COMMAND);
        }

        device = deviceNode->getComponent<VulkanDevice>()->getDevice();
        physicalDevice = deviceNode->getComponent<VulkanDevice>()->getPhysicalDevice();
        graphicsQueue = queues->getComponent<VulkanGraphicsQueue>();
        presentQueue = queues->getComponent<VulkanPresentQueue>();

        createSyncObjects();
    }

    EntityNode* createSwapChain(Entity* parentNode, Entity* surfaceNode, std::string name) {
        EntityNode* renderNode = new EntityNode(parentNode);
                renderNode->addComponent(new VulkanBasicSwapChain(name, surfaceNode));
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

            drawFrame(renderNode0);
            drawFrame(renderNode2);
            drawFrame(renderNode3);

            currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
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

    void recreateSwapChain(VulkanSwapChain* swapChain) {
        int width = 0, height = 0;
        while (width == 0 || height == 0) {
            glfwGetFramebufferSize(window, &width, &height);
            glfwWaitEvents();
        }

        vkDeviceWaitIdle(device);

        swapChain->getEntity().incrementVersion();
        swapChain->getEntity().update();

        std::cout << swapChain << std::endl;
        std::vector<VulkanDeviceRenderer*> renderers = swapChain->getEntity().getComponentsRecursive<VulkanDeviceRenderer>();
        std::cout << "num renderers: " << renderers.size() << std::endl;
        renderers[0]->render(VULKAN_RENDER_UPDATE_SHARED);
        renderers[0]->render(VULKAN_RENDER_UPDATE_DISPLAY);
        for (int f = 0; f < renderers.size(); f++) {  
            renderers[f]->render(VULKAN_RENDER_UPDATE);
            renderers[f]->render(VULKAN_RENDER_OBJECT);
            renderers[f]->render(VULKAN_RENDER_COMMAND);

        }

    }

    void createSyncObjects() {
        imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

        VkSemaphoreCreateInfo semaphoreInfo = {};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceInfo = {};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
                vkCreateSemaphore(device, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
                vkCreateFence(device, &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS) {
                throw std::runtime_error("failed to create synchronization objects for a frame!");
            }
        }
    }

    void drawFrame(Entity* renderNode) {
        vkWaitForFences(device, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

        VkSwapchainKHR swapChain = renderNode->getComponent<VulkanSwapChain>()->getSwapChain();

        uint32_t imageIndex;
        VkResult result = vkAcquireNextImageKHR(device, swapChain, UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            std::cout << "Out of date" << std::endl;
            recreateSwapChain(renderNode->getComponent<VulkanSwapChain>());
            return;
        } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("failed to acquire swap chain image!");
        }

        VulkanDeviceRenderer* vulkanRenderer = renderNode->getComponents<VulkanDeviceRenderer>()[imageIndex];
        vulkanRenderer->render(VULKAN_RENDER_OBJECT);

        VkSubmitInfo submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkSemaphore waitSemaphores[] = {imageAvailableSemaphores[currentFrame]};
        VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;

        submitInfo.commandBufferCount = 1;
        VkCommandBuffer cmdBuffer = renderNode->getComponentRecursive<VulkanCommandBuffer>()->getCommandBuffer(vulkanRenderer->getContext());
        submitInfo.pCommandBuffers = &cmdBuffer;

        VkSemaphore signalSemaphores[] = {renderFinishedSemaphores[currentFrame]};
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        vkResetFences(device, 1, &inFlightFences[currentFrame]);

        if (vkQueueSubmit(graphicsQueue->getQueue(), 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS) {
            throw std::runtime_error("failed to submit draw command buffer!");
        }

        VkPresentInfoKHR presentInfo = {};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;

        VkSwapchainKHR swapChains[] = {swapChain};
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;

        presentInfo.pImageIndices = &imageIndex;

        result = vkQueuePresentKHR(presentQueue->getQueue(), &presentInfo);

        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized) {
            std::cout << "Out of date / suboptimal / resized" << std::endl;
            framebufferResized = false;
            recreateSwapChain(renderNode->getComponent<VulkanSwapChain>());
        } else if (result != VK_SUCCESS) {
            throw std::runtime_error("failed to present swap chain image!");
        }

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

