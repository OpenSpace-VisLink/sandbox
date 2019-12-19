#include "sandbox/app/VulkanAppBase.h"
#include "sandbox/graphics/vulkan/render/VulkanDrawInstanced.h"

using namespace sandbox;

struct InstanceInfo {
    alignas(4) glm::vec3 location;
    alignas(4) glm::vec3 info;
    float armAngles[16];
    float armLengths[16];
};

class CellApp : public VulkanAppBase {
    void createWindows() {
        createWindow(0, 0, 1024, 768, "Vulkan");
        //createWindow(WIDTH, 0, WIDTH, HEIGHT, "Vulkan2");
    }

    void createInstanceInput(EntityNode* entity) {
        VulkanVertexInput* vertexInput = new VulkanVertexInput(sizeof(InstanceInfo), 3, VK_VERTEX_INPUT_RATE_INSTANCE);
        vertexInput->addAttribute(VK_FORMAT_R32G32B32_SFLOAT, offsetof(InstanceInfo, location));
        vertexInput->addAttribute(VK_FORMAT_R32G32B32_SFLOAT, offsetof(InstanceInfo, info));
        vertexInput->addAttribute(VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(InstanceInfo, armAngles));
        vertexInput->addAttribute(VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(InstanceInfo, armAngles)+sizeof(glm::vec4));
        vertexInput->addAttribute(VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(InstanceInfo, armAngles)+2*sizeof(glm::vec4));
        vertexInput->addAttribute(VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(InstanceInfo, armAngles)+3*sizeof(glm::vec4));
        vertexInput->addAttribute(VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(InstanceInfo, armLengths));
        vertexInput->addAttribute(VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(InstanceInfo, armLengths)+sizeof(glm::vec4));
        vertexInput->addAttribute(VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(InstanceInfo, armLengths)+2*sizeof(glm::vec4));
        vertexInput->addAttribute(VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(InstanceInfo, armLengths)+3*sizeof(glm::vec4));
        entity->addComponent(vertexInput);
    }

    void initScene() {

        appInfo.sphere = new EntityNode(&graphicsObjects);
            appInfo.sphere->addComponent(new Mesh());
            //appInfo.sphere->addComponent(new ShapeLoader(SHAPE_SPHERE, 100));
            appInfo.sphere->addComponent(new ShapeLoader(SHAPE_CIRCLE, 50));
            appInfo.sphere->addComponent(new VulkanMeshRenderer());
            appInfo.sphere->update();

        EntityNode* instances = new EntityNode(&graphicsObjects);
            VertexArray<InstanceInfo>* cellArray = new VertexArray<InstanceInfo>(3);
            InstanceInfo cell;
            cell.location = glm::vec3(3.0,0,0);
            cell.info.x = 1;
            cell.armAngles[0] = 0.0f;
            cell.armLengths[0] = 1.0f;
            cellArray->value.push_back(cell);
            cell.location = glm::vec3(0.0,0,0);
            cell.info.x = 10;
            cell.armAngles[0] = 0.1*2.0*3.1415;
            cell.armAngles[1] = 0.2*2.0*3.1415;
            cell.armAngles[2] = 0.4*2.0*3.1415;
            cell.armAngles[3] = 0.5*2.0*3.1415;
            cell.armAngles[4] = 0.51*2.0*3.1415;
            cell.armAngles[5] = 0.6*2.0*3.1415;
            cell.armAngles[6] = 0.65*2.0*3.1415;
            cell.armAngles[7] = 0.67*2.0*3.1415;
            cell.armAngles[8] = 0.7*2.0*3.1415;
            cell.armAngles[9] = 0.9*2.0*3.1415;
            cell.armLengths[0] = 1.0f;
            cell.armLengths[1] = 0.4f;
            cell.armLengths[2] = 1.0f;
            cell.armLengths[3] = 1.3f;
            cell.armLengths[4] = 1.0f;
            cell.armLengths[5] = 2.0f;
            cell.armLengths[6] = 1.0f;
            cell.armLengths[7] = 1.7f;
            cell.armLengths[8] = 1.4f;
            cell.armLengths[9] = 1.0f;
            cellArray->value.push_back(cell);
            cell.location = glm::vec3(0.0,0,0);
            cell.info.x = 3;
            cell.armAngles[0] = 0.0f;
            cell.armAngles[1] = 2.0*3.1415/3;
            cell.armAngles[2] = 2.0*2.0*3.1415/3;
            cell.armLengths[0] = 1.0f;
            cell.armLengths[1] = 0.5f;
            cell.armLengths[2] = 0.75f;
            cellArray->value.push_back(cell);
            cell.location = glm::vec3(0.0,1.0,0);
            cell.info.x = 5;
            cell.armAngles[0] = 0.1*2.0*3.1415/5;
            cell.armAngles[1] = 0.5*2.0*3.1415/5;
            cell.armAngles[2] = 1.0*2.0*3.1415/5;
            cell.armAngles[3] = 2.2*2.0*3.1415/5;
            cell.armAngles[4] = 4.0*2.0*3.1415/5;
            cell.armLengths[0] = 1.0f;
            cell.armLengths[1] = 0.0f;
            cell.armLengths[2] = 1.0f;
            cell.armLengths[3] = 0.3f;
            cell.armLengths[4] = 1.0f;
            cellArray->value.push_back(cell);
            instances->addComponent(cellArray);
            instances->addComponent(new VulkanDrawInstanced());
        instances->update();

        EntityNode* image = new EntityNode(&appInfo.images);
            image->addComponent(new Image("examples/Cells/textures/cell_texture.png"));
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
            createInstanceInput(pipelineBack);
        }
        EntityNode* pipelineFront = new EntityNode(&basicPipelines); {
            pipelineFront->addComponent(new VulkanShaderModule("examples/Cells/src/shaders/vert.spv", VK_SHADER_STAGE_VERTEX_BIT));
            pipelineFront->addComponent(new VulkanShaderModule("examples/Cells/src/shaders/frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT));
            VulkanGraphicsPipeline* pipeline = new VulkanGraphicsPipeline(layouts);
            //pipeline->cullMode = VK_CULL_MODE_FRONT_BIT;
            //pipeline->depthTestEnable = VK_FALSE;
            pipeline->blendEnable = VK_TRUE;
            pipelineFront->addComponent(pipeline);
            VulkanMeshRenderer::addVertexInput(pipelineFront);
            /*VulkanVertexInput* vertexInput = new VulkanVertexInput(sizeof(InstanceInfo), 3, VK_VERTEX_INPUT_RATE_INSTANCE);
            vertexInput->addAttribute(VK_FORMAT_R32G32B32_SFLOAT, offsetof(InstanceInfo, location));
            pipelineFront->addComponent(vertexInput);*/
            createInstanceInput(pipelineFront);
        }

        TransformUniformBuffer* transformUniformBuffer = appInfo.shaderObjects.getComponentRecursive<TransformUniformBuffer>();

        sceneGraph = new EntityNode(&graphicsObjects, "SceneGraph"); 
            sceneGraph->addComponent(new TransformRoot());
            sceneGraph->addComponent(new Transform(glm::scale(glm::mat4(1.0f), glm::vec3(1.0f))));
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

