#include "VulkanAppBase.h"

using namespace sandbox;

class StereoUniformBuffer : public VulkanUniformBufferValue<ViewBufferObject> {
protected:
    void updateBuffer(const GraphicsContext& context, VulkanDeviceState& state, VulkanBuffer* buffer) {
        ViewBufferObject ubo = {};
        ubo.view = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        ubo.proj = glm::perspective(glm::radians(45.0f), (float) state.getExtent().width / (float) state.getExtent().height, 0.01f, 100.0f);
        ubo.model = glm::mat4(1.0f);
        /*if (VulkanSwapChainState::get(context).getSwapChain()->getName() == "window 2") {
            ubo.model = glm::translate(ubo.model, glm::vec3(0.25,0,0.0));
        }*/
        if (VulkanSwapChainState::get(context).getSwapChain()->getName() == "Vulkan2") {
            ubo.model = glm::translate(ubo.model, glm::vec3(0.25,0,0.0));
        }
        ubo.proj[1][1] *= -1;

        buffer->update(&ubo, sizeof(ubo));
    }
};

class PlanetApp : public VulkanAppBase {
    void createWindows() {
        createWindow(0, 0, WIDTH, HEIGHT, "Vulkan");
        createWindow(WIDTH, 0, WIDTH, HEIGHT, "Vulkan2");
    }

    class Planet {
    public:
        Planet(PlanetAppInfo& appInfo, const std::string& texture, float yearSpeed = 0.0f, float daySpeed = 0.0f) : appInfo(appInfo), yearSpeed(yearSpeed), daySpeed(daySpeed) {
            image = new EntityNode(&appInfo.images);
                image->addComponent(new Image(texture));
                image->addComponent(new VulkanImage());
                image->addComponent(new VulkanImageView());
            image->update();
            shape = appInfo.sphere;
            materialDescriptorSet = new EntityNode(&appInfo.descriptorSetGroup);
                materialDescriptorSet->addComponent(new VulkanDescriptorSetLayout()); 
                materialDescriptorSet->addComponent(new VulkanSwapChainDescriptorPool());
                materialDescriptorSet->addComponent(new VulkanDescriptorSet());
                materialDescriptorSet->addComponent(new VulkanDescriptor(new VulkanImageViewDecorator(appInfo.shaderObjects.getComponentRecursive<VulkanSampler>(), image->getComponent<VulkanImageView>()), VK_SHADER_STAGE_FRAGMENT_BIT));
        }

        EntityNode* addAt(EntityNode* entity, glm::mat4 transform = glm::mat4(1.0f)) {
            TransformUniformBuffer* transformUniformBuffer = appInfo.shaderObjects.getComponentRecursive<TransformUniformBuffer>();
            planetNode = new EntityNode(entity);
                planetNode->addComponent(new Transform());
                planetNode->addComponent(new Transform(transform));
                rotationNode = new EntityNode(planetNode);
                    rotationNode->addComponent(new Transform());
                    rotationNode->addComponent(new VulkanCmdBindDynamicDescriptorSet2(appInfo.transformDescriptorSet->getComponent<VulkanDescriptorSet>(), transformUniformBuffer, 1)); 
                    rotationNode->addComponent(new UpdateTransform(transformUniformBuffer)); 
                    rotationNode->addComponent(new RenderNode(appInfo.sphere));
            return planetNode;
        }

        void render(EntityNode* drawObject, EntityNode* sceneGraph) {
            drawObject->addComponent(new VulkanCmdBindDescriptorSet(materialDescriptorSet->getComponent<VulkanDescriptorSet>(), 2));
            drawObject->addComponent(new RenderNode(sceneGraph, RENDER_ACTION_RENDER, new RenderAt(rotationNode)));
        }

        void update() {
            static auto startTime = std::chrono::high_resolution_clock::now();
            auto currentTime = std::chrono::high_resolution_clock::now();
            float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
            planetNode->getComponent<Transform>()->setTransform(glm::rotate(glm::mat4(1.0f), yearSpeed*time, glm::vec3(0.0f,1.0f,0.0f)));
            rotationNode->getComponent<Transform>()->setTransform(glm::rotate(glm::mat4(1.0f), daySpeed*time, glm::vec3(0.0f,1.0f,0.0f)));
        }

    private:
        EntityNode* image;
        EntityNode* materialDescriptorSet;
        EntityNode* shape;
        EntityNode* planetNode;
        EntityNode* rotationNode;
        PlanetAppInfo& appInfo;
        float yearSpeed;
        float daySpeed;
    };

    void initScene() {
        planets.push_back(new Planet(appInfo, "examples/Planets/textures/2k_sun.jpg"));
        planets.push_back(new Planet(appInfo, "examples/Planets/textures/earth-1k.png", 1.0f/365.0f, 1.0f));
        planets.push_back(new Planet(appInfo, "examples/Planets/textures/moon-1k.jpg", 1.0f/30.0f, 0.0f)); 

        Entity* materialDescriptorSet = appInfo.descriptorSetGroup.getChildren()[2];

        EntityNode* stereoUniformBuffer = new EntityNode(&appInfo.shaderObjects);
            stereoUniformBuffer->addComponent(new StereoUniformBuffer());

        appInfo.viewDescriptorSet = new EntityNode(&appInfo.descriptorSetGroup);
            appInfo.viewDescriptorSet->addComponent(new VulkanDescriptorSetLayout()); 
            appInfo.viewDescriptorSet->addComponent(new VulkanSwapChainDescriptorPool());
            appInfo.viewDescriptorSet->addComponent(new VulkanDescriptorSet());
            appInfo.viewDescriptorSet->addComponent(new VulkanDescriptor(stereoUniformBuffer->getComponent<VulkanUniformBuffer>(), VK_SHADER_STAGE_VERTEX_BIT));

        appInfo.sphere = new EntityNode(&graphicsObjects);
            appInfo.sphere->addComponent(new Mesh());
            appInfo.sphere->addComponent(new ShapeLoader(SHAPE_SPHERE, 50));
            appInfo.sphere->addComponent(new VulkanMeshRenderer());
            appInfo.sphere->update();

        pipelineNode.addComponent(new VulkanShaderModule("examples/Planets/src/shaders/vert2.spv", VK_SHADER_STAGE_VERTEX_BIT));
        pipelineNode.addComponent(new VulkanShaderModule("examples/Planets/src/shaders/frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT));
        std::vector<VulkanDescriptorSetLayout*> layouts;
        layouts.push_back(appInfo.viewDescriptorSet->getComponent<VulkanDescriptorSetLayout>());
        layouts.push_back(appInfo.transformDescriptorSet->getComponent<VulkanDescriptorSetLayout>());
        layouts.push_back(materialDescriptorSet->getComponent<VulkanDescriptorSetLayout>());
        pipelineNode.addComponent(new VulkanGraphicsPipeline(layouts));
        VulkanMeshRenderer::addVertexInput(&pipelineNode);

        renderPassNode.addComponent(new VulkanBasicRenderPass());
        renderPassNode.addComponent(new VulkanSwapChainFramebuffer());

        sceneGraph = new EntityNode(&graphicsObjects, "SceneGraph"); 
            sceneGraph->addComponent(new TransformRoot());
            planets[0]->addAt(sceneGraph);
            EntityNode* planetNodes = new EntityNode(sceneGraph, "Planets");
                glm::mat4 trans = glm::translate(glm::mat4(1.0f), glm::vec3(2.0, 0, 0));
                EntityNode* earth = planets[1]->addAt(planetNodes, glm::scale(trans, glm::vec3(0.1)));
                    trans = glm::translate(glm::mat4(1.0f), glm::vec3(2.0, 0, 0));
                    planets[2]->addAt(earth, glm::scale(trans, glm::vec3(0.1)));

        scene.addComponent(new MouseInteraction(&input));
        scene.addComponent(new TouchTranslate(&input));
        scene.addComponent(new TouchScale(&input));
        EntityNode* drawObject = new EntityNode(&scene);
            drawObject->addComponent(new RenderNode(&renderPassNode, RENDER_ACTION_START));
            drawObject->addComponent(new RenderNode(&pipelineNode, RENDER_ACTION_START));
            drawObject->addComponent(new VulkanCmdBindDescriptorSet(appInfo.viewDescriptorSet->getComponent<VulkanDescriptorSet>(), 0));
            for (int f = 0; f < planets.size(); f++) {
                planets[f]->render(drawObject,sceneGraph);
            }
            drawObject->addComponent(new RenderNode(&pipelineNode, RENDER_ACTION_END));
            drawObject->addComponent(new RenderNode(&renderPassNode, RENDER_ACTION_END));


    }

    void update() {
        for (int f = 0; f < planets.size(); f++) {
            planets[f]->update();
        }
    }

    std::vector<Planet*> planets;
};

int main() {
    PlanetApp app;

    try {
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

