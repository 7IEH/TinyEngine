#include "pch.h"
#include "CTileMap.h"

#include "CStructuredBuffer.h"

#include "CAssetMgr.h"
#include "CMesh.h"

#include "CTransform.h"

CTileMap::CTileMap()
    : CRenderComponent(COMPONENT_TYPE::TILEMAP)
    , m_iTileCountX(0)
    , m_iTileCountY(0)
    , m_vTileRenderSize(Vec2(64.f, 64.f))
    , m_TileAtlas()
    , m_vTilePixelSize()
    , m_vSliceSizeUV()
    , m_vecTileInfo{}
    , m_TileInfoBuffer(nullptr)
{
    SetMesh(CAssetMgr::GetInst()->FindAsset<CMesh>(L"RectMesh"));
    SetMaterial(CAssetMgr::GetInst()->FindAsset<CMaterial>(L"TileMapMtrl"));

    m_TileInfoBuffer = new CStructuredBuffer;

    SetTileCount(m_iTileCountX, m_iTileCountY);
}

CTileMap::~CTileMap()
{
    if (nullptr != m_TileInfoBuffer)
        delete m_TileInfoBuffer;
}

void CTileMap::finaltick()
{
    // (Ÿ�� ���� * Ÿ�� ������) �� ����� ����ó���Ѵ�.
    Vec3 vTileMapSize = Vec3(m_iTileCountX * m_vTileRenderSize.x, m_iTileCountY * m_vTileRenderSize.y, 1.f);
    Transform()->SetRelativeScale(vTileMapSize);
}

void CTileMap::render()
{
    // ������ ��Ʋ�� �ؽ��� ����.
    GetMaterial()->SetTexParam(TEX_0, m_TileAtlas);

    // Ÿ���� ���� ���� ����
    GetMaterial()->SetScalarParam(INT_0, &m_iTileCountX);
    GetMaterial()->SetScalarParam(INT_1, &m_iTileCountY);

    // ��Ʋ�� �̹������� Ÿ�� 1���� �ڸ��� ������(UV ����)
    GetMaterial()->SetScalarParam(VEC2_0, &m_vSliceSizeUV);

    // �� Ÿ�� ������ ����ȭ ���۷� �̵�
    m_TileInfoBuffer->SetData(m_vecTileInfo.data(), (UINT)m_vecTileInfo.size());

    // Ÿ�� ����ȭ ���۸� t20 �� ���ε�
    m_TileInfoBuffer->UpdateData(20);

    // ���� ������Ʈ
    GetMaterial()->UpdateData();

    Transform()->UpdateData();

    GetMesh()->render();
}

void CTileMap::UpdateData()
{
}

void CTileMap::SetTileAtlas(Ptr<CTexture> _Atlas, Vec2 _TilePixelSize)
{
    m_TileAtlas = _Atlas;
    m_vTilePixelSize = _TilePixelSize;

    m_vSliceSizeUV = Vec2(m_vTilePixelSize.x / m_TileAtlas->GetWidth(), m_vTilePixelSize.y / m_TileAtlas->GetHeight());
}

void CTileMap::SetTileCount(UINT _TileCountX, UINT _TileCountY)
{
    m_iTileCountX = _TileCountX;
    m_iTileCountY = _TileCountY;

    vector<tTileInfo> vecTemp;
    m_vecTileInfo.swap(vecTemp);
    m_vecTileInfo.resize(m_iTileCountX * m_iTileCountY);

    m_TileInfoBuffer->Create(sizeof(tTileInfo), m_iTileCountX * m_iTileCountY, SB_TYPE::READ_ONLY, true);
}

void CTileMap::SetTileIndex(UINT _Row, UINT _Col, UINT _ImgIdx)
{
    if (nullptr == m_TileAtlas)
        return;

    UINT idx = _Row * m_iTileCountX + _Col;

    // �������� Ÿ�� ����
    UINT iRow = _ImgIdx / UINT(m_vTilePixelSize.x * m_iTileCountX);
    UINT iCol = _ImgIdx % UINT(m_vTilePixelSize.y * m_iTileCountX);

    m_vecTileInfo[idx].vLeftTopUV = Vec2((iCol * m_vTilePixelSize.x) / m_TileAtlas->GetWidth(),
                                         (iRow * m_vTilePixelSize.y) / m_TileAtlas->GetHeight());

    m_vecTileInfo[idx].bRender = 1;
}

void CTileMap::SaveToLevelFile(FILE* _File)
{
}

void CTileMap::LoadFromLevelFile(FILE* _File)
{
}
