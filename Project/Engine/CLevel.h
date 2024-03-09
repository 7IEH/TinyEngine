#pragma once
#include "CEntity.h"
#include "CLayer.h"

class CGameObject;

class CLevel : public CEntity
{
private:
    CLayer* m_arrLayer[LAYER_MAX];
    LEVEL_STATE m_State;
    int m_StepFrames;

public:
    virtual void begin();
    virtual void tick();
    virtual void finaltick();

    // _bChildMove : ������Ʈ�� �ڽĿ�����Ʈ�� ���� ���̾ ������ ����
    void AddObject(CGameObject* _Object, int _LayerIdx, bool _bChildMove = true);
    void AddObject(CGameObject* _Object, const wstring& _strLayerName, bool _bChildMove = true);

    CLayer* GetLayer(int _iLayerIdx) const { return m_arrLayer[_iLayerIdx]; }
    CLayer* GetLayer(const wstring& _strLayerName) const;

public:
    void ChangeState(LEVEL_STATE _NextState);
    LEVEL_STATE GetState() const { return m_State; }
    void Step(int _Frames = 1);

private:
    void clear();

    CLONE(CLevel);

public:
    CLevel();
    CLevel(const CLevel& origin);
    virtual ~CLevel();

    friend class CLevelMgr;
};