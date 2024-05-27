#include "pch.h"
#include "CBulletScript.h"

CBulletScript::CBulletScript()
    : CScript(BULLETSCRIPT)
    , m_InitVelocity()
{
}

CBulletScript::~CBulletScript()
{
}

void CBulletScript::begin()
{
    if (Rigidbody())
    {
        Rigidbody()->AddForce(m_InitVelocity, ForceMode::Impulse); // Impule�� �ѹ��� ���� ���� ����
    }
}
