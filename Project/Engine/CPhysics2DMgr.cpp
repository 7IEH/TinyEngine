#include "pch.h"
#include "CPhysics2DMgr.h"
#include "CLevelMgr.h"
#include "CTimeMgr.h"

#include "CLevel.h"
#include "CGameObject.h"
#include "components.h"

#include <box2d\\b2_world.h>
#include <box2d\\b2_body.h>
#include <box2d\\b2_fixture.h>
#include <box2d\\b2_polygon_shape.h>
#include <box2d\\b2_circle_shape.h>

#include <box2d\\b2_contact.h>

void CollisionCallback::BeginContact(b2Contact* contact)
{
    CCollider2D* pColliderA = (CCollider2D*)contact->GetFixtureA()->GetUserData().pointer;
    CCollider2D* pColliderB = (CCollider2D*)contact->GetFixtureB()->GetUserData().pointer;

    if (pColliderA->IsTrigger() || pColliderB->IsTrigger())
    {
        pColliderA->OnTriggerEnter(pColliderB);
        pColliderB->OnTriggerEnter(pColliderA);
    }
    else
    {
        pColliderA->OnCollisionEnter(pColliderB);
        pColliderB->OnCollisionEnter(pColliderA);
    }
}

void CollisionCallback::EndContact(b2Contact* contact)
{
    CCollider2D* pColliderA = (CCollider2D*)contact->GetFixtureA()->GetUserData().pointer;
    CCollider2D* pColliderB = (CCollider2D*)contact->GetFixtureB()->GetUserData().pointer;

    if (pColliderA->IsTrigger() || pColliderB->IsTrigger())
    {
        pColliderA->OnTriggerExit(pColliderB);
        pColliderB->OnTriggerExit(pColliderA);
    }
    else
    {
        pColliderA->OnCollisionExit(pColliderB);
        pColliderB->OnCollisionExit(pColliderA);
    }
}

static b2BodyType Rigidbody2DTypeTob2BodyType(BODY_TYPE bodyType)
{
    switch (bodyType)
    {
    case BODY_TYPE::Dynamic:
        return b2_dynamicBody;
    case BODY_TYPE::Kinematic:
        return b2_kinematicBody;
    case BODY_TYPE::Static:
        return b2_staticBody;
    }

    LOG(Error, "Unknown body type");
    return b2_dynamicBody;
}

CPhysics2DMgr::CPhysics2DMgr()
    : m_PhysicsWorld(nullptr)
    , m_CallbackInst()
    , m_vecPhysicsObj{}
    , m_Matrix{}
    , m_PPM(100.f)
{
    EnableAllLayer();
}

CPhysics2DMgr::~CPhysics2DMgr()
{
    if (nullptr != m_PhysicsWorld)
    {
        delete m_PhysicsWorld;
        m_PhysicsWorld = nullptr;
    }
}

void CPhysics2DMgr::tick()
{
    if (nullptr == m_PhysicsWorld)
        return;

    const int32_t velocityIterations = 6; // �ӵ��� �󸶳� ���ϰ� �����ؾ��ϴ���
    const int32_t positionIterations = 2; // ��ġ�� �󸶳� ���ϰ� �����ؾ� �ϴ���

    // �ùķ��̼�
    m_PhysicsWorld->Step(DT, velocityIterations, positionIterations);

    // �ùķ��̼� ���� �浹 ī��Ʈ�� �����ִٸ� �浹 Stay ����
    b2Contact* contact = m_PhysicsWorld->GetContactList();
    while (contact)
    {
        CCollider2D* pColliderA = (CCollider2D*)contact->GetFixtureA()->GetUserData().pointer;
        CCollider2D* pColliderB = (CCollider2D*)contact->GetFixtureB()->GetUserData().pointer;

        if (pColliderA->IsTrigger() || pColliderB->IsTrigger())
        {
            if (pColliderA->m_TriggerCount > 0)
                pColliderA->OnTriggerStay(pColliderB);
            if (pColliderB->m_TriggerCount > 0)
                pColliderB->OnTriggerStay(pColliderA);
        }
        else
        {
            if (pColliderA->m_CollisionCount > 0)
                pColliderA->OnCollisionStay(pColliderB);
            if (pColliderB->m_CollisionCount > 0)
                pColliderB->OnCollisionStay(pColliderA);
        }

        contact = contact->GetNext();
    }

    for (UINT i = 0; i < m_vecPhysicsObj.size(); i++)
    {
        CTransform* pTr = m_vecPhysicsObj[i]->Transform();
        CRigidbody2D* rb2d = m_vecPhysicsObj[i]->Rigidbody2D();

        // �ùķ��̼� ��������� Ʈ������ ������Ʈ
        if (nullptr != rb2d)
        {
            b2Body* body = (b2Body*)rb2d->m_RuntimeBody;
            const auto& position = body->GetPosition();
            Vec3 pos = pTr->GetRelativePos();
            Vec3 rot = pTr->GetRelativeRotation();

            pos.x += (position.x * m_PPM) - pTr->GetWorldPos().x;
            pos.y += (position.y * m_PPM) - pTr->GetWorldPos().y;

            rot.z += body->GetAngle() - pTr->GetWorldRotation().z;

            pTr->SetRelativePos(pos);
            pTr->SetRelativeRotation(rot);
        }
    }
}

void CPhysics2DMgr::OnPhysics2DStart()
{
    OnPhysics2DStop(); // Clear

    m_PhysicsWorld = new b2World({0.0f, -9.8f});
    m_PhysicsWorld->SetContactListener(&m_CallbackInst);

    // ������ ��� ������Ʈ�� ��ȸ�Ͽ� World �� �߰��� ������Ʈ�� ���
    CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurrentLevel();
    for (UINT i = 0; i < LAYER_MAX; i++)
    {
        const vector<CGameObject*>& vecParentObj = pCurLevel->GetLayer(i)->GetParentObjects();

        for (const auto& ParentObj : vecParentObj)
        {
            list<CGameObject*> queue;
            queue.push_back(ParentObj);

            while (!queue.empty())
            {
                CGameObject* pObject = queue.front();
                queue.pop_front();

                const vector<CGameObject*>& vecChildObj = pObject->GetChildObject();

                for (size_t i = 0; i < vecChildObj.size(); ++i)
                {
                    queue.push_back(vecChildObj[i]);
                }

                AddPhysicsObject(pObject);
            }
        }
    }
}

void CPhysics2DMgr::OnPhysics2DStop()
{
    if (nullptr != m_PhysicsWorld)
    {
        delete m_PhysicsWorld;
        m_PhysicsWorld = nullptr;
    }

    m_vecPhysicsObj.clear();
}

void CPhysics2DMgr::AddPhysicsObject(CGameObject* _GameObject)
{
    if (nullptr == m_PhysicsWorld)
        return;

    CTransform* pTr = _GameObject->Transform();
    CRigidbody2D* rb2d = _GameObject->Rigidbody2D();
    CBoxCollider2D* bc2d = _GameObject->BoxCollider2D();
    CCircleCollider2D* cc2d = _GameObject->CircleCollider2D();

    if (nullptr == rb2d && nullptr == bc2d && nullptr == cc2d)
        return;

    b2BodyDef bodyDef;
    bodyDef.position.Set(pTr->GetWorldPos().x / m_PPM, pTr->GetWorldPos().y / m_PPM);
    bodyDef.angle = pTr->GetWorldRotation().z;
    b2Body* body = nullptr;

    // Rigidbody 2D
    if (nullptr != rb2d)
    {
        bodyDef.type = Rigidbody2DTypeTob2BodyType(rb2d->m_BodyType);
        bodyDef.linearDamping = rb2d->m_LinearDrag;
        bodyDef.angularDamping = rb2d->m_AngularDrag;
        bodyDef.gravityScale = rb2d->m_GravityScale;
        bodyDef.enabled = rb2d->m_bSimulated;

        body = m_PhysicsWorld->CreateBody(&bodyDef);
        rb2d->m_RuntimeBody = body;

        body->SetFixedRotation(rb2d->m_bFreezeRotation);
    }
    else
    {
        body = m_PhysicsWorld->CreateBody(&bodyDef);
    }

    // Box Collider 2D
    if (nullptr != bc2d)
    {
        b2PolygonShape boxShape;
        boxShape.SetAsBox(bc2d->m_Size.x * pTr->GetWorldScale().x / m_PPM, bc2d->m_Size.y * pTr->GetWorldScale().y / m_PPM,
                          b2Vec2(bc2d->m_Offset.x / m_PPM, bc2d->m_Offset.y / m_PPM), 0.f);

        b2FixtureDef fixtureDef;
        fixtureDef.shape = &boxShape;
        fixtureDef.userData.pointer = reinterpret_cast<uintptr_t>(bc2d);
        Ptr<CPhysics2DMaterial> Mtrl = bc2d->GetMaterial();
        if (nullptr != Mtrl)
        {
            fixtureDef.friction = Mtrl->m_Friction;
            fixtureDef.restitution = Mtrl->m_Bounciness;
        }
        fixtureDef.density = 1.f;
        fixtureDef.isSensor = nullptr == rb2d ? true : bc2d->m_bTrigger;

        fixtureDef.filter.categoryBits = (1 << _GameObject->GetLayerIdx());
        fixtureDef.filter.maskBits = m_Matrix[_GameObject->GetLayerIdx()];

        bc2d->m_RuntimeFixture = body->CreateFixture(&fixtureDef);
    }

    // Circle Collider 2D
    if (nullptr != cc2d)
    {
        b2CircleShape circleShape;
        circleShape.m_p.Set(cc2d->m_Offset.x / m_PPM, cc2d->m_Offset.y / m_PPM);
        circleShape.m_radius = pTr->GetWorldScale().x * cc2d->m_Radius / m_PPM;

        b2FixtureDef fixtureDef;
        fixtureDef.shape = &circleShape;
        fixtureDef.userData.pointer = reinterpret_cast<uintptr_t>(cc2d);
        Ptr<CPhysics2DMaterial> Mtrl = cc2d->GetMaterial();
        if (nullptr != Mtrl)
        {
            fixtureDef.friction = Mtrl->m_Friction;
            fixtureDef.restitution = Mtrl->m_Bounciness;
        }
        fixtureDef.density = 1.f;
        fixtureDef.isSensor = nullptr == rb2d ? true : cc2d->m_bTrigger;

        fixtureDef.filter.categoryBits = (1 << _GameObject->GetLayerIdx());
        fixtureDef.filter.maskBits = m_Matrix[_GameObject->GetLayerIdx()];

        cc2d->m_RuntimeFixture = body->CreateFixture(&fixtureDef);
    }

    // ���� ����
    if (nullptr != rb2d && !rb2d->m_bAutoMass)
    {
        b2MassData MassData = body->GetMassData();
        MassData.mass = rb2d->m_Mass;
        body->SetMassData(&MassData);
    }

    m_vecPhysicsObj.push_back(_GameObject);
}

void CPhysics2DMgr::RemovePhysicsObject(CGameObject* _GameObject)
{
    if (nullptr == m_PhysicsWorld)
        return;

    for (UINT i = 0; i < m_vecPhysicsObj.size(); i++)
    {
        if (m_vecPhysicsObj[i] != _GameObject)
            continue;

        CRigidbody2D* rb2d = _GameObject->Rigidbody2D();
        CBoxCollider2D* bc2d = _GameObject->BoxCollider2D();
        CCircleCollider2D* cc2d = _GameObject->CircleCollider2D();

        b2Body* body = nullptr;

        if (nullptr != rb2d)
            body = (b2Body*)rb2d->m_RuntimeBody;
        else if (nullptr != bc2d)
            body = ((b2Fixture*)bc2d->m_RuntimeFixture)->GetBody();
        else if (nullptr != cc2d)
            body = ((b2Fixture*)cc2d->m_RuntimeFixture)->GetBody();

        m_PhysicsWorld->DestroyBody(body);
        m_vecPhysicsObj.erase(m_vecPhysicsObj.begin() + i);
        break;
    }
}

void CPhysics2DMgr::LayerCheck(UINT _LeftLayer, UINT _RightLayer, bool _bCheck)
{
    UINT iRow = (UINT)_LeftLayer;
    UINT iCol = (UINT)_RightLayer;

    if (_bCheck)
    {
        m_Matrix[iRow] |= (1 << iCol);
        m_Matrix[iCol] |= (1 << iRow);
    }
    else
    {
        m_Matrix[iRow] &= ~(1 << iCol);
        m_Matrix[iCol] &= ~(1 << iRow);
    }
}

void CPhysics2DMgr::LayerCheck(CLevel* _CurLevel, const wstring& _LeftLayer, const wstring& _RightLayer)
{
    CLayer* pLeftLayer = _CurLevel->GetLayer(_LeftLayer);
    CLayer* pRightLayer = _CurLevel->GetLayer(_RightLayer);

    // �̸��� �ش��ϴ� Layer �� �������� ������
    assert(pLeftLayer && pRightLayer);

    LayerCheck(pLeftLayer->GetLayerIdx(), pRightLayer->GetLayerIdx());
}

void CPhysics2DMgr::EnableAllLayer()
{
    for (int i = 0; i < LAYER_MAX; ++i)
    {
        m_Matrix[i] = 0xFFFF;
    }
}

void CPhysics2DMgr::DisableAllLayer()
{
    for (int i = 0; i < LAYER_MAX; ++i)
    {
        m_Matrix[i] = 0;
    }
}

CGameObject* CPhysics2DMgr::CollisionCheck(Vec2 _point)
{
    CGameObject* pSelectedObj = nullptr;

    if (nullptr == m_PhysicsWorld)
        OnPhysics2DStart();

    for (UINT i = 0; i < m_vecPhysicsObj.size(); i++)
    {
        CBoxCollider2D* bc2d = m_vecPhysicsObj[i]->BoxCollider2D();
        CCircleCollider2D* cc2d = m_vecPhysicsObj[i]->CircleCollider2D();

        b2Fixture* fixture = nullptr;

        if (nullptr != bc2d)
            fixture = (b2Fixture*)bc2d->m_RuntimeFixture;
        else if (nullptr != cc2d)
            fixture = (b2Fixture*)cc2d->m_RuntimeFixture;

        if (nullptr == fixture)
            continue;

        if (fixture->TestPoint(b2Vec2(_point.x / m_PPM, _point.y / m_PPM)))
        {
            pSelectedObj = m_vecPhysicsObj[i];
            break;
        }
    }

    return pSelectedObj;
}
