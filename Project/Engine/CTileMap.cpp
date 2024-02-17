#include "pch.h"
#include "CTileMap.h"

#include "CStructuredBuffer.h"

#include "CAssetMgr.h"
#include "CMesh.h"

#include "CTransform.h"

CTileMap::CTileMap()
    : CRenderComponent(COMPONENT_TYPE::TILEMAP)
    , m_iTileCountX(1)
    , m_iTileCountY(1)
    , m_vTileRenderSize()
    , m_TileAtlas()
    , m_vTilePixelSize()
    , m_vSliceSizeUV()
    , m_vecTileInfo{}
    , m_TileInfoBuffer(nullptr)
{
    CAssetMgr::GetInst()->Load<CTexture>(L"EnterTheGungeon\\Map\\MapTile.bmp", L"EnterTheGungeon\\Map\\MapTile.bmp");
    CAssetMgr::GetInst()->Load<CTexture>(L"EnterTheGungeon\\Map\\MapTileSmall.bmp", L"EnterTheGungeon\\Map\\MapTileSmall.bmp");

    SetMesh(CAssetMgr::GetInst()->FindAsset<CMesh>(L"RectMesh"));
    SetMaterial(CAssetMgr::GetInst()->FindAsset<CMaterial>(L"TileMapMtrl"));

    m_TileInfoBuffer = new CStructuredBuffer;

    SetTileCount(m_iTileCountX, m_iTileCountY);
}

CTileMap::CTileMap(const CTileMap& origin)
    : CRenderComponent(origin)
    , m_iTileCountX(origin.m_iTileCountX)
    , m_iTileCountY(origin.m_iTileCountY)
    , m_vTileRenderSize(origin.m_vTileRenderSize)
    , m_TileAtlas(origin.m_TileAtlas)
    , m_vTilePixelSize(origin.m_vTilePixelSize)
    , m_vSliceSizeUV(origin.m_vSliceSizeUV)
    , m_vecTileInfo(origin.m_vecTileInfo)
    , m_TileInfoBuffer(nullptr)
{
    if (nullptr != origin.m_TileInfoBuffer)
    {
        m_TileInfoBuffer = origin.m_TileInfoBuffer->Clone();
    }
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

void CTileMap::UpdateData()
{
    // ������ ��Ʋ�� �ؽ��� ����.
    GetMaterial()->SetTexParam(TEX_0, m_TileAtlas);

    // Ÿ���� ���� ���� ����
    GetMaterial()->SetScalarParam(INT_0, m_iTileCountX);
    GetMaterial()->SetScalarParam(INT_1, m_iTileCountY);

    // ��Ʋ�� �̹������� Ÿ�� 1���� �ڸ��� ������(UV ����)
    GetMaterial()->SetScalarParam(VEC2_0, m_vSliceSizeUV);

    // �� Ÿ�� ������ ����ȭ ���۷� �̵�
    m_TileInfoBuffer->SetData(m_vecTileInfo.data(), (UINT)m_vecTileInfo.size());

    // Ÿ�� ����ȭ ���۸� t22 �� ���ε�
    m_TileInfoBuffer->UpdateData(22);

    // ���� ������Ʈ
    GetMaterial()->UpdateData();

    Transform()->UpdateData();
}

void CTileMap::render()
{
    UpdateData();

    GetMesh()->render();
}

void CTileMap::render(Ptr<CMaterial> _mtrl)
{
    // ������ ��Ʋ�� �ؽ��� ����.
    _mtrl->SetTexParam(TEX_0, m_TileAtlas);

    // Ÿ���� ���� ���� ����
    _mtrl->SetScalarParam(INT_0, m_iTileCountX);
    _mtrl->SetScalarParam(INT_1, m_iTileCountY);

    // ��Ʋ�� �̹������� Ÿ�� 1���� �ڸ��� ������(UV ����)
    _mtrl->SetScalarParam(VEC2_0, m_vSliceSizeUV);

    // �� Ÿ�� ������ ����ȭ ���۷� �̵�
    m_TileInfoBuffer->SetData(m_vecTileInfo.data(), (UINT)m_vecTileInfo.size());

    // Ÿ�� ����ȭ ���۸� t22 �� ���ε�
    m_TileInfoBuffer->UpdateData(22);

    // ���� ������Ʈ
    _mtrl->UpdateData();

    Transform()->UpdateData();

    GetMesh()->render();
}

void CTileMap::SetTileAtlas(Ptr<CTexture> _Atlas, Vec2 _TilePixelSize)
{
    m_TileAtlas = _Atlas;
    m_vTilePixelSize = _TilePixelSize;

    m_vSliceSizeUV = Vec2(m_vTilePixelSize.x / m_TileAtlas->GetWidth(), m_vTilePixelSize.y / m_TileAtlas->GetHeight());
}

void CTileMap::SetTileCount(UINT _TileCountX, UINT _TileCountY)
{
    int originX = m_iTileCountX;
    int originY = m_iTileCountY;

    m_iTileCountX = _TileCountX;
    m_iTileCountY = _TileCountY;

    vector<tTileInfo> vecTemp;
    m_vecTileInfo.swap(vecTemp);
    m_vecTileInfo.resize(m_iTileCountX * m_iTileCountY);

    if (!vecTemp.empty())
    {
        // Ÿ�ϸ��� ũ�Ⱑ Ȯ��� ���
        if ((int)m_iTileCountX > originX || (int)m_iTileCountY > originY)
        {
            for (int y = 0; y < originY; y++)
            {
                for (int x = 0; x < originX; x++)
                {
                    m_vecTileInfo[y * m_iTileCountX + x] = vecTemp[y * originX + x];
                }
            }
        }
        else
        {
            // Ÿ�ϸ��� ũ�Ⱑ ��ҵ� ���
            for (UINT y = 0; y < m_iTileCountY; y++)
            {
                for (UINT x = 0; x < m_iTileCountX; x++)
                {
                    m_vecTileInfo[y * m_iTileCountX + x] = vecTemp[y * originX + x];
                }
            }
        }
    }

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

    m_vecTileInfo[idx].vLeftTopUV =
        Vec2((iCol * m_vTilePixelSize.x) / m_TileAtlas->GetWidth(), (iRow * m_vTilePixelSize.y) / m_TileAtlas->GetHeight());

    m_vecTileInfo[idx].bRender = 1;
    m_vecTileInfo[idx].ImageIndex = _ImgIdx;
}

void CTileMap::SaveToLevelFile(FILE* _File)
{
    CRenderComponent::SaveToLevelFile(_File);

    fwrite(&m_iTileCountX, sizeof(UINT), 1, _File);
    fwrite(&m_iTileCountY, sizeof(UINT), 1, _File);
    fwrite(&m_vTileRenderSize, sizeof(Vec2), 1, _File);

    SaveAssetRef<CTexture>(m_TileAtlas.Get(), _File);

    fwrite(&m_vTilePixelSize, sizeof(Vec2), 1, _File);
    fwrite(&m_vSliceSizeUV, sizeof(Vec2), 1, _File);

    fwrite(m_vecTileInfo.data(), sizeof(tTileInfo), m_vecTileInfo.size(), _File);
}

void CTileMap::LoadFromLevelFile(FILE* _File)
{
    CRenderComponent::LoadFromLevelFile(_File);

    fread(&m_iTileCountX, sizeof(UINT), 1, _File);
    fread(&m_iTileCountY, sizeof(UINT), 1, _File);
    fread(&m_vTileRenderSize, sizeof(Vec2), 1, _File);

    LoadAssetRef<CTexture>(m_TileAtlas, _File);

    fread(&m_vTilePixelSize, sizeof(Vec2), 1, _File);
    fread(&m_vSliceSizeUV, sizeof(Vec2), 1, _File);

    SetTileCount(m_iTileCountX, m_iTileCountY);
    fread(m_vecTileInfo.data(), sizeof(tTileInfo), m_iTileCountX * m_iTileCountY, _File);
}
