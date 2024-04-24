#pragma once
#include "CComponent.h"

enum class ForceMode
{
    Force,          // ������ ����� ��ü�� �������� ���� �߰�
    Acceleration,   // ������ �����ϰ� ��ü�� �������� ���ӵ� �߰�
    Impulse,        // ������ ����Ͽ� ��ü�� �ﰢ���� �� ��� �߰�
    VelocityChange, // ������ �����ϰ� ��ü�� �ﰢ���� �ӵ� ��ȭ �߰�
};

enum class CollisionDetection
{
    Discrete,
    Continuous,
    ContinuousSpecutive,
};

class CRigidbody : public CComponent
{
private:
    void* m_RuntimeBody;
    float m_Mass;
    float m_Drag;
    float m_AngularDrag;
    bool m_bGravity;
    bool m_bKinematic;
    CollisionDetection m_CollisionDetection;
    bool m_FreezePosition[3];
    bool m_FreezeRotation[3];

public:
    virtual void finaltick() override;

public:
    void AddForce(Vec3 _Force, ForceMode _Mode = ForceMode::Force);
    void AddTorque(Vec3 _Torque, ForceMode _Mode = ForceMode::Force);

    float GetSpeed();

    Vec3 GetVelocity();
    void SetVelocity(Vec3 _vel);

    Vec3 GetAngularVelocity();
    void SetAngularVelocity(Vec3 _vel);

    Vec3 GetInertiaTensor();
    void SetInertiaTensor(Vec3 _Tensor);

    Vec3 GetLocalCenterOfMass();
    void SetLocalCenterOfMass(Vec3 _Center);

    bool IsSleeping();
    void Sleep();
    void WakeUp();

public:
    float GetMass() const { return m_Mass; }
    void SetMass(float _Mass) { m_Mass = _Mass; }

    float GetDrag() const { return m_Drag; }
    void SetDrag(float _Drag) { m_Drag = _Drag; }

    float GetAngularDrag() const { return m_AngularDrag; }
    void SetAngularDrag(float _Drag) { m_AngularDrag = _Drag; }

    bool IsUseGravity() const { return m_bGravity; }
    void SetUseGravity(bool _Use) { m_bGravity = _Use; }

    bool IsKinematic() const { return m_bKinematic; }
    void SetKinematic(bool _kinematic) { m_bKinematic = _kinematic; }

    CollisionDetection GetCollisionDetection() { return m_CollisionDetection; }
    void SetCollisionDetection(CollisionDetection _ColDet) { m_CollisionDetection = _ColDet; }

    bool IsFreezePosition(AXIS_TYPE _Axis) { return m_FreezePosition[(UINT)_Axis]; }
    void SetFreezePosition(AXIS_TYPE _Axis, bool _Freeze) { m_FreezePosition[(UINT)_Axis] = _Freeze; }

    bool IsFreezeRotation(AXIS_TYPE _Axis) { return m_FreezeRotation[(UINT)_Axis]; }
    void SetFreezeRotation(AXIS_TYPE _Axis, bool _Freeze) { m_FreezeRotation[(UINT)_Axis] = _Freeze; }

public:
    virtual void SaveToLevelFile(FILE* _File) override;
    virtual void LoadFromLevelFile(FILE* _File) override;

    CLONE(CRigidbody);

public:
    CRigidbody();
    CRigidbody(const CRigidbody& origin);
    virtual ~CRigidbody();

    friend class CPhysicsMgr;
};
