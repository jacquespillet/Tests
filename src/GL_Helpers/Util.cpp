#include "Util.hpp"

#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags
#include <assimp/material.h>

GL_Mesh MeshFromFile(std::string filename) {
    std::vector<unsigned int> triangles;
    std::vector<GL_Mesh::Vertex> vertices;   

    Assimp::Importer import;
    const aiScene *scene = import.ReadFile(filename, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_FlipWindingOrder | aiProcess_CalcTangentSpace ); 

    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) 
    {
        std::cout << "ModelLoader:LoaderModel: ERROR::" << import.GetErrorString() << std::endl;
        // return;
    }
    std::cout << "Num materials " << scene->mNumMaterials << std::endl;
    std::cout << "Num Meshes " << scene->mNumMeshes << std::endl;
    std::cout << "Num Textures " << scene->mNumTextures << std::endl;
    for(unsigned int j = 0; j < scene->mNumMeshes; j++) {

        aiMesh *mesh = scene->mMeshes[j]; 
        std::cout << "NUM VERTICES " << mesh->mNumVertices << std::endl;
        for(unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            glm::vec3 pos(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
            // pos *= 0.001; 
            glm::vec3 normal(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
            glm::vec2 uv(0);
            if(mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
            {
                uv.x = mesh->mTextureCoords[0][i].x; 
                uv.y = mesh->mTextureCoords[0][i].y;
            }
            vertices.push_back({
                pos,
                normal,
                uv,
                glm::vec4(1)                
            });
        }

        for(unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            for(unsigned int k = 0; k < face.mNumIndices; k++)
                triangles.push_back(face.mIndices[k]);
        }
    }
    GL_Mesh gl_mesh(vertices, triangles);
    // processNode(scene->mRootNode, scene, vertex, normals, uv, colors, triangles);
    
    return gl_mesh;
}
