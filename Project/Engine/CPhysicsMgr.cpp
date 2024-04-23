#include "pch.h"
#include "CPhysicsMgr.h"
#include "CTimeMgr.h"
#include "CLevelMgr.h"
#include "CAssetMgr.h"
#include "CRenderMgr.h"

#include "CLevel.h"
#include "CGameObject.h"
#include "components.h"

using namespace physx;

CPhysicsMgr::CPhysicsMgr()
    : m_Allocator()
    , m_ErrorCallback()
    , m_Foundation(NULL)
    , m_Physics(NULL)
    , m_Dispatcher(NULL)
    , m_Scene(NULL)
    , m_Pvd(NULL)
    , m_vecPhysicsObj{}
{
}

CPhysicsMgr::~CPhysicsMgr()
{
    OnPhysicsStop();
}

void CPhysicsMgr::tick()
{
    if (nullptr == m_Scene)
        return;

    m_Scene->simulate(DT);
    m_Scene->fetchResults(true);

    // �ùķ��̼� ����� Ʈ������ ������Ʈ
    PxU32 nbActors = m_Scene->getNbActors(PxActorTypeFlag::eRIGID_DYNAMIC | PxActorTypeFlag::eRIGID_STATIC);
    std::vector<PxRigidActor*> actors(nbActors);
    m_Scene->getActors(PxActorTypeFlag::eRIGID_DYNAMIC | PxActorTypeFlag::eRIGID_STATIC, reinterpret_cast<PxActor**>(&actors[0]), nbActors);

    for (PxU32 i = 0; i < nbActors; i++)
    {
        const PxU32 nbShapes = actors[i]->getNbShapes();
        vector<PxShape*> shapes(nbShapes);
        actors[i]->getShapes(shapes.data(), nbShapes);

        if (actors[i]->is<PxRigidDynamic>())
        {
            CGameObject* obj = (CGameObject*)actors[i]->userData;
            CTransform* pTr = obj->Transform();

            Vec3 WorldPos = pTr->GetWorldPos();
            Vec3 WorldRot = pTr->GetWorldRotation();

            const PxMat44 ActorPose(actors[i]->getGlobalPose());
            Matrix SimulatedMat = Matrix(ActorPose.front());

            // �ùķ��̼� Matrix SRT ����
            float Ftranslation[3] = {0.0f, 0.0f, 0.0f}, Frotation[3] = {0.0f, 0.0f, 0.0f}, Fscale[3] = {0.0f, 0.0f, 0.0f};
            ImGuizmo::DecomposeMatrixToComponents(*SimulatedMat.m, Ftranslation, Frotation, Fscale);

            // ��ȭ�� ����
            Vec3 vPosOffset = Vec3(WorldPos.x - Ftranslation[0], WorldPos.y - Ftranslation[1], WorldPos.z - Ftranslation[2]);
            Vec3 vRotOffset = Vec3(WorldRot.x - DirectX::XMConvertToRadians(Frotation[0]), WorldRot.y - DirectX::XMConvertToRadians(Frotation[1]),
                                   WorldRot.z - DirectX::XMConvertToRadians(Frotation[2]));

            // ��ȭ����ŭ Relative �� ����
            pTr->SetRelativePos(pTr->GetRelativePos() - vPosOffset);
            pTr->SetRelativeRotation(pTr->GetRelativeRotation() - vRotOffset);
        }
    }
}

void CPhysicsMgr::OnPhysicsStart()
{
    m_Foundation = PxCreateFoundation(PX_PHYSICS_VERSION, m_Allocator, m_ErrorCallback);

    m_Pvd = PxCreatePvd(*m_Foundation);
    PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate(PVD_HOST, 5425, 10);
    m_Pvd->connect(*transport, PxPvdInstrumentationFlag::eALL);

    // 1M ����
    // PxTolerancesScale worldScale;
    // worldScale.length = 100; // typical length of an object
    // worldScale.speed = 981;  // typical speed of an object, gravity*1s is a reasonable choice

    m_Physics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_Foundation, PxTolerancesScale(), true, m_Pvd);

    PxSceneDesc sceneDesc(m_Physics->getTolerancesScale());
    sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
    m_Dispatcher = PxDefaultCpuDispatcherCreate(2);
    sceneDesc.cpuDispatcher = m_Dispatcher;
    sceneDesc.filterShader = PxDefaultSimulationFilterShader;
    m_Scene = m_Physics->createScene(sceneDesc);

    PxPvdSceneClient* pvdClient = m_Scene->getScenePvdClient();
    if (pvdClient)
    {
        pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
        pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
        pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
    }

    // ������ ��� ������Ʈ�� ��ȸ�Ͽ� Scene �� �߰��� ������Ʈ�� ���
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

void CPhysicsMgr::OnPhysicsStop()
{
    PX_RELEASE(m_Scene);
    PX_RELEASE(m_Dispatcher);
    PX_RELEASE(m_Physics);
    if (m_Pvd)
    {
        PxPvdTransport* transport = m_Pvd->getTransport();
        m_Pvd->release();
        m_Pvd = NULL;
        PX_RELEASE(transport);
    }
    PX_RELEASE(m_Foundation);

    for (UINT i = 0; i < m_vecPhysicsObj.size(); i++)
    {
        CRigidbody* pRigidbody = m_vecPhysicsObj[i]->Rigidbody();
        CBoxCollider* pBoxCol = m_vecPhysicsObj[i]->BoxCollider();
        CSphereCollider* pSphereCol = m_vecPhysicsObj[i]->SphereCollider();
        CCapsuleCollider* pCapsuleCol = m_vecPhysicsObj[i]->CapsuleCollider();

        if (nullptr != pRigidbody)
            pRigidbody->m_RuntimeBody = nullptr;
        if (nullptr != pBoxCol)
            pBoxCol->m_RuntimeShape = nullptr;
        if (nullptr != pSphereCol)
            pSphereCol->m_RuntimeShape = nullptr;
        if (nullptr != pCapsuleCol)
            pCapsuleCol->m_RuntimeShape = nullptr;
    }

    m_vecPhysicsObj.clear();
}

void CPhysicsMgr::AddPhysicsObject(CGameObject* _GameObject)
{
    if (nullptr == m_Scene)
        return;

    CTransform* pTr = _GameObject->Transform();
    CRigidbody* pRigidbody = _GameObject->Rigidbody();
    CBoxCollider* pBoxCol = _GameObject->BoxCollider();
    CSphereCollider* pSphereCol = _GameObject->SphereCollider();
    CCapsuleCollider* pCapsuleCol = _GameObject->CapsuleCollider();

    if (nullptr == pRigidbody && nullptr == pBoxCol && nullptr == pSphereCol && nullptr == pCapsuleCol)
        return;

    Vec3 WorldPos = pTr->GetWorldPos();
    Vec3 WorldRot = pTr->GetWorldRotation();
    Vec3 WorldScale = pTr->GetWorldScale();

    SimpleMath::Quaternion QuatX = SimpleMath::Quaternion::CreateFromAxisAngle(Vec3(1.f, 0.f, 0.f), WorldRot.x);
    SimpleMath::Quaternion QuatY = SimpleMath::Quaternion::CreateFromAxisAngle(Vec3(0.f, 1.f, 0.f), WorldRot.y);
    SimpleMath::Quaternion QuatZ = SimpleMath::Quaternion::CreateFromAxisAngle(Vec3(0.f, 0.f, 1.f), WorldRot.z);
    SimpleMath::Quaternion Quat = QuatX * QuatY * QuatZ;

    PxTransform PxTr = PxTransform(WorldPos.x, WorldPos.y, WorldPos.z, PxQuat(Quat.x, Quat.y, Quat.z, Quat.w));

    PxRigidActor* RigidActor = nullptr;

    // RigidBody
    if (nullptr != pRigidbody)
    {
        PxRigidDynamic* dynamicRigid = m_Physics->createRigidDynamic(PxTr);

        dynamicRigid->setMass(pRigidbody->m_Mass);
        dynamicRigid->setLinearDamping(pRigidbody->m_Drag);
        dynamicRigid->setAngularDamping(pRigidbody->m_AngularDrag);
        dynamicRigid->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, !pRigidbody->m_bGravity);
        dynamicRigid->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, pRigidbody->m_bKinematic);

        dynamicRigid->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_LINEAR_X, pRigidbody->m_FreezePosition[0]);
        dynamicRigid->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_LINEAR_Y, pRigidbody->m_FreezePosition[1]);
        dynamicRigid->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_LINEAR_Z, pRigidbody->m_FreezePosition[2]);
        dynamicRigid->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, pRigidbody->m_FreezeRotation[0]);
        dynamicRigid->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y, pRigidbody->m_FreezeRotation[1]);
        dynamicRigid->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z, pRigidbody->m_FreezeRotation[2]);

        RigidActor = dynamicRigid;
        pRigidbody->m_RuntimeBody = dynamicRigid;
    }
    // ������ٵ� ���� �ݶ��̴��� ������ ���
    else
    {
        PxRigidStatic* staticRigid = m_Physics->createRigidStatic(PxTr);
        RigidActor = staticRigid;
    }

    Ptr<CPhysicMaterial> pDefaultMtrl = CAssetMgr::GetInst()->FindAsset<CPhysicMaterial>(L"Default Material");
    PxMaterial* DefaultPxMtrl =
        m_Physics->createMaterial(pDefaultMtrl->GetStaticFriction(), pDefaultMtrl->GetDynamicFriction(), pDefaultMtrl->GetBounciness());

    // Box Collider
    if (nullptr != pBoxCol)
    {
        PxMaterial* pPxMtrl = DefaultPxMtrl;
        if (nullptr != pBoxCol->m_Mtrl)
        {
            pPxMtrl = m_Physics->createMaterial(pBoxCol->m_Mtrl->GetStaticFriction(), pBoxCol->m_Mtrl->GetDynamicFriction(),
                                                pBoxCol->m_Mtrl->GetBounciness());
        }

        PxShape* shape = PxRigidActorExt::createExclusiveShape(
            *RigidActor, PxBoxGeometry(WorldScale.x * pBoxCol->m_Size.x, WorldScale.y * pBoxCol->m_Size.y, WorldScale.z * pBoxCol->m_Size.z),
            *pPxMtrl);

        Vec3 Center = pBoxCol->GetCenter();
        PxTransform LocalPos = shape->getLocalPose();
        LocalPos.p.x = Center.x;
        LocalPos.p.y = Center.y;
        LocalPos.p.z = Center.z;
        shape->setLocalPose(LocalPos);

        shape->userData = (void*)pBoxCol;
        pBoxCol->m_RuntimeShape = shape;
    }

    // Sphere Collider
    if (nullptr != pSphereCol)
    {
        PxMaterial* pPxMtrl = DefaultPxMtrl;
        if (nullptr != pSphereCol->m_Mtrl)
        {
            pPxMtrl = m_Physics->createMaterial(pSphereCol->m_Mtrl->GetStaticFriction(), pSphereCol->m_Mtrl->GetDynamicFriction(),
                                                pSphereCol->m_Mtrl->GetBounciness());
        }

        PxShape* shape = PxRigidActorExt::createExclusiveShape(*RigidActor, PxSphereGeometry(WorldScale.x * pSphereCol->m_Radius * 2.f), *pPxMtrl);

        Vec3 Center = pSphereCol->GetCenter();
        PxTransform LocalPos = shape->getLocalPose();
        LocalPos.p.x = Center.x;
        LocalPos.p.y = Center.y;
        LocalPos.p.z = Center.z;
        shape->setLocalPose(LocalPos);

        shape->userData = (void*)pSphereCol;
        pSphereCol->m_RuntimeShape = shape;
    }

    RigidActor->userData = (void*)_GameObject;
    m_Scene->addActor(*RigidActor);
    m_vecPhysicsObj.push_back(_GameObject);
}

void CPhysicsMgr::RemovePhysicsObject(CGameObject* _GameObject)
{
    if (nullptr == m_Scene)
        return;

    for (UINT i = 0; i < m_vecPhysicsObj.size(); i++)
    {
        if (m_vecPhysicsObj[i] != _GameObject)
            continue;

        CRigidbody* pRigidbody = m_vecPhysicsObj[i]->Rigidbody();
        CBoxCollider* pBoxCol = m_vecPhysicsObj[i]->BoxCollider();
        CSphereCollider* pSphereCol = m_vecPhysicsObj[i]->SphereCollider();
        CCapsuleCollider* pCapsuleCol = m_vecPhysicsObj[i]->CapsuleCollider();

        PxRigidActor* RigidActor = nullptr;

        if (nullptr != pRigidbody)
            RigidActor = (PxRigidActor*)pRigidbody->m_RuntimeBody;
        else if (nullptr != pBoxCol)
            RigidActor = ((PxShape*)pBoxCol->m_RuntimeShape)->getActor();
        else if (nullptr != pSphereCol)
            RigidActor = ((PxShape*)pSphereCol->m_RuntimeShape)->getActor();
        else if (nullptr != pCapsuleCol)
            RigidActor = ((PxShape*)pCapsuleCol->m_RuntimeShape)->getActor();

        if (nullptr != RigidActor)
        {
            m_Scene->removeActor(*RigidActor);
        }

        m_vecPhysicsObj.erase(m_vecPhysicsObj.begin() + i);

        if (nullptr != pRigidbody)
            pRigidbody->m_RuntimeBody = nullptr;
        if (nullptr != pBoxCol)
            pBoxCol->m_RuntimeShape = nullptr;
        if (nullptr != pSphereCol)
            pSphereCol->m_RuntimeShape = nullptr;
        if (nullptr != pCapsuleCol)
            pCapsuleCol->m_RuntimeShape = nullptr;

        break;
    }
}
