

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

const int WIDTH = 800;
const int HEIGHT = 600;

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

    VkInstance instance;
    VkSurfaceKHR surface;

    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice device;

    VulkanQueue* graphicsQueue;
    VulkanQueue* presentQueue;

    VkSwapchainKHR swapChain;
    std::vector<VkImage> swapChainImages;
    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;
    std::vector<VkImageView> swapChainImageViews;
    VulkanFramebuffer* framebuffer;

    VkRenderPass renderPass;
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
    VkDeviceMemory indexBufferMemory;
    VertexArray<Vertex>* vertexArray;
    IndexArray* indexArray;

    std::vector<VkBuffer> uniformBuffers;
    std::vector<VkDeviceMemory> uniformBuffersMemory;
    VulkanUniformBuffer* uniformBuffer;

    VkDescriptorPool descriptorPool;
    std::vector<VkDescriptorSet> descriptorSets;
    VulkanDescriptorPool* vulkanDescriptorPool;
    VulkanDescriptorSet* descriptorSet;

    std::vector<VkCommandBuffer> commandBuffers;

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
    EntityNode images;
    Entity* mainImage;
    Entity* renderNode;
    GraphicsRenderer* renderer;
    EntityNode* objectNode;

    void initWindow() {
        glfwInit();

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
        glfwSetWindowUserPointer(window, this);
        glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);

        window2 = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
        glfwSetWindowUserPointer(window2, this);
        glfwSetFramebufferSizeCallback(window2, framebufferResizeCallback);
        glfwSetWindowPos (window2, WIDTH+5, 0);
    }

    static void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
        auto app = reinterpret_cast<HelloTriangleApplication*>(glfwGetWindowUserPointer(window));
        app->framebufferResized = true;
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
        pipelineNode.addComponent(new VulkanGraphicsPipeline(mainDescriptorSet->getComponent<VulkanDescriptorSetLayout>()));
        pipelineNode.addComponent(vertexInput);


        /*EntityNode* scene;
            scene->addComponent(VertexBuffer);
            scene->addComponent(Image);*/

        

        vulkanNode.addComponent(new VulkanInstance());
        EntityNode* surfaceNode = new EntityNode(&vulkanNode);
            surfaceNode->addComponent(new GlfwSurface(window, &vulkanNode));
        EntityNode* surface2Node = new EntityNode(&vulkanNode);
            surfaceNode->addComponent(new GlfwSurface(window2, &vulkanNode));
        EntityNode* deviceNode = new EntityNode(&vulkanNode);
            deviceNode->addComponent(new VulkanDevice(&vulkanNode));
            EntityNode* queues = new EntityNode(deviceNode);
                graphicsQueue = new VulkanGraphicsQueue();
                queues->addComponent(graphicsQueue);
                queues->addComponent(new VulkanPresentQueue(surfaceNode));
            renderNode = new EntityNode(deviceNode);
                renderNode->addComponent(new VulkanBasicSwapChain(surfaceNode));
                renderNode->addComponent(new VulkanBasicRenderPass());
                renderNode->addComponent(new VulkanSwapChainFramebuffer());
                EntityNode* graphicsNode = new EntityNode(renderNode);
                    graphicsNode->addComponent(new RenderNode(&shaderObjects));//, UPDATE_ONLY));
                    graphicsNode->addComponent(new RenderNode(&descriptorSetGroup)); //, UPDATE_ONLY));
                    graphicsNode->addComponent(new RenderNode(&pipelineNode));//, UPDATE_ONLY));

                    EntityNode* commandNode = new EntityNode(graphicsNode);
                        commandNode->addComponent(new VulkanCommandPool(graphicsQueue));
                        commandNode->addComponent(new VulkanCommandBuffer());
                        //commandNode->addComponent(new RenderNode(scene, RENDER_ONLY));
            objectNode = new EntityNode(deviceNode);
                objectNode->addComponent(new VulkanDeviceRenderer(new GraphicsContext(renderNode->getComponent<VulkanBasicSwapChain>()->getSharedContext(), new Context(), false)));
                objectNode->addComponent(new VulkanCommandPool(graphicsQueue));
                objectNode->addComponent(new RenderNode(&graphicsObjects));
                objectNode->addComponent(new RenderNode(&images));


        //createDevice(window2);

        vulkanNode.update();

        std::vector<VulkanDeviceRenderer*> renderers = deviceNode->getComponentsRecursive<VulkanDeviceRenderer>();
        renderers[renderers.size()-1]->render(VULKAN_RENDER_UPDATE_SHARED);
        renderers[0]->render(VULKAN_RENDER_UPDATE_SHARED);
        for (int f = 0; f < renderers.size(); f++) {  
            renderers[f]->render(VULKAN_RENDER_UPDATE);
            renderers[f]->render(VULKAN_RENDER_OBJECT);
        }

        //instance = vulkanNode.getComponent<VulkanInstance>()->getInstance();
        //surface = surfaceNode->getComponent<VulkanSurface>()->getSurface();
        device = deviceNode->getComponent<VulkanDevice>()->getDevice();
        physicalDevice = deviceNode->getComponent<VulkanDevice>()->getPhysicalDevice();
        graphicsQueue = queues->getComponent<VulkanGraphicsQueue>();
        presentQueue = queues->getComponent<VulkanPresentQueue>();

        swapChain = renderNode->getComponent<VulkanBasicSwapChain>()->swapChain;
        swapChainImages = renderNode->getComponent<VulkanBasicSwapChain>()->swapChainImages;
        //swapChainImageFormat = renderNode->getComponent<VulkanBasicSwapChain>()->swapChainImageFormat;
        swapChainExtent = renderNode->getComponent<VulkanBasicSwapChain>()->swapChainExtent;
        swapChainImageViews = renderNode->getComponent<VulkanBasicSwapChain>()->swapChainImageViews;
        commandPool = commandNode->getComponent<VulkanCommandPool>()->getCommandPool();
        renderer = renderNode->getComponents<GraphicsRenderer>()[1];
        renderPass = renderNode->getComponent<VulkanRenderPass>()->getRenderPass(renderer->getContext());

        graphicsPipeline = pipelineNode.getComponent<VulkanGraphicsPipeline>()->getGraphicsPipeline(renderer->getContext());
        pipelineLayout = pipelineNode.getComponent<VulkanGraphicsPipeline>()->getPipelineLayout(renderer->getContext());
        descriptorSetLayout = mainDescriptorSet->getComponent<VulkanDescriptorSetLayout>()->getDescriptorSetLayout(renderer->getContext());
        framebuffer = renderNode->getComponent<VulkanFramebuffer>();
        //createInstance();
        //setupDebugMessenger();
        //createSurface();
        //pickPhysicalDevice();
        //createLogicalDevice();
        //createSwapChain();
        //createImageViews();
        //createRenderPass();
        //createDescriptorSetLayout();
        //createGraphicsPipeline();
        //createFramebuffers();
        //createCommandPool();
        //createTextureImage();
        //createTextureImageView();
        //createTextureSampler();
        //createVertexBuffer();
        //createIndexBuffer();
        //createUniformBuffers();
        //createDescriptorPool();
        //createDescriptorSets();
        createCommandBuffers();
        createSyncObjects();
    }

    void mainLoop() {
        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();
            drawFrame();
        }

        vkDeviceWaitIdle(device);
    }

    void cleanupSwapChain() {
        /*for (auto framebuffer : swapChainFramebuffers) {
            vkDestroyFramebuffer(device, framebuffer, nullptr);
        }*/

        vkFreeCommandBuffers(device, commandPool, static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());

        vkDestroyPipeline(device, graphicsPipeline, nullptr);
        vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
        vkDestroyRenderPass(device, renderPass, nullptr);

        for (auto imageView : swapChainImageViews) {
            vkDestroyImageView(device, imageView, nullptr);
        }

        vkDestroySwapchainKHR(device, swapChain, nullptr);

        for (size_t i = 0; i < swapChainImages.size(); i++) {
            vkDestroyBuffer(device, uniformBuffers[i], nullptr);
            vkFreeMemory(device, uniformBuffersMemory[i], nullptr);
        }

        vkDestroyDescriptorPool(device, descriptorPool, nullptr);
    }

    void cleanup() {
        cleanupSwapChain();

        vkDestroySampler(device, textureSampler, nullptr);
        vkDestroyImageView(device, textureImageView, nullptr);

        vkDestroyImage(device, textureImage, nullptr);
        vkFreeMemory(device, textureImageMemory, nullptr);

        vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);

        vkDestroyBuffer(device, indexBuffer, nullptr);
        vkFreeMemory(device, indexBufferMemory, nullptr);

        vkDestroyBuffer(device, vertexBuffer, nullptr);
        vkFreeMemory(device, vertexBufferMemory, nullptr);

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            vkDestroySemaphore(device, renderFinishedSemaphores[i], nullptr);
            vkDestroySemaphore(device, imageAvailableSemaphores[i], nullptr);
            vkDestroyFence(device, inFlightFences[i], nullptr);
        }

        vkDestroyCommandPool(device, commandPool, nullptr);

        //vkDestroyDevice(device, nullptr);

        //vkDestroySurfaceKHR(instance, surface, nullptr);

        glfwDestroyWindow(window);
        glfwDestroyWindow(window2);

        glfwTerminate();
    }

    void recreateSwapChain() {
        int width = 0, height = 0;
        while (width == 0 || height == 0) {
            glfwGetFramebufferSize(window, &width, &height);
            glfwWaitEvents();
        }

        vkDeviceWaitIdle(device);

        cleanupSwapChain();

        //createSwapChain();
        //createImageViews();
        //createRenderPass();
        //createGraphicsPipeline();
        //createFramebuffers();
        //createUniformBuffers();
        //createDescriptorPool();
        //createDescriptorSets();
        createCommandBuffers();
    }


    void createCommandBuffers() {

        commandBuffers.resize(swapChainImageViews.size());

        VkCommandBufferAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = commandPool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = (uint32_t) commandBuffers.size();

        if (vkAllocateCommandBuffers(device, &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate command buffers!");
        }

        for (size_t i = 0; i < commandBuffers.size(); i++) {
            VulkanDeviceRenderer* renderer = renderNode->getComponents<VulkanDeviceRenderer>()[i];
            renderer->render(VULKAN_RENDER_COMMAND);

            VkCommandBufferBeginInfo beginInfo = {};
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

            if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS) {
                throw std::runtime_error("failed to begin recording command buffer!");
            }

            VkRenderPassBeginInfo renderPassInfo = {};
            renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            renderPassInfo.renderPass = renderPass;
            renderPassInfo.framebuffer = framebuffer->getFramebuffer(renderer->getContext());
            renderPassInfo.renderArea.offset = {0, 0};
            renderPassInfo.renderArea.extent = swapChainExtent;

            VkClearValue clearColor = {0.0f, 0.0f, 0.0f, 1.0f};
            renderPassInfo.clearValueCount = 1;
            renderPassInfo.pClearValues = &clearColor;

            vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

                vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

                VkBuffer vertexBuffers[] = {vertexArray->getBuffer(renderer->getContext())};
                VkDeviceSize offsets[] = {0};
                vkCmdBindVertexBuffers(commandBuffers[i], 0, 1, vertexBuffers, offsets);

                vkCmdBindIndexBuffer(commandBuffers[i], indexArray->getBuffer(renderer->getContext()), 0, VK_INDEX_TYPE_UINT16);

                VkDescriptorSet descSet = descriptorSet->getDescriptorSet(renderer->getContext());
                std::cout << descSet << std::endl;
                vkCmdBindDescriptorSets(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descSet, 0, nullptr);

                vkCmdDrawIndexed(commandBuffers[i], static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);

            vkCmdEndRenderPass(commandBuffers[i]);

            if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS) {
                throw std::runtime_error("failed to record command buffer!");
            }
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

    void drawFrame() {
        vkWaitForFences(device, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

        uint32_t imageIndex;
        VkResult result = vkAcquireNextImageKHR(device, swapChain, UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            recreateSwapChain();
            return;
        } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("failed to acquire swap chain image!");
        }

        //updateUniformBuffer(imageIndex);
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
        submitInfo.pCommandBuffers = &commandBuffers[imageIndex];

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
            framebufferResized = false;
            recreateSwapChain();
        } else if (result != VK_SUCCESS) {
            throw std::runtime_error("failed to present swap chain image!");
        }

        currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    }

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
        std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

        return VK_FALSE;
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

