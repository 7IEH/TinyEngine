#pragma once
#include "CEntity.h"

class CGameObject;

class CLayer : public CEntity
{
private:
    vector<CGameObject*> m_vecParent;
    vector<CGameObject*> m_vecObjects;
    int m_iLayerIdx;

public:
    const vector<CGameObject*>& GetParentObject() const { return m_vecParent; }

public:
    void begin();
    void tick();
    void finaltick();

public:
    // Ư�� ������Ʈ�� ���̾�� ����
    void DetachGameObject(CGameObject* _Object);
    void RegisterGameObject(CGameObject* _Object) { m_vecObjects.push_back(_Object); }
    int GetLayerIdx() const { return m_iLayerIdx; }

    const vector<CGameObject*>& GetParentObjects() const { return m_vecParent; }
    const vector<CGameObject*>& GetLayerObjects() const { return m_vecObjects; }

    void AddObject(CGameObject* _Object, bool _bMove);

    CLONE(CLayer);
public:
    CLayer();
    CLayer(const CLayer& origin);
    virtual ~CLayer();

    friend class CLevel;
};
