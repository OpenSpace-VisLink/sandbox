#include "sandbox/app/VulkanAppBase.h"
#include "sandbox/graphics/vulkan/render/VulkanDrawInstanced.h"

using namespace sandbox;

class CellApp : public VulkanAppBase {
    void createWindows() {
        createWindow(0, 0, 1024, 768, "Vulkan");
        //createWindow(WIDTH, 0, WIDTH, HEIGHT, "Vulkan2");
    }

    void initScene() {

        appInfo.sphere = new EntityNode(&graphicsObjects);
            appInfo.sphere->addComponent(new Mesh());
            appInfo.sphere->addComponent(new ShapeLoader(SHAPE_SPHERE, 50));
            appInfo.sphere->addComponent(new VulkanMeshRenderer());
            appInfo.sphere->update();

        EntityNode* instances = new EntityNode(&graphicsObjects);
            VertexArray<glm::vec3>* locArray = new VertexArray<glm::vec3>(3);
            locArray->value.push_back(glm::vec3(3.0,0,0));
            locArray->value.push_back(glm::vec3(-1.0,0,0));
            locArray->value.push_back(glm::vec3(0.0,0,0));
            locArray->value.push_back(glm::vec3(0.0,1.0,0));
            instances->addComponent(locArray);
            instances->addComponent(new VulkanDrawInstanced());
        instances->update();

        EntityNode* image = new EntityNode(&appInfo.images);
            image->addComponent(new Image("examples/Planets/textures/2k_sun.jpg"));
            image->addComponent(new VulkanImage());
            image->addComponent(new VulkanImageView());
        image->update();

        EntityNode* materialDescriptorSet = new EntityNode(&appInfo.descriptorSetGroup);
            materialDescriptorSet->addComponent(new VulkanDescriptorSetLayout()); 
            materialDescriptorSet->addComponent(new VulkanSwapChainDescriptorPool());
            materialDescriptorSet->addComponent(new VulkanDescriptorSet());
            materialDescriptorSet->addComponent(new VulkanDescriptor(new VulkanImageViewDecorator(appInfo.shaderObjects.getComponentRecursive<VulkanSampler>(), image->getComponent<VulkanImageView>()), VK_SHADER_STAGE_FRAGMENT_BIT));

        std::vector<VulkanDescriptorSetLayout*> layouts;
            layouts.push_back(appInfo.viewDescriptorSet->getComponent<VulkanDescriptorSetLayout>());
            layouts.push_back(appInfo.transformDescriptorSet->getComponent<VulkanDescriptorSetLayout>());
            layouts.push_back(materialDescriptorSet->getComponent<VulkanDescriptorSetLayout>());

        basicRenderPass.addComponent(new VulkanBasicRenderPass());
        basicRenderPass.addComponent(new VulkanSwapChainFramebuffer());

        EntityNode* pipelineBack = new EntityNode(&basicPipelines); {
            pipelineBack->addComponent(new VulkanShaderModule("examples/Cells/src/shaders/vert.spv", VK_SHADER_STAGE_VERTEX_BIT));
            pipelineBack->addComponent(new VulkanShaderModule("examples/Cells/src/shaders/frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT));
            VulkanGraphicsPipeline* pipeline = new VulkanGraphicsPipeline(layouts);
            pipeline->cullMode = VK_CULL_MODE_FRONT_BIT;
            pipelineBack->addComponent(pipeline);
            VulkanMeshRenderer::addVertexInput(pipelineBack);
            VulkanVertexInput* vertexInput = new VulkanVertexInput(sizeof(glm::vec3), 3, VK_VERTEX_INPUT_RATE_INSTANCE);
            vertexInput->addAttribute(VK_FORMAT_R32G32B32_SFLOAT, 0);
            pipelineBack->addComponent(vertexInput);
        }
        EntityNode* pipelineFront = new EntityNode(&basicPipelines); {
            pipelineFront->addComponent(new VulkanShaderModule("examples/Cells/src/shaders/vert.spv", VK_SHADER_STAGE_VERTEX_BIT));
            pipelineFront->addComponent(new VulkanShaderModule("examples/Cells/src/shaders/frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT));
            VulkanGraphicsPipeline* pipeline = new VulkanGraphicsPipeline(layouts);
            //pipeline->cullMode = VK_CULL_MODE_FRONT_BIT;
            pipeline->depthTestEnable = VK_FALSE;
            pipeline->blendEnable = VK_TRUE;
            pipelineFront->addComponent(pipeline);
            VulkanMeshRenderer::addVertexInput(pipelineFront);
            VulkanVertexInput* vertexInput = new VulkanVertexInput(sizeof(glm::vec3), 3, VK_VERTEX_INPUT_RATE_INSTANCE);
            vertexInput->addAttribute(VK_FORMAT_R32G32B32_SFLOAT, 0);
            pipelineFront->addComponent(vertexInput);
        }

        TransformUniformBuffer* transformUniformBuffer = appInfo.shaderObjects.getComponentRecursive<TransformUniformBuffer>();

        sceneGraph = new EntityNode(&graphicsObjects, "SceneGraph"); 
            sceneGraph->addComponent(new TransformRoot());
            sceneGraph->addComponent(new Transform(glm::scale(glm::mat4(1.0f), glm::vec3(0.5f))));
            sceneGraph->addComponent(new VulkanCmdBindDynamicDescriptorSet2(appInfo.transformDescriptorSet->getComponent<VulkanDescriptorSet>(), transformUniformBuffer, 1)); 
            sceneGraph->addComponent(new UpdateTransform(transformUniformBuffer)); 
            sceneGraph->addComponent(new RenderNode(appInfo.sphere));

        scene.addComponent(new MouseInteraction(&input));
        scene.addComponent(new TouchTranslate(&input));
        scene.addComponent(new TouchScale(&input));
        EntityNode* drawObject = new EntityNode(&scene);
            drawObject->addComponent(new RenderNode(&basicRenderPass, RENDER_ACTION_START));

            drawObject->addComponent(new RenderNode(pipelineBack, RENDER_ACTION_START));
            drawObject->addComponent(new VulkanCmdBindDescriptorSet(appInfo.viewDescriptorSet->getComponent<VulkanDescriptorSet>(), 0));
            drawObject->addComponent(new VulkanCmdBindDescriptorSet(materialDescriptorSet->getComponent<VulkanDescriptorSet>(), 2));
            drawObject->addComponent(new RenderNode(instances, RENDER_ACTION_START));
            drawObject->addComponent(new RenderNode(sceneGraph));
            drawObject->addComponent(new RenderNode(instances, RENDER_ACTION_END));
            drawObject->addComponent(new RenderNode(pipelineBack, RENDER_ACTION_END));

            drawObject->addComponent(new RenderNode(pipelineFront, RENDER_ACTION_START));
            drawObject->addComponent(new VulkanCmdBindDescriptorSet(appInfo.viewDescriptorSet->getComponent<VulkanDescriptorSet>(), 0));
            drawObject->addComponent(new VulkanCmdBindDescriptorSet(materialDescriptorSet->getComponent<VulkanDescriptorSet>(), 2));
            drawObject->addComponent(new RenderNode(instances, RENDER_ACTION_START));
            drawObject->addComponent(new RenderNode(sceneGraph));
            drawObject->addComponent(new RenderNode(instances, RENDER_ACTION_END));
            drawObject->addComponent(new RenderNode(pipelineFront, RENDER_ACTION_END));

            drawObject->addComponent(new RenderNode(&basicRenderPass, RENDER_ACTION_END));

    }

    void initPipelines(EntityNode* renderNode) {
        renderNode->addComponent(new RenderNode(&basicRenderPass, RENDER_ACTION_START));
        renderNode->addComponent(new RenderNode(&basicPipelines));
        renderNode->addComponent(new RenderNode(&basicRenderPass, RENDER_ACTION_END));
    }

    void update() {
    }

    EntityNode basicRenderPass;
    EntityNode basicPipelines;
};

int main() {
    CellApp app;

    try {
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

