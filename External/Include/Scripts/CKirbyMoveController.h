#pragma once

#include <Engine/CScript.h>

// ���� ���� Ÿ�� (enum ������ �켱����)
enum class ForceDirType
{
    DEFORM,
    HIT,
    END,
};

// ��ü�� ������ ������ �ٲ���ϴ� ���
struct ForceDirInfo
{
    ForceDirType Type;
    Vec3 Dir; // �ٶ������ ����(World ��ǥ��)
};


class CKirbyMoveController : public CScript
{
private:
    // �Է�
    Vec3                        m_Input;

    // ����
    Vec3                        m_CurDir;
    Vec3                        m_MoveDir;
    Vec3                        m_TowardDir;
    Vec3                        m_GroundNormal;
    vector<ForceDirInfo>        m_ForceDirInfos;

    // ����
    Vec3                        m_MoveVelocity;
    float                       m_Speed;
    float                       m_JumpPower;
    float                       m_RayCastDist;
    float                       m_Gravity;



private:
    virtual void OnControllerColliderHit(struct ControllerColliderHit Hit);

public:
    virtual void begin() override;
    virtual void tick() override;

public:
    Vec3 GetInput() const { return m_Input; }

private:
    void Input();
    void SetDir();
    void Move();
    void SurfaceAlignment();

public:
    virtual void SaveToLevelFile(FILE* _File) override;
    virtual void LoadFromLevelFile(FILE* _File) override;

public:
    CLONE(CKirbyMoveController);
    CKirbyMoveController();
    CKirbyMoveController(const CKirbyMoveController& _Origin);
    virtual ~CKirbyMoveController();
};
