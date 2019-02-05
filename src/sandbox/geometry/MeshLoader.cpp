#include "sandbox/geometry/MeshLoader.h"
#include <iostream>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace sandbox {

MeshLoader::MeshLoader(const std::string& fileName) : fileName(fileName), isLoaded(false) {
	addType<MeshLoader>();
}

void MeshLoader::updateModel() {
	if (!isLoaded) {
		Mesh* mesh = getSceneNode().getComponent<Mesh>();
		if (mesh) {
			load(fileName, mesh);
			isLoaded = true;
		}
	}
}

void MeshLoader::load(const std::string& fileName, Mesh* mesh) {
	Assimp::Importer importer;
	unsigned int flags = aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals;
	const aiScene *scene = importer.ReadFile(fileName, flags);

	if(!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
		exit(0);
	}

	/*for (int f = 0; f < scene->mNumMeshes; f++) {
		for (int i = 0; i < scene->mMeshes[f]->mNumVertices; i++) {
			mesh.nodes.push_back(glm::vec3(scene->mMeshes[f]->mVertices[i].x, scene->mMeshes[f]->mVertices[i].y, scene->mMeshes[f]->mVertices[i].z));
		}

		if (scene->mMeshes[f]->HasNormals()) {
			for (int i = 0; i < scene->mMeshes[f]->mNumVertices; i++) {
				mesh.normals.push_back(glm::vec3(scene->mMeshes[f]->mNormals[i].x, scene->mMeshes[f]->mNormals[i].y, scene->mMeshes[f]->mNormals[i].z));
			}
		}

		if (scene->mMeshes[f]->HasFaces()) {
			for (int i = 0; i < scene->mMeshes[f]->mNumFaces; i++) {
				mesh.indices.push_back(scene->mMeshes[f]->mFaces[i].mIndices[0]);
				mesh.indices.push_back(scene->mMeshes[f]->mFaces[i].mIndices[1]);
				mesh.indices.push_back(scene->mMeshes[f]->mFaces[i].mIndices[2]);
			}
		}
	}

	mesh.min = mesh.nodes[0];
	mesh.max = mesh.min;
	for (int f = 1; f < mesh.nodes.size(); f++) {
		mesh.min = glm::min(mesh.min, mesh.nodes[f]);
		mesh.max = glm::max(mesh.max, mesh.nodes[f]);
	}

	glm::vec3 diag = mesh.max-mesh.min;
	mesh.scale = 2.0f/glm::max(diag.x, glm::max(diag.y, diag.z));*/
}

}