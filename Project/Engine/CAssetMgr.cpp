#include "pch.h"
#include "CAssetMgr.h"
#include "CPathMgr.h"
#include "CTaskMgr.h"

#include "CMesh.h"
#include "CGraphicsShader.h"
#include "CModleLoader.h"

#include "CGameObject.h"
#include "CTransform.h"
#include "CMeshRender.h"

#include "CDevice.h"
#include "CSound.h"

CAssetMgr::CAssetMgr()
{
}

CAssetMgr::~CAssetMgr()
{
}

void CAssetMgr::init()
{
    CreateDefaultMesh();
    CreateDefaultGraphicsShader();
    CreateDefaultComputeShader();
    CreateDefaultMaterial();
    CreateDefaultPhysics2DMaterial();

    initSound();
}

void CAssetMgr::initSound()
{
    FMOD::System_Create(&CSound::g_pFMOD);

    if (nullptr == CSound::g_pFMOD)
    {
        assert(nullptr);
    }

    // 32�� ä�� ����
    CSound::g_pFMOD->init(32, FMOD_DEFAULT, nullptr);
}

void CAssetMgr::ReloadContent()
{
    LoadAssetsFromFile(CPathMgr::GetContentPath());

    // ���� ������ ������ ������ �޸𸮿��� ����
    for (UINT i = 0; i < (UINT)ASSET_TYPE::END; i++)
    {
        for (const auto& pair : m_mapAsset[i])
        {
            if (pair.second->IsEngineAsset())
                continue;

            wstring strFilePath = CPathMgr::GetContentPath() + pair.first;
            if (!std::filesystem::exists(strFilePath))
            {
                MessageBox(nullptr, L"���������� �����Ǿ����ϴ�.", L"Asset ��ũ", MB_OK);

                GamePlayStatic::DeleteAsset((ASSET_TYPE)i, pair.second);
            }
        }
    }
}

void CAssetMgr::LoadAssetsFromFile(std::filesystem::path _EntryPath)
{
    using namespace std::filesystem;

    for (auto& directoryEntry : directory_iterator(_EntryPath))
    {
        // ���丮 �ΰ�� ���� ���丮 ��ȸ
        if (directoryEntry.is_directory())
        {
            LoadAssetsFromFile(directoryEntry);
        }
        else
        {
            path FilePath = directoryEntry.path();
            path FileRelativePath = FilePath.lexically_relative(CPathMgr::GetContentPath());
            path FileExtension = FilePath.extension();

            if (FileExtension == L".mesh")
                Load<CMesh>(FileRelativePath, FileRelativePath);
            // if (FileExtension == L".mdat")
            //     Load<CMeshData>(FileRelativePath, FileRelativePath);
            if (FileExtension == L".pref")
                Load<CPrefab>(FileRelativePath, FileRelativePath);
            if (FileExtension == L".dds" || FileExtension == L".DDS" || FileExtension == L".tga" || FileExtension == L".TGA" ||
                FileExtension == L".png" || FileExtension == L".PNG" || FileExtension == L".bmp" || FileExtension == L".BMP" ||
                FileExtension == L".jpg" || FileExtension == L".JPG" || FileExtension == L".jpeg" || FileExtension == L".JPEG")
                Load<CTexture>(FileRelativePath, FileRelativePath);
            if (FileExtension == L".mtrl")
                Load<CMaterial>(FileRelativePath, FileRelativePath);
            if (FileExtension == L".wav" || FileExtension == L".mp3" || FileExtension == L".ogg")
                Load<CSound>(FileRelativePath, FileRelativePath);
            if (FileExtension == L".physic2Dmtrl")
                Load<CPhysics2DMaterial>(FileRelativePath, FileRelativePath);
        }
    }
}

vector<tMeshData> CAssetMgr::ReadFromFile(string _basePath, string _filename, bool _revertNormals)
{
    CModelLoader modelLoader;
    modelLoader.Load(_basePath, _filename, _revertNormals);
    vector<tMeshData>& meshes = modelLoader.meshes;

    // Normalize vertices
    Vector3 vmin(1000, 1000, 1000);
    Vector3 vmax(-1000, -1000, -1000);
    for (auto& mesh : meshes)
    {
        for (auto& v : mesh.vertices)
        {
            vmin.x = XMMin(vmin.x, v.vPos.x);
            vmin.y = XMMin(vmin.y, v.vPos.y);
            vmin.z = XMMin(vmin.z, v.vPos.z);
            vmax.x = XMMax(vmax.x, v.vPos.x);
            vmax.y = XMMax(vmax.y, v.vPos.y);
            vmax.z = XMMax(vmax.z, v.vPos.z);
        }
    }

    float dx = vmax.x - vmin.x, dy = vmax.y - vmin.y, dz = vmax.z - vmin.z;
    float dl = XMMax(XMMax(dx, dy), dz);
    float cx = (vmax.x + vmin.x) * 0.5f, cy = (vmax.y + vmin.y) * 0.5f, cz = (vmax.z + vmin.z) * 0.5f;

    for (auto& mesh : meshes)
    {
        for (auto& v : mesh.vertices)
        {
            v.vPos.x = (v.vPos.x - cx) / dl;
            v.vPos.y = (v.vPos.y - cy) / dl;
            v.vPos.z = (v.vPos.z - cz) / dl;
        }
    }

    return meshes;
}

void CAssetMgr::SetModelMaterial(const Ptr<CMaterial>& _Mtrl, const tMeshData& _MeshData)
{
    // �ؽ��� �ε�
    wstring path = ToWstring(_MeshData.RelativeTextureFilePath);

    if (!_MeshData.AmbientTextureFilename.empty())
    {
        LOG(Log, "%s", _MeshData.AmbientTextureFilename.c_str());

        wstring name = ToWstring(_MeshData.AmbientTextureFilename);

        _Mtrl->SetTexParam(TEX_0, Load<CTexture>(path + name, path + name));
    }

    if (!_MeshData.AoTextureFilename.empty())
    {
        LOG(Log, "%s", _MeshData.AoTextureFilename.c_str());

        wstring name = ToWstring(_MeshData.AoTextureFilename);

        _Mtrl->SetTexParam(TEX_1, Load<CTexture>(path + name, path + name));
    }

    if (!_MeshData.NormalTextureFilename.empty())
    {
        LOG(Log, "%s", _MeshData.NormalTextureFilename.c_str());

        wstring name = ToWstring(_MeshData.NormalTextureFilename);

        _Mtrl->SetTexParam(TEX_2, Load<CTexture>(path + name, path + name));
    }

    if (!_MeshData.HeightTextureFilename.empty())
    {
        LOG(Log, "%s", _MeshData.HeightTextureFilename.c_str());

        wstring name = ToWstring(_MeshData.HeightTextureFilename);

        _Mtrl->SetTexParam(TEX_3, Load<CTexture>(path + name, path + name));
    }

    if (!_MeshData.MetallicTextureFilename.empty() || !_MeshData.RoughnessTextureFilename.empty())
    {
        LOG(Log, "%s", _MeshData.MetallicTextureFilename.c_str());
        LOG(Log, "%s", _MeshData.RoughnessTextureFilename.c_str());
        wstring name = ToWstring(_MeshData.MetallicTextureFilename); // Metallic �̸����� ����

        // GLTF ����� �̹� ����������
        if (!_MeshData.MetallicTextureFilename.empty() && (_MeshData.MetallicTextureFilename == _MeshData.RoughnessTextureFilename))
        {
            _Mtrl->SetTexParam(TEX_4, Load<CTexture>(path + name, path + name));
        }
        else
        {
            // ���� ������ ��� ���� �о �����ݴϴ�.

            // ReadImage()�� Ȱ���ϱ� ���ؼ� �� �̹������� ���� 4ä�η� ��ȯ �� �ٽ�
            // 3ä�η� ��ġ�� ������� ����
            int mWidth = 0, mHeight = 0;
            int rWidth = 0, rHeight = 0;
            std::vector<uint8_t> mImage;
            std::vector<uint8_t> rImage;

            // �� �� �ϳ��� ���� ��쵵 ����ϱ� ���� ���� ���ϸ� Ȯ��
            if (!_MeshData.MetallicTextureFilename.empty())
            {
                ReadImage(ToString(path) + _MeshData.MetallicTextureFilename, mImage, mWidth, mHeight);
            }

            if (!_MeshData.RoughnessTextureFilename.empty())
            {
                ReadImage(ToString(path) + _MeshData.RoughnessTextureFilename, rImage, rWidth, rHeight);
            }

            // �� �̹����� �ػ󵵰� ���ٰ� ����
            if (!_MeshData.MetallicTextureFilename.empty() && !_MeshData.RoughnessTextureFilename.empty())
            {
                assert(mWidth == rWidth);
                assert(mHeight == rHeight);
            }

            vector<uint8_t> combinedImage(size_t(mWidth * mHeight) * 4);
            fill(combinedImage.begin(), combinedImage.end(), 0);

            for (size_t i = 0; i < size_t(mWidth * mHeight); i++)
            {
                if (rImage.size())
                    combinedImage[4 * i + 1] = rImage[4 * i]; // Green = Roughness
                if (mImage.size())
                    combinedImage[4 * i + 2] = mImage[4 * i]; // Blue = Metalness
            }

            // ������¡ �ؽ��� ����� CPU���� �̹����� �����մϴ�.
            ComPtr<ID3D11Texture2D> stagingTexture = CreateStagingTexture(mWidth, mHeight, combinedImage, DXGI_FORMAT_R8G8B8A8_UNORM, 0);

            // ������ ����� �ؽ��� ����
            ComPtr<ID3D11Texture2D> metallicRoughnessTexture;
            D3D11_TEXTURE2D_DESC txtDesc;
            ZeroMemory(&txtDesc, sizeof(txtDesc));
            txtDesc.Width = mWidth;
            txtDesc.Height = mHeight;
            txtDesc.MipLevels = 0; // �Ӹ� ���� �ִ�
            txtDesc.ArraySize = 1;
            txtDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            txtDesc.SampleDesc.Count = 1;
            txtDesc.Usage = D3D11_USAGE_DEFAULT; // ������¡ �ؽ���κ��� ���� ����
            txtDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
            txtDesc.CPUAccessFlags = 0;

            // �ʱ� ������ ���� �ؽ��� ���� (���� ������)
            DEVICE->CreateTexture2D(&txtDesc, NULL, metallicRoughnessTexture.GetAddressOf());

            // ��ü ����
            CONTEXT->CopyResource(metallicRoughnessTexture.Get(), stagingTexture.Get());

            _Mtrl->SetTexParam(TEX_4, CreateTexture(path + name, metallicRoughnessTexture));
        }
    }

    if (!_MeshData.EmissiveTextureFilename.empty())
    {
        LOG(Log, "%s", _MeshData.EmissiveTextureFilename.c_str());

        wstring name = ToWstring(_MeshData.EmissiveTextureFilename);

        _Mtrl->SetTexParam(TEX_5, Load<CTexture>(name, path + name));
    }
}

CGameObject* CAssetMgr::LoadModel(const wstring& _name, string _basePath, string _filename, bool _revertNormals, tMeshData _TexturesName)
{
    // �Ž� �ε�
    auto meshes = ReadFromFile(_basePath, _filename, _revertNormals);

    // .FBX �����϶��� �ؽ���/��� ����
    std::filesystem::path format = _filename;
    if (format.extension() == ".FBX" || format.extension() == ".fbx")
    {
        if (meshes[0].AmbientTextureFilename.empty())
            meshes[0].AmbientTextureFilename = _TexturesName.AmbientTextureFilename;

        if (meshes[0].AoTextureFilename.empty())
            meshes[0].AoTextureFilename = _TexturesName.AoTextureFilename;

        if (meshes[0].NormalTextureFilename.empty())
            meshes[0].NormalTextureFilename = _TexturesName.NormalTextureFilename;

        if (meshes[0].HeightTextureFilename.empty())
            meshes[0].HeightTextureFilename = _TexturesName.HeightTextureFilename;

        if (meshes[0].MetallicTextureFilename.empty())
            meshes[0].MetallicTextureFilename = _TexturesName.MetallicTextureFilename;

        if (meshes[0].RoughnessTextureFilename.empty())
            meshes[0].RoughnessTextureFilename = _TexturesName.RoughnessTextureFilename;

        if (meshes[0].EmissiveTextureFilename.empty())
            meshes[0].EmissiveTextureFilename = _TexturesName.EmissiveTextureFilename;

        if (meshes[0].RelativeTextureFilePath.empty())
            meshes[0].RelativeTextureFilePath = _TexturesName.RelativeTextureFilePath;
    }

    return LoadModel(_name, meshes);
}

CGameObject* CAssetMgr::LoadModel(const wstring& _name, vector<tMeshData> meshes)
{
    CGameObject* model = new CGameObject;
    model->SetName(_name);
    model->AddComponent(new CTransform);

    int idx = 0;
    for (auto& meshData : meshes)
    {
        CGameObject* Parts = new CGameObject;
        Parts->SetName(_name + L" Parts " + std::to_wstring(idx));

        Ptr<CMesh> pMesh = new CMesh;
        pMesh->SetName(_name + L" Parts " + std::to_wstring(idx) + L" Mesh");
        pMesh->Create(meshData.vertices.data(), (UINT)meshData.vertices.size(), meshData.indices.data(), (UINT)meshData.indices.size());
        AddAsset<CMesh>(pMesh->GetName(), pMesh);

        Parts->AddComponent(new CTransform);
        Parts->AddComponent(new CMeshRender);

        Parts->Transform()->SetAbsolute(false);
        Parts->MeshRender()->SetMesh(pMesh);
        Parts->MeshRender()->SetMaterial(FindAsset<CMaterial>(L"UnrealPBRMtrl"));

        // SetModelMaterial(Parts->GetRenderComponent()->CreateDynamicMaterial(), meshData);

        model->AddChild(Parts);
        ++idx;
    }

    return model;
}

Ptr<CTexture> CAssetMgr::CreateTexture(const wstring& _strKey, UINT _Width, UINT _Height, DXGI_FORMAT _pixelformat, UINT _BindFlag,
                                       D3D11_USAGE _Usage, const D3D11_DEPTH_STENCIL_VIEW_DESC* _dsvDesc,
                                       const D3D11_RENDER_TARGET_VIEW_DESC* _rtvDesc, const D3D11_SHADER_RESOURCE_VIEW_DESC* _srvDesc,
                                       const D3D11_UNORDERED_ACCESS_VIEW_DESC* _uavDesc)
{
    Ptr<CTexture> pTex = FindAsset<CTexture>(_strKey);

    assert(nullptr == pTex);

    pTex = new CTexture(true);
    if (FAILED(pTex->Create(_Width, _Height, _pixelformat, _BindFlag, _Usage, _dsvDesc, _rtvDesc, _srvDesc, _uavDesc)))
    {
        assert(nullptr);
    }

    AddAsset<CTexture>(_strKey, pTex);

    return pTex;
}

Ptr<CTexture> CAssetMgr::CreateTexture(const wstring& _strKey, ComPtr<ID3D11Texture2D> _Tex2D, const D3D11_DEPTH_STENCIL_VIEW_DESC* _dsvDesc,
                                       const D3D11_RENDER_TARGET_VIEW_DESC* _rtvDesc, const D3D11_SHADER_RESOURCE_VIEW_DESC* _srvDesc,
                                       const D3D11_UNORDERED_ACCESS_VIEW_DESC* _uavDesc)
{
    Ptr<CTexture> pTex = FindAsset<CTexture>(_strKey);

    assert(nullptr == pTex);

    pTex = new CTexture(true);
    if (FAILED(pTex->Create(_Tex2D, _dsvDesc, _rtvDesc, _srvDesc, _uavDesc)))
    {
        assert(nullptr);
    }

    pTex->SetKey(_strKey);
    AddAsset<CTexture>(_strKey, pTex);

    return pTex;
}

void CAssetMgr::DeleteAsset(ASSET_TYPE _type, const wstring& _strKey)
{
    map<wstring, Ptr<CAsset>>::iterator iter = m_mapAsset[(UINT)_type].find(_strKey);

    assert(!(iter == m_mapAsset[(UINT)_type].end()));

    m_mapAsset[(UINT)_type].erase(iter);
}
