#include "sandbox/geometry/MeshLoader.h"
#include "sandbox/base/Object.h"

namespace sandbox {

MeshLoader::MeshLoader() : isLoaded(false) {
	addType<MeshLoader>();
}

void MeshLoader::update() {
	if (!isLoaded) {
		Mesh* mesh = &getEntity().getComponent< Object<Mesh> >()->get();
		if (mesh) {
			load(mesh);
			isLoaded = true;
		}
	}
}

/*void MeshLoader::load(const std::string& path, Mesh* mesh) {
	Assimp::Importer importer;
	unsigned int flags = aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals;
	const aiScene *scene = importer.ReadFile(path, flags);

	if(!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
		exit(0);
	}

	std::vector<glm::vec3> nodes;
	std::vector<glm::vec3> normals;
	std::vector<unsigned int> indices;

	for (int f = 0; f < scene->mNumMeshes; f++) {
		for (int i = 0; i < scene->mMeshes[f]->mNumVertices; i++) {
			nodes.push_back(glm::vec3(scene->mMeshes[f]->mVertices[i].x, scene->mMeshes[f]->mVertices[i].y, scene->mMeshes[f]->mVertices[i].z));
		}

		if (scene->mMeshes[f]->HasNormals()) {
			for (int i = 0; i < scene->mMeshes[f]->mNumVertices; i++) {
				normals.push_back(glm::vec3(scene->mMeshes[f]->mNormals[i].x, scene->mMeshes[f]->mNormals[i].y, scene->mMeshes[f]->mNormals[i].z));
			}
		}

		if (scene->mMeshes[f]->HasFaces()) {
			for (int i = 0; i < scene->mMeshes[f]->mNumFaces; i++) {
				indices.push_back(scene->mMeshes[f]->mFaces[i].mIndices[0]);
				indices.push_back(scene->mMeshes[f]->mFaces[i].mIndices[1]);
				indices.push_back(scene->mMeshes[f]->mFaces[i].mIndices[2]);
			}
		}
	}

	mesh->setIndices(indices);
	mesh->setNodes(nodes);
	mesh->setNormals(normals);

	/*mesh->min = nodes[0];
	mesh->max = mesh->min;
	for (int f = 1; f < nodes.size(); f++) {
		mesh->min = glm::min(mesh->min, nodes[f]);
		mesh->max = glm::max(mesh->max, nodes[f]);
	}

	glm::vec3 diag = mesh->max-mesh->min;
	mesh->scale = 2.0f/glm::max(diag.x, glm::max(diag.y, diag.z));*/
//}

}