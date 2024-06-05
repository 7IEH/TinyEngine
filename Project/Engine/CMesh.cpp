#include "pch.h"
#include "CMesh.h"

#include "CDevice.h"

#include "CAssetMgr.h"
#include "CTexture.h"

CMesh::CMesh(bool _bEngineAsset)
    : CAsset(ASSET_TYPE::MESH, _bEngineAsset)
    , m_VB{}
    , m_VBDesc{}
    , m_VtxCount(0)
    , m_VtxSysMem(nullptr)
    , m_vecIdxInfo{}
{
}

CMesh::~CMesh()
{
    if (nullptr != m_VtxSysMem)
    {
        delete m_VtxSysMem;
        m_VtxSysMem = nullptr;
    }

    for (size_t i = 0; i < m_vecIdxInfo.size(); ++i)
    {
        if (nullptr != m_vecIdxInfo[i].pIdxSysMem)
        {
            delete m_vecIdxInfo[i].pIdxSysMem;
            m_vecIdxInfo[i].pIdxSysMem = nullptr;
        }
    }
}

void CMesh::UpdateData(UINT _iSubset)
{
    UINT iStride = sizeof(Vtx);
    UINT iOffset = 0;

    CONTEXT->IASetVertexBuffers(0, 1, m_VB.GetAddressOf(), &iStride, &iOffset);
    CONTEXT->IASetIndexBuffer(m_vecIdxInfo[_iSubset].pIB.Get(), DXGI_FORMAT_R32_UINT, 0);
}

CMesh* CMesh::CreateFromContainer(CFBXLoader& _loader)
{
    const tContainer* container = &_loader.GetContainer(0);

    UINT iVtxCount = (UINT)container->vecPos.size();

    D3D11_BUFFER_DESC tVtxDesc = {};

    tVtxDesc.ByteWidth = sizeof(Vtx) * iVtxCount;
    tVtxDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    tVtxDesc.Usage = D3D11_USAGE_DEFAULT;
    if (D3D11_USAGE_DYNAMIC == tVtxDesc.Usage)
        tVtxDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    D3D11_SUBRESOURCE_DATA tSub = {};
    tSub.pSysMem = malloc(tVtxDesc.ByteWidth);
    Vtx* pSys = (Vtx*)tSub.pSysMem;
    for (UINT i = 0; i < iVtxCount; ++i)
    {
        pSys[i].vPos = container->vecPos[i];
        pSys[i].vColor = Vec4(1.f, 0.f, 1.f, 1.f);
        pSys[i].vUV = container->vecUV[i];
        pSys[i].vTangent = container->vecTangent[i];
        pSys[i].vBiTangent = container->vecBinormal[i];
        pSys[i].vNormal = container->vecNormal[i];
        // pSys[i].vWeights = container->vecWeights[i];
        // pSys[i].vIndices = container->vecIndices[i];
    }

    ComPtr<ID3D11Buffer> pVB = NULL;
    if (FAILED(DEVICE->CreateBuffer(&tVtxDesc, &tSub, pVB.GetAddressOf())))
    {
        return NULL;
    }

    CMesh* pMesh = new CMesh;
    pMesh->m_VB = pVB;
    pMesh->m_VBDesc = tVtxDesc;
    pMesh->m_VtxSysMem = pSys;

    // �ε��� ����
    UINT iIdxBufferCount = (UINT)container->vecIdx.size();
    D3D11_BUFFER_DESC tIdxDesc = {};

    for (UINT i = 0; i < iIdxBufferCount; ++i)
    {
        tIdxDesc.ByteWidth = (UINT)container->vecIdx[i].size() * sizeof(UINT); // Index Format �� R32_UINT �̱� ����
        tIdxDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        tIdxDesc.Usage = D3D11_USAGE_DEFAULT;
        if (D3D11_USAGE_DYNAMIC == tIdxDesc.Usage)
            tIdxDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

        void* pSysMem = malloc(tIdxDesc.ByteWidth);
        memcpy(pSysMem, container->vecIdx[i].data(), tIdxDesc.ByteWidth);
        tSub.pSysMem = pSysMem;

        ComPtr<ID3D11Buffer> pIB = nullptr;
        if (FAILED(DEVICE->CreateBuffer(&tIdxDesc, &tSub, pIB.GetAddressOf())))
        {
            return NULL;
        }

        tIndexInfo info = {};
        info.tIBDesc = tIdxDesc;
        info.iIdxCount = (UINT)container->vecIdx[i].size();
        info.pIdxSysMem = pSysMem;
        info.pIB = pIB;

        pMesh->m_vecIdxInfo.push_back(info);
    }

    return pMesh;
}

int CMesh::Create(void* _Vtx, UINT _VtxCount, void* _Idx, UINT _IdxCount)
{
    m_VtxCount = _VtxCount;

    // ���ؽ� ���� ����
    m_VBDesc = {};

    m_VBDesc.ByteWidth = sizeof(Vtx) * _VtxCount;
    m_VBDesc.StructureByteStride = sizeof(Vtx);
    m_VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    // ���ۿ� ������ ���� ����
    m_VBDesc.CPUAccessFlags = 0;
    m_VBDesc.Usage = D3D11_USAGE_DEFAULT;

    // g_Vtx �迭�� �����͸� �ʱ� �����ͷ� ����
    D3D11_SUBRESOURCE_DATA tSubData = {};
    tSubData.pSysMem = _Vtx;

    // ���ؽ� ���� ����
    if (FAILED(DEVICE->CreateBuffer(&m_VBDesc, &tSubData, m_VB.GetAddressOf())))
    {
        MessageBox(nullptr, L"���ؽ� ���� ���� ����", L"����", MB_OK);
        return E_FAIL;
    }

    // �ε��� ���� ����
    tIndexInfo IndexInfo = {};
    IndexInfo.iIdxCount = _IdxCount;

    IndexInfo.tIBDesc.ByteWidth = sizeof(UINT) * _IdxCount;

    // ���� ���� ���Ŀ���, ������ ������ ���� �� �� �ִ� �ɼ�
    IndexInfo.tIBDesc.CPUAccessFlags = 0;
    IndexInfo.tIBDesc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;

    // ������ �����ϴ� ������ ���� ���� �˸�
    IndexInfo.tIBDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_INDEX_BUFFER;
    IndexInfo.tIBDesc.MiscFlags = 0;
    IndexInfo.tIBDesc.StructureByteStride = 0;

    // �ʱ� �����͸� �Ѱ��ֱ� ���� ���� ����ü
    tSubData.pSysMem = _Idx;

    if (FAILED(DEVICE->CreateBuffer(&IndexInfo.tIBDesc, &tSubData, IndexInfo.pIB.GetAddressOf())))
    {
        assert(nullptr);
    }

    // ���� �������� �� �ε��� ������ �����Ҵ��� �����ٰ� ������ѵΰ� ����
    m_VtxSysMem = new Vtx[m_VtxCount];
    IndexInfo.pIdxSysMem = new UINT[IndexInfo.iIdxCount];

    memcpy(m_VtxSysMem, _Vtx, sizeof(Vtx) * m_VtxCount);
    memcpy(IndexInfo.pIdxSysMem, _Idx, sizeof(UINT) * IndexInfo.iIdxCount);

    m_vecIdxInfo.push_back(IndexInfo);

    return S_OK;
}

void CMesh::render(UINT _iSubset)
{
    UpdateData(_iSubset);

    CONTEXT->DrawIndexed(m_vecIdxInfo[_iSubset].iIdxCount, 0, 0);
}

void CMesh::render_draw(UINT _iSubset)
{
    UpdateData(_iSubset);

    CONTEXT->Draw(m_vecIdxInfo[_iSubset].iIdxCount, 0);
}

void CMesh::render_IndexedInstanced(UINT _InstanceCount)
{
    UpdateData(0);

    CONTEXT->DrawIndexedInstanced(m_vecIdxInfo[0].iIdxCount, _InstanceCount, 0, 0, 0);
}

