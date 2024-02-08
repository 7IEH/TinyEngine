#include "pch.h"
#include "CMesh.h"

#include "CDevice.h"

#include "CAssetMgr.h"
#include "CTexture.h"

#include "COutputLog.h"

CMesh::CMesh()
    : CAsset(ASSET_TYPE::MESH)
    , m_VBDesc{}
    , m_IBDesc{}
    , m_VtxCount(0)
    , m_IdxCount(0)
    , m_VtxSysMem(nullptr)
    , m_IdxSysMem(nullptr)
{
}

CMesh::~CMesh()
{
    if (nullptr != m_VtxSysMem)
    {
        delete m_VtxSysMem;
        m_VtxSysMem = nullptr;
    }

    if (nullptr != m_IdxSysMem)
    {
        delete m_IdxSysMem;
        m_IdxSysMem = nullptr;
    }
}

int CMesh::Create(void* _Vtx, UINT _VtxCount, void* _Idx, UINT _IdxCount)
{
    m_VtxCount = _VtxCount;
    m_IdxCount = _IdxCount;

    // ���ؽ� ���� ����;
    m_VBDesc = {};

    m_VBDesc.ByteWidth = sizeof(Vtx) * m_VtxCount;
    m_VBDesc.StructureByteStride = sizeof(Vtx); // ���ؽ� 1���� ũ��
    m_VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    m_VBDesc.CPUAccessFlags = 0;
    m_VBDesc.Usage = D3D11_USAGE_DEFAULT;

    // �Է� �迭�� �����͸� �ʱ� �����ͷ� ����
    D3D11_SUBRESOURCE_DATA tSubData = {};
    tSubData.pSysMem = _Vtx;

    // ���ؽ� ���� ����
    if (FAILED(DEVICE->CreateBuffer(&m_VBDesc, &tSubData, m_VB.GetAddressOf())))
    {
        MessageBox(nullptr, L"���ؽ� ���� ���� ����", L"����", MB_OK);
        return E_FAIL;
    }

    m_IBDesc = {};

    m_IBDesc.ByteWidth = sizeof(UINT) * m_IdxCount;
    m_IBDesc.StructureByteStride = sizeof(UINT);
    m_IBDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

    m_IBDesc.CPUAccessFlags = 0;
    m_IBDesc.Usage = D3D11_USAGE_DEFAULT;

    tSubData = {};
    tSubData.pSysMem = _Idx;

    // �ε��� ���� ����
    if (FAILED(DEVICE->CreateBuffer(&m_IBDesc, &tSubData, m_IB.GetAddressOf())))
    {
        MessageBox(nullptr, L"�ε��� ���� ���� ����", L"����", MB_OK);
        return E_FAIL;
    }

    // ���� �������� �� �ε��� ������ �����Ҵ��� �����ٰ� ������ѵΰ� ����

    m_VtxSysMem = new Vtx[m_VtxCount];
    m_IdxSysMem = new UINT[m_IdxCount];

    memcpy(m_VtxSysMem, _Vtx, sizeof(Vtx) * m_VtxCount);
    memcpy(m_IdxSysMem, _Idx, sizeof(UINT) * m_IdxCount);

    return S_OK;
}

void CMesh::UpdateData()
{
    UINT iStride = sizeof(Vtx);
    UINT iOffset = 0;
    CONTEXT->IASetVertexBuffers(0, 1, m_VB.GetAddressOf(), &iStride, &iOffset);
    CONTEXT->IASetIndexBuffer(m_IB.Get(), DXGI_FORMAT_R32_UINT, 0);
}

void CMesh::render()
{
    UpdateData();
    CONTEXT->DrawIndexed(m_IdxCount, 0, 0);
}

void CMesh::render_draw()
{
    UpdateData();
    CONTEXT->Draw(m_VtxCount, 0);
}

void CMesh::render_IndexedInstanced(UINT _InstanceCount)
{
    UpdateData();
    CONTEXT->DrawIndexedInstanced(m_IdxCount, _InstanceCount, 0, 0, 0);
}

int CMesh::Save(const wstring& _strFilePath)
{
    // ����� ����
    SetRelativePath(_strFilePath);

    // ���� ��� �����
    wstring strFilePath = CPathMgr::GetContentPath() + _strFilePath;

    FILE* pFile = nullptr;
    _wfopen_s(&pFile, strFilePath.c_str(), L"wb");

    if (nullptr == pFile)
    {
        LOG(Error, "���� ���� ����");
        return E_FAIL;
    }

    // Ű��, ��� ���
    SaveWString(GetName(), pFile);
    SaveWString(GetKey(), pFile);
    SaveWString(GetRelativePath(), pFile);

    fwrite(&m_VtxCount, sizeof(UINT), 1, pFile);
    fwrite(&m_IdxCount, sizeof(UINT), 1, pFile);

    // ���� ������ ����
    int iByteSize = m_VBDesc.ByteWidth;
    fwrite(&iByteSize, sizeof(int), 1, pFile);
    fwrite(m_VtxSysMem, iByteSize, 1, pFile);

    // �ε��� ����
    iByteSize = m_IBDesc.ByteWidth;
    fwrite(&iByteSize, sizeof(int), 1, pFile);
    fwrite(m_IdxSysMem, iByteSize, 1, pFile);

    fclose(pFile);

    return S_OK;
}

int CMesh::Load(const wstring& _strFilePath)
{
    // �б���� ���Ͽ���
    FILE* pFile = nullptr;
    _wfopen_s(&pFile, _strFilePath.c_str(), L"rb");

    if (nullptr == pFile)
    {
        LOG(Error, "���� ���� ����");
        return E_FAIL;
    }

    // Ű��, �����
    wstring strName, strKey, strRelativePath;
    LoadWString(strName, pFile);
    LoadWString(strKey, pFile);
    LoadWString(strRelativePath, pFile);

    SetName(strName);
    SetKey(strKey);
    SetRelativePath(strRelativePath);

    fread(&m_VtxCount, sizeof(UINT), 1, pFile);
    fread(&m_IdxCount, sizeof(UINT), 1, pFile);

    // ����������
    UINT iByteSize = 0;
    fread(&iByteSize, sizeof(int), 1, pFile);
    m_VtxSysMem = (Vtx*)malloc(iByteSize);
    fread(m_VtxSysMem, 1, iByteSize, pFile);

    // �ε��� ������
    iByteSize = 0;
    fread(&iByteSize, sizeof(int), 1, pFile);
    m_IdxSysMem = (Vtx*)malloc(iByteSize);
    fread(m_IdxSysMem, 1, iByteSize, pFile);

    Create(m_VtxSysMem, m_VtxCount, m_IdxSysMem, m_IdxCount);

    fclose(pFile);

    return S_OK;
}
