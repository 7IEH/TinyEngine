#pragma once
#include "CEntity.h"
#include "CLayer.h"

class CGameObject;

class CLevel : public CEntity
{
private:
    CLayer* m_arrLayer[LAYER_MAX];

public:
    virtual void begin();
    virtual void tick();
    virtual void finaltick();
    virtual void render();

    void AddObject(CGameObject* _Object, int _LayerIdx,
                   bool _bChildMove = true); // _bChildMove : ������Ʈ�� �ڽĿ�����Ʈ�� ���� ���̾ ������ ����
    CLayer* GetLayer(int _iLayerIdx) const { return m_arrLayer[_iLayerIdx]; }

private:
    void clear();

public:
    CLevel();
    virtual ~CLevel();

    friend class CLevelMgr;
};