#include "pch.h"
#include "CMorphoSockWave.h"
#include "CPlayerMgr.h"

CMorphoSockWave::CMorphoSockWave()
    : CScript(MORPHOSOCKWAVE)
{
}

CMorphoSockWave::~CMorphoSockWave()
{
}

void CMorphoSockWave::tick()
{
}

void CMorphoSockWave::OnCollisionEnter(CCollider* _OtherCollider)
{
    if (_OtherCollider->GetOwner() != PLAYER)
        return;

    //@CAMERA ȸ�� (30s)
    //@EFFECT Ŀ�� ����� ȿ��
}

UINT CMorphoSockWave::SaveToLevelFile(FILE* _File)
{
    return 0;
}

UINT CMorphoSockWave::LoadFromLevelFile(FILE* _File)
{
    return 0;
}