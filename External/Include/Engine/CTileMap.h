#pragma once

#include "CRenderComponent.h"

class CStructuredBuffer;

struct tTileInfo
{
    Vec2 vLeftTopUV = Vec2();
    int bRender = false;
    int ImageIndex = -1;
};

class CTileMap : public CRenderComponent
{
private:
    UINT m_iTileCountX;     // ���� Ÿ�� ����
    UINT m_iTileCountY;     // ���� Ÿ�� ����
    Vec2 m_vTileRenderSize; // ���� �������� �������� Ÿ�� ������

    Ptr<CTexture> m_TileAtlas;
    Vec2 m_vTilePixelSize; // ���� �������� Ÿ�� ������
    Vec2 m_vSliceSizeUV;

    vector<tTileInfo> m_vecTileInfo;
    CStructuredBuffer* m_TileInfoBuffer;

public:
    UINT GetTileCountX() const { return m_iTileCountX; }
    UINT GetTileCountY() const { return m_iTileCountY; }
    void SetTileCount(UINT _TileCountX, UINT _TileCountY);

    Vec2 GetTileRenderSize() const { return m_vTileRenderSize; }
    void SetTileRenderSize(Vec2 _size) { m_vTileRenderSize = _size; }

    void SetTileAtlas(Ptr<CTexture> _Atlas, Vec2 _TilePixelSize);
    Ptr<CTexture> GetTileAtlas() const { return m_TileAtlas; }

    Vec2 GetTilePixelSize() const { return m_vTilePixelSize; }

    void SetTileIndex(UINT _Row, UINT _Col, UINT _ImgIdx); // row,col ��ġ�� Ÿ�Ͽ� ImgIdx�� ����
public:
    virtual void finaltick() override;
    virtual void UpdateData() override;
    virtual void render() override;

public:
    virtual void SaveToLevelFile(FILE* _File) override;
    virtual void LoadFromLevelFile(FILE* _File) override;

public:
    CTileMap();
    virtual ~CTileMap();

    friend class CTileMapEditor;
};
