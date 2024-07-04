#pragma once

#include <Engine/CScript.h>

// ���� ���� Ÿ�� (enum ������ �켱����)
enum class ForceDirType
{
    DEFORM,
    HIT,
    END,
};

enum class AddForceType
{
    Acceleration,   // ������ �����ϰ� ��ü�� �ﰢ���� ���ӵ� �߰�
    VelocityChange, // ������ �����ϰ� ��ü�� �ﰢ���� �ӵ� ��ȭ �߰�
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
    bool                        m_bMoveLock;
    bool                        m_bJumpLock;
    bool                        m_bDirLock;

    // ����
    Vec3                        m_CurDir;
    Vec3                        m_MoveDir;
    Vec3                        m_TowardDir;
    Vec3                        m_GroundNormal;
    vector<ForceDirInfo>        m_ForceDirInfos;

    // ����
    Vec3                        m_MoveVelocity;
    Vec3                        m_AdditionalVelocity;
    float                       m_Speed;
    float                       m_RotSpeed;
    float                       m_JumpPower;
    float                       m_RayCastDist;
    float                       m_Gravity;
    float                       m_HoveringLimitHeight;
    float                       m_HoveringHeight;



private:
    virtual void OnControllerColliderHit(struct ControllerColliderHit Hit);

public:
    virtual void begin() override;
    virtual void tick() override;
    void AddForce(Vec3 _Force, AddForceType _Type);

public:
    Vec3 GetInput() const { return m_Input; }
    Vec3 GetMoveDir() const { return m_MoveDir; }
    float GetGravity() const { return m_Gravity; }

    void LockMove() { m_bMoveLock = true; }
    void UnlockMove() { m_bMoveLock = false; }
    void LockJump() { m_bJumpLock = true; }
    void UnlockJump() { m_bJumpLock = false; }
    void LockDirection() { m_bDirLock = true; }
    void UnlockDirection() { m_bDirLock = false; }
    void ClearHoveringHeight() { m_HoveringHeight = 0.f; }
    void ClearVelocityY() { m_MoveVelocity.y = 0.f; }
    void SetGravity(float _Gravity) { m_Gravity = _Gravity; }

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
