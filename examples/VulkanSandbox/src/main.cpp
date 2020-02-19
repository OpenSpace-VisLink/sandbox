

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
//#include <optional>
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
        ubo.view = glm::lookAt(glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
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
    GLFWwindow* window2;
    GLFWwindow* window3;

    VkDevice device;
    VkDevice device2;

    VulkanQueue* graphicsQueue;
    VulkanQueue* presentQueue;

    EntityNode vulkanNode;
    EntityNode renderPassNode;
    EntityNode pipelineNode;
    EntityNode pipelineNode2;
    EntityNode pipelineNode3;
    EntityNode shaderObjects;
    EntityNode graphicsObjects;
    EntityNode descriptorSetGroup;
    EntityNode scene;
    EntityNode input;
    EntityNode images;
    EntityNode* updateSharedNode;
    EntityNode* sceneGraph;
    Entity* renderNode0;
    Entity* renderNode2;
    Entity* renderNode3;

    void initWindow() { 
        glfwInit();

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
        glfwSetWindowUserPointer(window, this);
        glfwSetWindowPos (window, 0, 0);

        window2 = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
        glfwSetWindowUserPointer(window2, this);
        glfwSetWindowPos (window2, WIDTH+5, 0);

        window3 = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
        glfwSetWindowUserPointer(window3, this);
        glfwSetWindowPos (window3, 2*(WIDTH+5), 0);
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
        EntityNode* mainUniformBuffer2 = new EntityNode(&shaderObjects);
            mainUniformBuffer2->addComponent(new MainUniformBuffer2());
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
           //mainDescriptorSet->addComponent(new VulkanDescriptor(transformUniformBuffer->getComponent<VulkanUniformBuffer>(), VK_SHADER_STAGE_VERTEX_BIT));

        EntityNode* mainDescriptorSet2 = new EntityNode(&descriptorSetGroup);
            mainDescriptorSet2->addComponent(new VulkanDescriptorSetLayout()); 
            mainDescriptorSet2->addComponent(new VulkanSwapChainDescriptorPool());
            mainDescriptorSet2->addComponent(new VulkanDescriptorSet());
            mainDescriptorSet2->addComponent(new VulkanDescriptor(mainUniformBuffer2->getComponent<VulkanUniformBuffer>(), VK_SHADER_STAGE_VERTEX_BIT));
            mainDescriptorSet2->addComponent(new VulkanDescriptor(new VulkanImageViewDecorator(samplerNode->getComponent<VulkanSampler>(), mainImage->getComponent<VulkanImageView>()), VK_SHADER_STAGE_FRAGMENT_BIT));
           //mainDescriptorSet->addComponent(new VulkanDescriptor(transformUniformBuffer->getComponent<VulkanUniformBuffer>(), VK_SHADER_STAGE_VERTEX_BIT));


        EntityNode* secondDescriptorSet = new EntityNode(&descriptorSetGroup);
            secondDescriptorSet->addComponent(new VulkanDescriptorSetLayout());
            secondDescriptorSet->addComponent(new VulkanSwapChainDescriptorPool());
            secondDescriptorSet->addComponent(new VulkanDescriptorSet());
            secondDescriptorSet->addComponent(new VulkanDescriptor(mainUniformBuffer->getComponent<VulkanUniformBuffer>(), VK_SHADER_STAGE_VERTEX_BIT));
            secondDescriptorSet->addComponent(new VulkanDescriptor(new VulkanImageViewDecorator(samplerNode->getComponent<VulkanSampler>(), secondImage->getComponent<VulkanImageView>()), VK_SHADER_STAGE_FRAGMENT_BIT));


        EntityNode* transformDescriptorSet = new EntityNode(&descriptorSetGroup);
            transformDescriptorSet->addComponent(new VulkanDescriptorSetLayout());
            transformDescriptorSet->addComponent(new VulkanSwapChainDescriptorPool());
            transformDescriptorSet->addComponent(new VulkanDescriptorSet());
            transformDescriptorSet->addComponent(new VulkanDescriptor(transformUniformBuffer->getComponent<VulkanUniformBuffer>(), VK_SHADER_STAGE_VERTEX_BIT));



        EntityNode* quad = new EntityNode(&graphicsObjects);
            VertexArray<Vertex>* vertexArray = new VertexArray<Vertex>();
            vertexArray->value = vertices;
            IndexArray16* indexArray = new IndexArray16();
            indexArray->value = indices;
            quad->addComponent(vertexArray);
            vertexArray = new VertexArray<Vertex>(1);
            vertexArray->value = tri_vertices;
            quad->addComponent(vertexArray);
            quad->addComponent(indexArray);
        EntityNode* triangle = new EntityNode(&graphicsObjects);
            VertexArray<Vertex>* tri_vertexArray = new VertexArray<Vertex>();
            tri_vertexArray->value = tri_vertices;
            IndexArray16* tri_indexArray = new IndexArray16();
            tri_indexArray->value = tri_indices;
            triangle->addComponent(tri_vertexArray);
            tri_vertexArray = new VertexArray<Vertex>(1);
            tri_vertexArray->value = tri_vertices;
            triangle->addComponent(tri_vertexArray);
            triangle->addComponent(tri_indexArray);
        EntityNode* cylindar = new EntityNode(&graphicsObjects);
            Mesh* mesh = new Mesh();
            cylindar->addComponent(mesh);
            cylindar->addComponent(new ShapeLoader(SHAPE_CYLINDAR, 20));
            //cylindar->addComponent(new ShapeLoader(SHAPE_CIRCLE, 20));
            //cylindar->addComponent(new ShapeLoader(SHAPE_QUAD));
            cylindar->update();
            VertexArray<glm::vec3>* cylindar_vertexArray = new VertexArray<glm::vec3>(0);
            cylindar_vertexArray->value = mesh->getNodes();
            cylindar->addComponent(cylindar_vertexArray);
            cylindar_vertexArray = new VertexArray<glm::vec3>(1);
            cylindar_vertexArray->value = mesh->getNormals();
            cylindar->addComponent(cylindar_vertexArray);
            VertexArray<glm::vec2>* cylindar_coords = new VertexArray<glm::vec2>(2);
            cylindar_coords->value = mesh->getCoords();
            cylindar->addComponent(cylindar_coords);
            IndexArray* cylindar_indexArray = new IndexArray();
            cylindar_indexArray->value = mesh->getIndices();
            cylindar->addComponent(cylindar_indexArray);



        pipelineNode.addComponent(new VulkanShaderModule("examples/VulkanSandbox/src/shaders/vert.spv", VK_SHADER_STAGE_VERTEX_BIT));
        pipelineNode.addComponent(new VulkanShaderModule("examples/VulkanSandbox/src/shaders/frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT));
        std::vector<VulkanDescriptorSetLayout*> layouts;
        layouts.push_back(mainDescriptorSet->getComponent<VulkanDescriptorSetLayout>());
        layouts.push_back(transformDescriptorSet->getComponent<VulkanDescriptorSetLayout>());
        pipelineNode.addComponent(new VulkanGraphicsPipeline(layouts));
        VulkanVertexInput* vertexInput = new VulkanVertexInput(sizeof(Vertex));
        vertexInput->addAttribute(VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, pos));
        vertexInput->addAttribute(VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color));
        vertexInput->addAttribute(VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, texCoord));
        pipelineNode.addComponent(vertexInput);
        vertexInput = new VulkanVertexInput(sizeof(Vertex),3);
        vertexInput->addAttribute(VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, pos));
        vertexInput->addAttribute(VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color));
        vertexInput->addAttribute(VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, texCoord));
        pipelineNode.addComponent(vertexInput);

        renderPassNode.addComponent(new VulkanBasicRenderPass());
        renderPassNode.addComponent(new VulkanSwapChainFramebuffer());


        pipelineNode2.addComponent(new VulkanShaderModule("examples/VulkanSandbox/src/shaders/vert.spv", VK_SHADER_STAGE_VERTEX_BIT));
        pipelineNode2.addComponent(new VulkanShaderModule("examples/VulkanSandbox/src/shaders/frag2.spv", VK_SHADER_STAGE_FRAGMENT_BIT));
        std::vector<VulkanDescriptorSetLayout*> layouts2;
        layouts2.push_back(mainDescriptorSet->getComponent<VulkanDescriptorSetLayout>());
        layouts2.push_back(transformDescriptorSet->getComponent<VulkanDescriptorSetLayout>());
        pipelineNode2.addComponent(new VulkanGraphicsPipeline(layouts));
        VulkanVertexInput* vertexInput2 = new VulkanVertexInput(sizeof(Vertex));
        vertexInput2->addAttribute(VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, pos));
        vertexInput2->addAttribute(VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color));
        vertexInput2->addAttribute(VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, texCoord));
        pipelineNode2.addComponent(vertexInput2);


        pipelineNode3.addComponent(new VulkanShaderModule("examples/VulkanSandbox/src/shaders/vert2.spv", VK_SHADER_STAGE_VERTEX_BIT));
        pipelineNode3.addComponent(new VulkanShaderModule("examples/VulkanSandbox/src/shaders/frag3.spv", VK_SHADER_STAGE_FRAGMENT_BIT));
        std::vector<VulkanDescriptorSetLayout*> layouts3;
        layouts3.push_back(mainDescriptorSet->getComponent<VulkanDescriptorSetLayout>());
        layouts3.push_back(transformDescriptorSet->getComponent<VulkanDescriptorSetLayout>());
        pipelineNode3.addComponent(new VulkanGraphicsPipeline(layouts));
        VulkanVertexInput* vertexInput3 = new VulkanVertexInput(sizeof(glm::vec3), 0);
        vertexInput3->addAttribute(VK_FORMAT_R32G32B32_SFLOAT, 0);
        pipelineNode3.addComponent(vertexInput3);
        vertexInput3 = new VulkanVertexInput(sizeof(glm::vec3), 1);
        vertexInput3->addAttribute(VK_FORMAT_R32G32B32_SFLOAT, 0);
        pipelineNode3.addComponent(vertexInput3);
        vertexInput3 = new VulkanVertexInput(sizeof(glm::vec2), 2);
        vertexInput3->addAttribute(VK_FORMAT_R32G32_SFLOAT, 0);
        pipelineNode3.addComponent(vertexInput3);


        UniformBufferIterator bufferTransform(transformUniformBuffer->getComponent<VulkanUniformBuffer>(), transformDescriptorSet->getComponent<VulkanDescriptorSet>(), 1);

        sceneGraph = new EntityNode(&graphicsObjects, "sceneGraph"); 
            sceneGraph->addComponent(new TransformRoot());
            sceneGraph->addComponent(new Transform(glm::rotate(glm::scale(glm::mat4(1.0f), glm::vec3(0.25f)), glm::radians(-45.0f), glm::vec3(1.0f, 0.0f, 0.0f))));
            sceneGraph->addComponent(new Transform());
            bufferTransform.apply(sceneGraph);
            sceneGraph->addComponent(new RenderNode(quad));
            EntityNode* subTree = new EntityNode(sceneGraph, "subTree");
                subTree->addComponent(new Transform());
                subTree->addComponent(new Transform(glm::translate(glm::mat4(1.0f), glm::vec3(1.0f,0.0,0.0))));
                bufferTransform.apply(subTree);
                subTree->addComponent(new RenderNode(triangle));
                EntityNode* subTree2 = new EntityNode(subTree, "subTree2");
                    subTree2->addComponent(new Transform(glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f,0.0,-1.0))));
                    EntityNode* subTree3 = new EntityNode(subTree2, "subTree3");
                        //subTree3->addComponent(new VulkanCmdBindDescriptorSet(secondDescriptorSet->getComponent<VulkanDescriptorSet>(), 0));
                        bufferTransform.apply(subTree3);
                        subTree3->addComponent(new RenderNode(triangle)); 
                    EntityNode* cylindarNode = new EntityNode(subTree2, "cylindar");
                        cylindarNode->addComponent(new Transform(glm::translate(glm::mat4(1.0f), glm::vec3(0.0,1.0f,-0.1))));
                        bufferTransform.apply(cylindarNode);
                        cylindarNode->addComponent(new RenderNode(cylindar));

        scene.addComponent(new MouseInteraction(&input));
        scene.addComponent(new TouchTranslate(&input));
        scene.addComponent(new TouchScale(&input));
        EntityNode* drawObject = new EntityNode(&scene);
            drawObject->addComponent(new RenderNode(&renderPassNode, RENDER_ACTION_START));
            drawObject->addComponent(new RenderNode(&pipelineNode, RENDER_ACTION_START));
            drawObject->addComponent(new VulkanCmdBindDescriptorSet(mainDescriptorSet->getComponent<VulkanDescriptorSet>(), 0));
            drawObject->addComponent(new RenderNode(sceneGraph, RENDER_ACTION_RENDER, new IgnoreNode(subTree2)));
            drawObject->addComponent(new VulkanCmdBindDescriptorSet(secondDescriptorSet->getComponent<VulkanDescriptorSet>(), 0));
            drawObject->addComponent(new RenderNode(sceneGraph, RENDER_ACTION_RENDER, new RenderAt(subTree3)));
            //drawObject->addComponent(new RenderNode(sceneGraph));
            drawObject->addComponent(new RenderNode(&pipelineNode, RENDER_ACTION_END));
            drawObject->addComponent(new RenderNode(&pipelineNode2, RENDER_ACTION_START));
            drawObject->addComponent(new VulkanCmdBindDescriptorSet(mainDescriptorSet2->getComponent<VulkanDescriptorSet>(), 0));
            //drawObject->addComponent(new Transform(glm::translate(glm::mat4(1.0f), glm::vec3(0.25f,0.0,0.1))));
            //drawObject->addComponent(new RenderNode(sceneGraph));
            drawObject->addComponent(new RenderNode(sceneGraph, RENDER_ACTION_RENDER, new IgnoreNode(cylindarNode)));
            drawObject->addComponent(new RenderNode(&pipelineNode2, RENDER_ACTION_END));

            drawObject->addComponent(new RenderNode(&pipelineNode3, RENDER_ACTION_START));
            drawObject->addComponent(new VulkanCmdBindDescriptorSet(mainDescriptorSet->getComponent<VulkanDescriptorSet>(), 0));
            drawObject->addComponent(new RenderNode(sceneGraph, RENDER_ACTION_RENDER, new RenderAt(cylindarNode)));
            drawObject->addComponent(new RenderNode(&pipelineNode3, RENDER_ACTION_END));

            drawObject->addComponent(new RenderNode(&renderPassNode, RENDER_ACTION_END));
        /*EntityNode* drawObject2 = new EntityNode(&scene);
            drawObject2->addComponent(new Transform(glm::translate(glm::mat4(1.0f), glm::vec3(0.7f,0.0,0.0))));
            drawObject2->addComponent(new RenderNode(&pipelineNode2, RENDER_ACTION_START));
            drawObject2->addComponent(new VulkanCmdBindDescriptorSet(mainDescriptorSet->getComponent<VulkanDescriptorSet>(), 0));
            drawObject2->addComponent(new RenderNode(sceneGraph));
            drawObject2->addComponent(new RenderNode(&pipelineNode2, RENDER_ACTION_END));
        /*EntityNode* drawObject2 = new EntityNode(&scene);
            drawObject2->addComponent(new VulkanCmdBindDescriptorSet(mainDescriptorSet->getComponent<VulkanDescriptorSet>(), 0));
            drawObject2->addComponent(new RenderNode(sceneGraph));
            drawObject2->addComponent(new RenderNode(&pipelineNode2, RENDER_ACTION_END));*/

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
            updateSharedNode = new EntityNode(deviceNode);
                updateSharedNode->addComponent(new AllowRenderModes(VULKAN_RENDER_UPDATE_SHARED | VULKAN_RENDER_OBJECT | VULKAN_RENDER_CLEANUP_SHARED));
                updateSharedNode->addComponent(new VulkanDeviceRenderer());
                updateSharedNode->addComponent(new VulkanCommandPool(graphicsQueue));
                updateSharedNode->addComponent(new RenderNode(&graphicsObjects));
                updateSharedNode->addComponent(new RenderNode(&images));
                EntityNode* renderSpecific = new EntityNode(updateSharedNode);
                    renderSpecific->addComponent(new AllowRenderModes(VULKAN_RENDER_SHARED_ONLY));
                    renderSpecific->addComponent(new RenderNode(&shaderObjects));//, UPDATE_ONLY));
                    renderSpecific->addComponent(new RenderNode(&descriptorSetGroup));
                    renderSpecific->addComponent(new RenderNode(&renderPassNode, RENDER_ACTION_START));
                    renderSpecific->addComponent(new RenderNode(&pipelineNode));//, UPDATE_ONLY));
                    renderSpecific->addComponent(new RenderNode(&pipelineNode2));//, UPDATE_ONLY));
                    renderSpecific->addComponent(new RenderNode(&pipelineNode3));//, UPDATE_ONLY));
                    renderSpecific->addComponent(new RenderNode(&renderPassNode, RENDER_ACTION_END));
                    //renderSpecific->addComponent(new RenderNode(&pipelineNode2));//, UPDATE_ONLY));
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

        std::cout << deviceNode->getComponent<VulkanDevice>()->getProperties().limits.maxUniformBufferRange << std::endl;
        std::cout << deviceNode->getComponent<VulkanDevice>()->getProperties().limits.maxVertexInputBindings << std::endl;
        /*size_t minUboAlignment = deviceNode->getComponent<VulkanDevice>()->getProperties().limits.minUniformBufferOffsetAlignment;
        std::cout << "minUboAlignment " << minUboAlignment << std::endl;
        size_t dynamicAlignment = sizeof(TransformUniformBufferObject);
        if (minUboAlignment > 0) {
            dynamicAlignment = (dynamicAlignment + minUboAlignment - 1) & ~(minUboAlignment - 1);
        }
        std::cout << "dynamicAlignment " << dynamicAlignment << std::endl;*/

        input.addComponent(new GLFWInput(window));
        input.addComponent(new GLFWInput(window2)); 
        input.addComponent(new GLFWInput(window3));
        input.addComponent(new TUIOTouchInput());
        //input.addComponent(new GLFWMouseInput(window));
        input.update();
    }


    EntityNode* createSwapChain(Entity* parentNode, Entity* surfaceNode, std::string name) {
        EntityNode* renderNode = new EntityNode(parentNode);
            renderNode->addComponent(new VulkanBasicSwapChain(name, surfaceNode));
            renderNode->addComponent(new VulkanFrameRenderer(graphicsQueue, presentQueue));
            EntityNode* updateNode = new EntityNode(renderNode);
                updateNode->addComponent(new AllowRenderModes(VULKAN_RENDER_UPDATE_DISPLAY | VULKAN_RENDER_UPDATE | VULKAN_RENDER_OBJECT | VULKAN_RENDER_CLEANUP_DISPLAY | VULKAN_RENDER_CLEANUP));
                updateNode->addComponent(new RenderNode(&shaderObjects));
                updateNode->addComponent(new RenderNode(&descriptorSetGroup));
                updateNode->addComponent(new RenderNode(&renderPassNode, RENDER_ACTION_START));
                updateNode->addComponent(new RenderNode(&pipelineNode));//, UPDATE_ONLY));
                updateNode->addComponent(new RenderNode(&pipelineNode2));//, UPDATE_ONLY));
                updateNode->addComponent(new RenderNode(&pipelineNode3));//, UPDATE_ONLY));
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
            renderNode0->getComponent<VulkanFrameRenderer>()->drawFrame();
            renderNode2->getComponent<VulkanFrameRenderer>()->drawFrame();
            renderNode3->getComponent<VulkanFrameRenderer>()->drawFrame();

            if (frame == 10000) {
                std::cout << "test" << std::endl;
                VertexArray<Vertex>* va = static_cast< VertexArray<Vertex>* >(graphicsObjects.getChildren()[0]->getComponent< VulkanDeviceBuffer >());
                va->value[0].pos.x = 0.0;
                graphicsObjects.incrementVersion();
                VulkanDeviceRenderer* sharedRenderer = updateSharedNode->getComponentRecursive<VulkanDeviceRenderer>();
                //sharedRenderer->render(VULKAN_RENDER_UPDATE_SHARED);
                sharedRenderer->render(VULKAN_RENDER_OBJECT);
            }

            if (frame>=0) {   
                static auto startTime = std::chrono::high_resolution_clock::now();
                auto currentTime = std::chrono::high_resolution_clock::now();
                float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

                sceneGraph->getComponents<Transform>()[1]->setTransform(glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
                //sceneGraph->getChildren()[0]->getComponent<Transform>()->setTransform(glm::scale(glm::mat4(1.0f), glm::vec3(2.0f*std::sin(time))));

                static GraphicsContext updateContext;
                TransformState::get(updateContext).calculateTransform = true;
                static RenderNode sceneUpdate(&scene);
                sceneUpdate.render(updateContext);
            }



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

