#pragma once

#include "CRenderComponent.h"

class CTileMap : public CRenderComponent
{
private:
    UINT m_iTileCountX;     // ���� Ÿ�� ����
    UINT m_iTileCountY;     // ���� Ÿ�� ����
    Vec2 m_vTileRenderSize; // Ÿ�� 1ĭ ������ ������

    Ptr<CTexture> m_TileAtlas;
    Vec2 m_vTilePixelSize;

    UINT m_MaxCol;
    UINT m_MaxRow;

    UINT m_TileIdx;

public:
    void SetTileAtlas(Ptr<CTexture> _Atlas, Vec2 _TilePixelSize);
    Ptr<CTexture> GetTileAtlas() const { return m_TileAtlas; }

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
};
