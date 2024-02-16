#pragma once

#include "CEntity.h"

class CModelLoader : public CEntity
{
private:
    // ������/�����/�����̸�
    string filePath; // �����
    string basePath; // ������ + �����
    std::vector<tMeshData> meshes;
    bool m_isGLTF = false; // gltf or fbx
    bool m_revertNormals = false;

public:
    void Load(string filePath, string fileName, bool revertNormals);

    void ProcessNode(aiNode* node, const aiScene* scene, DirectX::SimpleMath::Matrix tr);

    tMeshData ProcessMesh(aiMesh* mesh, const aiScene* scene);

    string ReadFilename(aiMaterial* material, aiTextureType type);

    void UpdateTangents();

    CLONE_DISABLE(CModelLoader);

private:
    CModelLoader();
    virtual ~CModelLoader();

    friend class CAssetMgr;
};
