#include "pch.h"
#include "CBridgeScript.h"

CBridgeScript::CBridgeScript()
    : CScript(BRIDGESCRIPT)
    , m_DisabledTime(0.f)
{
}

CBridgeScript::~CBridgeScript()
{
}
void CBridgeScript::begin()
{
    SetEnabled(false);
}

void CBridgeScript::tick()
{
    if (m_DisabledTime > 0.f)
        m_DisabledTime -= DT;
}

void CBridgeScript::SetEnabled(bool _bEnable, bool ForceEnable)
{
    if (_bEnable)
    {
        // Enable �� DisabledTime�� ���� ��쿡�� ���� - ���� Ȱ��ȭ �ΰ��� ����
        if (!ForceEnable && m_DisabledTime > 0.f)
            return; 
    }
    else
        m_DisabledTime = 0.3f;

    if (nullptr == BoxCollider2D())
        return;

    BoxCollider2D()->SetEnabled(_bEnable);
}
