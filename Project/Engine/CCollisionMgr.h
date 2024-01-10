#pragma once

class CCollider2D;

union CollisionID {
    struct
    {
        UINT LeftID;
        UINT RightID;
    };

    UINT_PTR id;
};

class CCollisionMgr : public CSingleton<CCollisionMgr>
{
    SINGLE(CCollisionMgr);

private:
    UINT m_matrix[LAYER_MAX];
    map<UINT_PTR, bool> m_mapPrevInfo; // 이전 프레임에 두 충돌체의 충돌여부

public:
    void LayerCheck(UINT _LeftLayer, UINT _RightLayer, bool _bCheck = true);
    void LayerCheck(const wstring& _LeftLayer, const wstring& _RightLayer);

    void EnableAllLayer();
    void DisableAllLayer();

public:
    void tick();

public:
    UINT GetCollisionLayer(UINT idx) const { return m_matrix[idx]; }
    void SetCollisionLayer(UINT idx, UINT row) { m_matrix[idx] = row; }

private:
    void CollisionBtwLayer(UINT _leftCol, UINT _rightCol);
    bool CollisionBtwCollider(CCollider2D* _pLeftCol, CCollider2D* _pRightCol);

public:
    void CollisionRelease(UINT _LeftLayer, UINT _RightLayer);
    void CollisionRelease(CGameObject* _obj);

private:
    bool CollisionRectRect(CCollider2D* _pLeftCol, CCollider2D* _pRightCol);
    bool CollisionCircleCircle(CCollider2D* _pLeftCol, CCollider2D* _pRightCol);
    bool CollisionRectCircle(CCollider2D* _pRectCol, CCollider2D* _pCircleCol);
    bool CollisionPointCircle(float cx, float cy, float cr, float px, float py);
};
