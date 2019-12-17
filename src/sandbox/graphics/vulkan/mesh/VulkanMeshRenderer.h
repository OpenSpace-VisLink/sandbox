#ifndef SANDBOX_GRAPHICS_VULKAN_MESH_MESH_RENDERER_H_
#define SANDBOX_GRAPHICS_VULKAN_MESH_MESH_RENDERER_H_

#include "sandbox/graphics/vulkan/buffer/VulkanBufferTypes.h"
#include "sandbox/graphics/vulkan/buffer/VulkanVertexInput.h"

#include <sandbox/geometry/Mesh.h>

namespace sandbox {


class VulkanMeshRenderer : public Component {
public:
	VulkanMeshRenderer() : mesh(NULL) { addType<VulkanMeshRenderer>(); }
	virtual ~VulkanMeshRenderer() {}

	void afterAdd() {
		nodes = new VertexArray<glm::vec3>(0);
		normals = new VertexArray<glm::vec3>(1);
		coords = new VertexArray<glm::vec2>(2);
		indices =  new IndexArray();
		getEntity().addComponent(nodes);
		getEntity().addComponent(normals);
		getEntity().addComponent(coords);
		getEntity().addComponent(indices);
	}

	void update() {
		if (!mesh) {
			mesh = getEntity().getComponent<Mesh>();
		}

		if (mesh) {
            nodes->value = mesh->getNodes();
            normals->value = mesh->getNormals();
            coords->value = mesh->getCoords();
            indices->value = mesh->getIndices();
		}
	}

	static void addVertexInput(Entity* entity) {
		VulkanVertexInput* vertexInput = new VulkanVertexInput(sizeof(glm::vec3), 0);
        vertexInput->addAttribute(VK_FORMAT_R32G32B32_SFLOAT, 0);
        entity->addComponent(vertexInput);
        vertexInput = new VulkanVertexInput(sizeof(glm::vec3), 1);
        vertexInput->addAttribute(VK_FORMAT_R32G32B32_SFLOAT, 0);
        entity->addComponent(vertexInput);
        vertexInput = new VulkanVertexInput(sizeof(glm::vec2), 2);
        vertexInput->addAttribute(VK_FORMAT_R32G32_SFLOAT, 0);
        entity->addComponent(vertexInput);
    }

protected:
	sandbox::Mesh* mesh;
	VertexArray<glm::vec3>* nodes;
	VertexArray<glm::vec3>* normals;
	VertexArray<glm::vec2>* coords;
	IndexArray* indices;
};

}

#endif