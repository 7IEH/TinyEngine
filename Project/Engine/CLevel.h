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

    // _bChildMove : ������Ʈ�� �ڽĿ�����Ʈ�� ���� ���̾ ������ ����
    void AddObject(CGameObject* _Object, int _LayerIdx, bool _bChildMove = true);
    void AddObject(CGameObject* _Object, const wstring& _strLayerName, bool _bChildMove = true);
    
    CLayer* GetLayer(int _iLayerIdx) const { return m_arrLayer[_iLayerIdx]; }
    CLayer* GetLayer(const wstring& _strLayerName) const;

private:
    void clear();

public:
    CLevel();
    virtual ~CLevel();

    friend class CLevelMgr;
};