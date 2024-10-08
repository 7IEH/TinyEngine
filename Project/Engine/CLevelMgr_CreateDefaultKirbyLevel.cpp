#include "pch.h"

#include "CRenderMgr.h"
#include "CLevelMgr.h"
#include <Scripts\CScriptMgr.h>
#include "CScript.h"
#include "CEngine.h"
#include "CLevel.h"
#include "CEditorMgr.h"
#include "CEditor.h"

#include "CLevelSaveLoad.h"
#include "components.h"

CLevel* CLevelMgr::CreateDefaultKirbyLevel()
{
    CLevel* NewLevel = new CLevel;
    NewLevel->SetName(L"Default Kirby Level");

    // =============
    // create layer
    // =============
    NewLevel->GetLayer(0)->SetName(L"Default");
    NewLevel->GetLayer(1)->SetName(L"Manager");
    NewLevel->GetLayer(2)->SetName(L"World Static");
    NewLevel->GetLayer(3)->SetName(L"World Dynamic");
    NewLevel->GetLayer(4)->SetName(L"Player");
    NewLevel->GetLayer(5)->SetName(L"Player Trigger");
    NewLevel->GetLayer(6)->SetName(L"Player Attack");
    NewLevel->GetLayer(7)->SetName(L"Player Attack Trigger");
    NewLevel->GetLayer(8)->SetName(L"Monster");
    NewLevel->GetLayer(9)->SetName(L"Monster Trigger");
    NewLevel->GetLayer(10)->SetName(L"Monster Attack");
    NewLevel->GetLayer(11)->SetName(L"Monster Attack Trigger");
    NewLevel->GetLayer(12)->SetName(L"World Static Trigger");
    NewLevel->GetLayer(13)->SetName(L"Layer 13");
    NewLevel->GetLayer(14)->SetName(L"Effect");
    NewLevel->GetLayer(15)->SetName(L"UI");

    // layer collision check
    CPhysicsMgr::GetInst()->DisableAllLayer();

    // World Static
    for (int i = 2; i <= 11; ++i)
    {
        CPhysicsMgr::GetInst()->LayerCheck(2, i, true);
        CPhysicsMgr::GetInst()->LayerCheck(3, i, true);
    }

    // Player Layer
    CPhysicsMgr::GetInst()->LayerCheck(4, 8, true);
    CPhysicsMgr::GetInst()->LayerCheck(4, 9, true);
    CPhysicsMgr::GetInst()->LayerCheck(4, 11, true);
    CPhysicsMgr::GetInst()->LayerCheck(4, 12, true);

    // Player Trigger
    CPhysicsMgr::GetInst()->LayerCheck(5, 6, true);
    CPhysicsMgr::GetInst()->LayerCheck(5, 8, true);
    CPhysicsMgr::GetInst()->LayerCheck(5, 10, true);

    // Player Attack
    CPhysicsMgr::GetInst()->LayerCheck(6, 9, true);

    // Player Attack Trigger
    CPhysicsMgr::GetInst()->LayerCheck(7, 8, true);

    for (int i = 0; i < LAYER_MAX; ++i)
    {
        CPhysicsMgr::GetInst()->LayerCheck(0, i, false);
        CPhysicsMgr::GetInst()->LayerCheck(1, i, false);
        CPhysicsMgr::GetInst()->LayerCheck(14, i, false);
        CPhysicsMgr::GetInst()->LayerCheck(15, i, false);
    }

    // =============
    // world setting
    // =============
    CPhysicsMgr::GetInst()->SetPPM(20.f);

    // Manager
    CGameObject* pManager = new CGameObject;
    pManager->SetName(L"Manager");
    pManager->AddComponent(new CTransform);
    pManager->AddComponent(CScriptMgr::GetScript(FLOWMGR_LV0));
    pManager->AddComponent(CScriptMgr::GetScript(PLAYERMGR));

    NewLevel->AddObject(pManager, 1);

    // =============
    // create camera
    // =============
    CGameObject* pCamObj = new CGameObject;
    pCamObj->SetName(L"Main Camera");
    pCamObj->AddComponent(new CTransform);
    pCamObj->AddComponent(new CMeshRender);
    pCamObj->AddComponent(new CCamera);

    pCamObj->MeshRender()->SetMesh(CAssetMgr::GetInst()->FindAsset<CMesh>(L"PointMesh"));
    pCamObj->MeshRender()->SetMaterial(CAssetMgr::GetInst()->FindAsset<CMaterial>(L"CameraIconMtrl"), 0);
    pCamObj->MeshRender()->SetFrustumCheck(false);
    pCamObj->MeshRender()->SetCastShadow(false);

    pCamObj->Camera()->SetProjType(PROJ_TYPE::PERSPECTIVE);
    pCamObj->Camera()->SetCameraPriority(0);
    pCamObj->Camera()->LayerMaskAll();
    pCamObj->Camera()->LayerMask(NewLevel, L"UI", false);
    pCamObj->Camera()->SetHDRI(true);
    pCamObj->Camera()->SetFOV(XM_PI / 4.f);

    pCamObj->AddComponent(CScriptMgr::GetScript(SCRIPT_TYPE::CAMERACONTROLLER));

    NewLevel->AddObject(pCamObj, 0);

    // UI Camera
    CGameObject* pUICamObj = new CGameObject;
    pUICamObj->SetName(L"UI Camera");
    pUICamObj->AddComponent(new CTransform);
    pUICamObj->AddComponent(new CMeshRender);
    pUICamObj->AddComponent(new CCamera);

    pUICamObj->MeshRender()->SetMesh(CAssetMgr::GetInst()->FindAsset<CMesh>(L"PointMesh"));
    pUICamObj->MeshRender()->SetMaterial(CAssetMgr::GetInst()->FindAsset<CMaterial>(L"CameraIconMtrl"), 0);
    pUICamObj->MeshRender()->SetFrustumCheck(false);
    pUICamObj->MeshRender()->SetCastShadow(false);

    pUICamObj->Camera()->SetCameraPriority(1);
    pUICamObj->Camera()->LayerMask(NewLevel, L"UI", true);

    NewLevel->AddObject(pUICamObj, 0);

    // Editor Camera
    CGameObject* EditCam = CRenderMgr::GetInst()->GetEditorCamera()->GetOwner();
    EditCam->Transform()->SetLocalPos(Vec3(0.f, 160.f, -100.f));
    EditCam->Transform()->SetLocalRotation(Vec3(XMConvertToRadians(40.f), 0.f, 0.f));

    // =============
    // create Env
    // =============
    // SkyBox
    CGameObject* pSkyBoxObj = nullptr;
    Ptr<CPrefab> SkyboxPref = CAssetMgr::GetInst()->Load<CPrefab>(L"prefab\\Level0_SkySphere.pref");

    if (SkyboxPref != nullptr)
    {
        pSkyBoxObj = SkyboxPref->Instantiate();
    }
    else
    {
        Ptr<CMeshData> SkyboxFbx = CAssetMgr::GetInst()->LoadFBX(L"fbx\\LevelObject\\Skybox\\Day\\Day.fbx");

        if (SkyboxFbx != nullptr)
        {
            pSkyBoxObj = SkyboxFbx->Instantiate();
        }
        else
        {
            pSkyBoxObj = new CGameObject;
            pSkyBoxObj->AddComponent(new CTransform);
            pSkyBoxObj->AddComponent(new CMeshRender);

            pSkyBoxObj->Transform()->SetWorldScale(Vec3(2000.f));
            pSkyBoxObj->MeshRender()->SetMesh(CAssetMgr::GetInst()->FindAsset<CMesh>(L"SphereMesh"));
        }

        pSkyBoxObj->Transform()->SetLocalRotation(Vec3(-XM_PIDIV2, XM_PI, 0.f));
        pSkyBoxObj->Transform()->SetLocalScale(Vec3(5.f, 5.f, 5.f));

        pSkyBoxObj->GetRenderComponent()->SetFrustumCheck(false);
        pSkyBoxObj->GetRenderComponent()->SetCastShadow(false);

        Ptr<CMaterial> SkyBoxMtrl = CAssetMgr::GetInst()->Load<CMaterial>(L"material\\Day_SkySphereDayC.mtrl");
        if (SkyBoxMtrl == nullptr)
        {
            SkyBoxMtrl = new CMaterial(false);
            SkyBoxMtrl->SetShader(CAssetMgr::GetInst()->Load<CGraphicsShader>(L"KirbySkySphereShader"));
            wstring name = L"material\\Day_SkySphereDayC.mtrl";
            SkyBoxMtrl->SetName(name);
            SkyBoxMtrl->SetTexParam(TEX_0, CAssetMgr::GetInst()->Load<CTexture>(L"fbx\\LevelObject\\Skybox\\Day\\SkySphere_DayTest.1151417570.png"));
            CAssetMgr::GetInst()->AddAsset(name, SkyBoxMtrl);
            // SkyBoxMtrl->Save(SkyBoxMtrl->GetKey());
        }

        pSkyBoxObj->GetRenderComponent()->SetMaterial(SkyBoxMtrl, 0);
    }

    pSkyBoxObj->SetName(L"SkyBox");
    NewLevel->AddObject(pSkyBoxObj, 0);

    // IBL
    CGameObject* pIBLObj = new CGameObject;
    pIBLObj->SetName(L"IBL");
    pIBLObj->AddComponent(new CTransform);
    pIBLObj->AddComponent(new CSkyBox);

    pIBLObj->GetRenderComponent()->SetFrustumCheck(false);
    pIBLObj->GetRenderComponent()->SetCastShadow(false);

    pIBLObj->SkyBox()->SetDiffuseTex(CAssetMgr::GetInst()->Load<CTexture>(L"fbx\\LevelObject\\Map\\Light\\Default\\Diffuse.dds"));
    pIBLObj->SkyBox()->SetSpecularTex(CAssetMgr::GetInst()->Load<CTexture>(L"fbx\\LevelObject\\Map\\Light\\Default\\Specular.dds"));

    NewLevel->AddObject(pIBLObj, 0);

    // Light
    CGameObject* pDirLight = new CGameObject;
    pDirLight->SetName(L"Directional Light");
    pDirLight->AddComponent(new CTransform);
    pDirLight->AddComponent(new CLight);

    pDirLight->Transform()->SetLocalPos(Vec3(0.f, 2000.f, 0.f));
    pDirLight->Transform()->SetLocalRotation(Vec3(XMConvertToRadians(90.f), 0.f, 0.f));
    pDirLight->Light()->SetLightType(LIGHT_TYPE::DIRECTIONAL);
    pDirLight->Light()->SetRadius(3.f);

    NewLevel->AddObject(pDirLight, 0);

    // Floor
    CGameObject* pFloor = new CGameObject;
    pFloor->SetName(L"Floor");
    pFloor->AddComponent(new CTransform);
    pFloor->AddComponent(new CMeshRender);
    pFloor->AddComponent(new CBoxCollider);

    pFloor->Transform()->SetLocalScale(Vec3(10000.f, 1.f, 10000.f));
    pFloor->MeshRender()->SetMesh(CAssetMgr::GetInst()->FindAsset<CMesh>(L"BoxMesh"));
    Ptr<CMaterial> pFloorMtrl = CAssetMgr::GetInst()->Load<CMaterial>(L"material\\DefaultFloorMtrl.mtrl");
    if (nullptr == pFloorMtrl)
    {
        pFloorMtrl = new CMaterial(false);
        pFloorMtrl->SetShader(CAssetMgr::GetInst()->FindAsset<CGraphicsShader>(L"UnrealPBRDeferredShader"));
        wstring name = L"material\\DefaultFloorMtrl.mtrl";
        pFloorMtrl->SetName(name);
        pFloorMtrl->SetAlbedo(Vec4(1.f, 1.f, 1.f, 1.f));
        CAssetMgr::GetInst()->AddAsset(name, pFloorMtrl);
        pFloorMtrl->Save(name);
    }
    pFloor->MeshRender()->SetMaterial(pFloorMtrl, 0);
    pFloor->MeshRender()->SetFrustumCheck(false);

    NewLevel->AddObject(pFloor, 2);

    // ==================
    // create main Player
    // ==================
    // Main Player
    Ptr<CPrefab> PlayerPref = CAssetMgr::GetInst()->Load<CPrefab>(L"prefab\\Kirby.pref");
    CGameObject* pPlayer = nullptr;

    if (PlayerPref != nullptr)
    {
        pPlayer = PlayerPref->Instantiate();
    }
    else
    {
        Ptr<CMeshData> PlayerFbx = CAssetMgr::GetInst()->LoadFBX(L"fbx\\Characters\\Kirby\\Base\\Kirby.fbx");

        if (PlayerFbx != nullptr)
        {
            pPlayer = PlayerFbx->Instantiate();

            if (pPlayer)
            {
                pPlayer->AddComponent(new CCharacterController);
                pPlayer->AddComponent(CScriptMgr::GetScript(KIRBYUNITSCRIPT));
                pPlayer->AddComponent(CScriptMgr::GetScript(KIRBYMOVECONTROLLER));
                pPlayer->AddComponent(CScriptMgr::GetScript(KIRBYFSM));

                pPlayer->Transform()->SetLocalScale(Vec3(20.f, 20.f, 20.f));
                pPlayer->Transform()->SetLocalPos(Vec3(0.f, 10.f, 10.f));
                pPlayer->MeshRender()->SetFrustumCheck(false);
                pPlayer->CharacterController()->SetCenter(Vec3(0.f, 0.77f, 0.f));
                pPlayer->CharacterController()->SetHeight(1.51f);
                pPlayer->CharacterController()->SetRadius(0.51f);
                pPlayer->CharacterController()->SetSkinWidth(0.015f);
                pPlayer->CharacterController()->SetMinMoveDistance(0.f);

                Ptr<CMaterial> pPlayerMtrl = pPlayer->GetRenderComponent()->GetMaterial(0);
                Ptr<CGraphicsShader> pPlayerShader = CAssetMgr::GetInst()->Load<CGraphicsShader>(L"KirbyBodyShader");

                for (UINT i = 0; i < pPlayer->GetRenderComponent()->GetMtrlCount(); ++i)
                {
                    wstring MtrlName = pPlayer->GetRenderComponent()->GetMesh()->GetIBName(i);

                    if (MtrlName != L"BodyM__BodyC" && MtrlName != L"limbsM__BodyC" && MtrlName != L"MouthNormalM__BodyC")
                    {
                        pPlayer->GetRenderComponent()->SetMaterial(nullptr, i);
                    }
                }

                if (pPlayerMtrl != nullptr && pPlayerShader != nullptr)
                {
                    pPlayerMtrl->SetShader(pPlayerShader);
                    pPlayerMtrl->SetTexParam(TEX_PARAM::TEX_0,
                                             CAssetMgr::GetInst()->Load<CTexture>(L"fbx\\Characters\\Kirby\\Base\\KirbyEye.00.png"));
                    pPlayerMtrl->SetTexParam(TEX_PARAM::TEX_1,
                                             CAssetMgr::GetInst()->Load<CTexture>(L"fbx\\Characters\\Kirby\\Base\\KirbyEyeMask.00.png"));
                    pPlayerMtrl->SetTexParam(TEX_PARAM::TEX_2,
                                             CAssetMgr::GetInst()->Load<CTexture>(L"fbx\\Characters\\Kirby\\Base\\KirbyEyeNormal.00.png"));
                    pPlayerMtrl->SetTexParam(TEX_PARAM::TEX_3,
                                             CAssetMgr::GetInst()->Load<CTexture>(L"fbx\\Characters\\Kirby\\Base\\KirbySkin.856436594.png"));
                    pPlayerMtrl->SetTexParam(TEX_PARAM::TEX_4,
                                             CAssetMgr::GetInst()->Load<CTexture>(L"fbx\\Characters\\Kirby\\Base\\KirbyMouth.1209505089.png"));
                }

                // Player Vacuum Collider
                CGameObject* pVacuumCol = new CGameObject;
                pVacuumCol->SetName(L"Vacuum Collider");
                pVacuumCol->AddComponent(new CTransform);
                pVacuumCol->AddComponent(new CSphereCollider);
                pVacuumCol->AddComponent(CScriptMgr::GetScript(KIRBYVACUUMCOLLIDER));

                pVacuumCol->Transform()->SetAbsolute(false);
                pVacuumCol->Transform()->SetLocalPos(Vec3(0.f, 2.f, 2.f));
                pVacuumCol->Transform()->SetLocalScale(Vec3(1.5f, 1.5f, 1.5f));
                pVacuumCol->SphereCollider()->SetTrigger(true);

                GamePlayStatic::AddChildObject(pPlayer, pVacuumCol);
                GamePlayStatic::LayerChange(pVacuumCol, 5);

                // Player Body Collider
                CGameObject* pBodyCollider = new CGameObject;
                pBodyCollider->SetName(L"Body Collider");
                pBodyCollider->AddComponent(new CTransform);
                pBodyCollider->AddComponent(new CCapsuleCollider);
                pBodyCollider->AddComponent(CScriptMgr::GetScript(KIRBYBODYCOLLIDER));

                pBodyCollider->Transform()->SetAbsolute(false);
                pBodyCollider->Transform()->SetLocalScale(Vec3(1.2f, 1.2f, 1.2f));
                pBodyCollider->CapsuleCollider()->SetTrigger(true);
                pBodyCollider->CapsuleCollider()->SetCenter(Vec3(0.f, 0.65f, 0.f));
                pBodyCollider->CapsuleCollider()->SetHeight(1.51f);
                pBodyCollider->CapsuleCollider()->SetRadius(0.51f);

                GamePlayStatic::AddChildObject(pPlayer, pBodyCollider);
                GamePlayStatic::LayerChange(pBodyCollider, 5);

                // DeformLight PointLight
                Ptr<CPrefab> pPointLightpref =
                    CAssetMgr::GetInst()->Load<CPrefab>(L"prefab\\DeformLight PointLight.pref", L"prefab\\DeformLight PointLight.pref");
                CGameObject* pPointLight;

                if (pPointLightpref != nullptr)
                {
                    pPointLight = pPointLightpref->Instantiate();
                }
                else
                {
                    pPointLight = new CGameObject;
                    pPointLight->SetName(L"DeformLight PointLight");
                    pPointLight->AddComponent(new CTransform);
                    pPointLight->AddComponent(new CLight);

                    pPointLight->Transform()->SetLocalPos(Vec3(0.f, 150.f, 0.f));
                    pPointLight->Transform()->SetLocalRotation(Vec3(XMConvertToRadians(-90.f), 0.f, 0.f));
                    pPointLight->Light()->SetLightType(LIGHT_TYPE::POINT);
                    pPointLight->Light()->SetLightRadiance(Vec3(255.f, 226.f, 217.f) / 255.f);
                    pPointLight->Light()->SetRadius(80.f);
                    pPointLight->Light()->SetFallOffEnd(135.f);
                    pPointLight->Light()->SetHaloRadius(160.f);
                    pPointLight->Light()->SetHaloStrength(0.125f);
                }
                GamePlayStatic::AddChildObject(pPlayer, pPointLight);
                pPointLight->SetActive(false);

                // Snot Bubble
                Ptr<CPrefab> pSnotBubblePref = CAssetMgr::GetInst()->Load<CPrefab>(L"prefab\\SleepSnotBubble.pref", L"prefab\\SleepSnotBubble.pref");
                CGameObject* pSnotBubble = nullptr;

                if (pSnotBubblePref != nullptr)
                {
                    pSnotBubble = pSnotBubblePref->Instantiate();
                }
                else
                {
                    pSnotBubble =
                        CAssetMgr::GetInst()->LoadFBX(L"fbx\\Characters\\Kirby\\Sleep\\SleepSnotBubble\\SleepSnotBubble.fbx")->Instantiate();
                    pSnotBubble->SetName(L"SleepSnotBubble");

                    pSnotBubble->MeshRender()->SetBoundingRadius(0.1f);
                    pSnotBubble->MeshRender()->SetCastShadow(false);

                    GamePlayStatic::SpawnGameObject(pSnotBubble, 4);
                }
                GamePlayStatic::AddChildObject(pPlayer, pSnotBubble, L"Mouth");
                pSnotBubble->SetActive(false);
            }
        }
        else
        {
            pPlayer = new CGameObject;
        }
    }

    pPlayer->SetName(L"Main Player");
    NewLevel->AddObject(pPlayer, 4);

    // ==================
    // create default object
    // ==================
    // Box
    Ptr<CMeshData> BoxFbx = CAssetMgr::GetInst()->LoadFBX(L"fbx\\LevelObject\\Gimmick\\Block\\StarBlock\\H2W2\\StarBlock_H2W2.fbx");
    CGameObject* pBox = nullptr;
    
    if (BoxFbx != nullptr)
    {
        pBox = BoxFbx->Instantiate();
        pBox->SetName(L"Star Block");
        pBox->AddComponent(new CRigidbody);
        pBox->AddComponent(new CBoxCollider);

        pBox->Transform()->SetLocalScale(Vec3(20.f, 20.f, 20.f));
        pBox->Transform()->SetLocalPos(Vec3(-100.f, 30.f, 100.f));
        pBox->BoxCollider()->SetCenter(Vec3(0.f, 1.f, 0.f));
        pBox->GetRenderComponent()->SetBoundingRadius(60.f);

        NewLevel->AddObject(pBox, 3);
    }

    // Monster
    Ptr<CPrefab> MonsterPref = CAssetMgr::GetInst()->Load<CPrefab>(L"prefab\\NormalEnemy.pref");
    CGameObject* pMonsterAbility = nullptr;

    if (MonsterPref != nullptr)
    {
        pMonsterAbility = MonsterPref->Instantiate();
    }
    else
    {
        Ptr<CMeshData> MonsterFbx = CAssetMgr::GetInst()->LoadFBX(L"fbx\\Characters\\Monster\\NormalEnemy\\NormalEnemy.fbx");
        
        if (MonsterFbx != nullptr)
        {
            pMonsterAbility = MonsterPref->Instantiate();
            pMonsterAbility->AddComponent(new CRigidbody);
            pMonsterAbility->AddComponent(new CCapsuleCollider);

            pMonsterAbility->MeshRender()->GetMaterial(0)->SetShader(CAssetMgr::GetInst()->Load<CGraphicsShader>(L"NormalEnemyBodyShader"));
            pMonsterAbility->MeshRender()->GetMaterial(0)->SetTexParam(
                TEX_0, CAssetMgr::GetInst()->Load<CTexture>(L"fbx\\Characters\\Monster\\NormalEnemy\\NormalEnemyEye.00.png"));
            pMonsterAbility->MeshRender()->GetMaterial(0)->SetTexParam(
                TEX_1, CAssetMgr::GetInst()->Load<CTexture>(L"fbx\\Characters\\Monster\\NormalEnemy\\BodyC_BaseColor._919281726.png"));
            pMonsterAbility->MeshRender()->GetMaterial(0)->SetTexParam(
                TEX_2, CAssetMgr::GetInst()->Load<CTexture>(L"fbx\\Characters\\Monster\\NormalEnemy\\BodyC_Normal.1361449125.png"));
            pMonsterAbility->MeshRender()->GetMaterial(0)->SetTexParam(
                TEX_3, CAssetMgr::GetInst()->Load<CTexture>(L"fbx\\Characters\\Monster\\NormalEnemy\\BodyC_MRA.711223188.png"));
        }
    }

    if (pMonsterAbility)
    {
        pMonsterAbility->AddComponent(CScriptMgr::GetScript(KIRBYCOPYABILITYSCRIPT));

        pMonsterAbility->Transform()->SetLocalPos(Vec3(60.f, 20.f, 60.f));
        pMonsterAbility->Transform()->SetLocalRotation(Vec3(0.f, XMConvertToRadians(180.f), 0.f));
        pMonsterAbility->Transform()->SetLocalScale(Vec3(20.f, 20.f, 20.f));

        pMonsterAbility->SetName(L"Monster (Ability)");
        NewLevel->AddObject(pMonsterAbility, 8);
    }

    return NewLevel;
}

/*********************
|   DefaultUILevel
|   1. StartLevel
|   2. RobbyLevel
|   3. PlayLevel
*********************/

CLevel* CLevelMgr::CreateDefaultStartUILevel()
{
    CLevel* NewLevel = new CLevel;
    NewLevel->SetName(L"Default Kirby Start UI Level");

    // =============
    // create layer
    // =============
    NewLevel->GetLayer(0)->SetName(L"Default");
    NewLevel->GetLayer(1)->SetName(L"Manager");
    NewLevel->GetLayer(2)->SetName(L"World Static");
    NewLevel->GetLayer(3)->SetName(L"World Dynamic");
    NewLevel->GetLayer(4)->SetName(L"Player");
    NewLevel->GetLayer(5)->SetName(L"Player Trigger");
    NewLevel->GetLayer(6)->SetName(L"Player Attack");
    NewLevel->GetLayer(7)->SetName(L"Player Attack Trigger");
    NewLevel->GetLayer(8)->SetName(L"Monster");
    NewLevel->GetLayer(9)->SetName(L"Monster Trigger");
    NewLevel->GetLayer(10)->SetName(L"Monster Attack");
    NewLevel->GetLayer(11)->SetName(L"Monster Attack Trigger");
    NewLevel->GetLayer(12)->SetName(L"Layer 12");
    NewLevel->GetLayer(13)->SetName(L"Layer 13");
    NewLevel->GetLayer(14)->SetName(L"Effect");
    NewLevel->GetLayer(15)->SetName(L"UI");

    // =============
    // world setting
    // =============
    CPhysicsMgr::GetInst()->SetPPM(100.f);

    // Manager
    CGameObject* pManager = new CGameObject;
    pManager->SetName(L"Manager");
    pManager->AddComponent(new CTransform);
    pManager->AddComponent(CScriptMgr::GetScript(UIMANAGERSCRIPT));

    NewLevel->AddObject(pManager, 1);

    // =============
    // create camera
    // =============
    CGameObject* pCamObj = new CGameObject;
    pCamObj->SetName(L"Main Camera");
    pCamObj->AddComponent(new CTransform);
    pCamObj->AddComponent(new CMeshRender);
    pCamObj->AddComponent(new CCamera);

    pCamObj->Transform()->SetLocalRotation(Vec3(XMConvertToRadians(-68.5f), XMConvertToRadians(-62.f), 0.f));

    pCamObj->MeshRender()->SetMesh(CAssetMgr::GetInst()->FindAsset<CMesh>(L"PointMesh"));
    pCamObj->MeshRender()->SetMaterial(CAssetMgr::GetInst()->FindAsset<CMaterial>(L"CameraIconMtrl"), 0);
    pCamObj->MeshRender()->SetFrustumCheck(false);
    pCamObj->MeshRender()->SetCastShadow(false);

    pCamObj->Camera()->SetProjType(PROJ_TYPE::PERSPECTIVE);
    pCamObj->Camera()->SetCameraPriority(0);
    pCamObj->Camera()->LayerMaskAll();
    pCamObj->Camera()->LayerMask(NewLevel, L"UI", false);
    pCamObj->Camera()->SetHDRI(true);
    pCamObj->Camera()->SetFOV(XMConvertToRadians(60.f));

    NewLevel->AddObject(pCamObj, 0);

    // UI Camera
    CGameObject* pUICamObj = new CGameObject;
    pUICamObj->SetName(L"UI Camera");
    pUICamObj->AddComponent(new CTransform);
    pUICamObj->AddComponent(new CMeshRender);
    pUICamObj->AddComponent(new CCamera);

    pUICamObj->Transform()->SetLocalPos(Vec3(10000.f, 0.f, -5.f));

    pUICamObj->MeshRender()->SetMesh(CAssetMgr::GetInst()->FindAsset<CMesh>(L"PointMesh"));
    pUICamObj->MeshRender()->SetMaterial(CAssetMgr::GetInst()->FindAsset<CMaterial>(L"CameraIconMtrl"), 0);
    pUICamObj->MeshRender()->SetFrustumCheck(false);
    pUICamObj->MeshRender()->SetCastShadow(false);

    pUICamObj->Camera()->SetProjType(PROJ_TYPE::PERSPECTIVE);
    pUICamObj->Camera()->SetCameraPriority(1);
    pUICamObj->Camera()->LayerMask(NewLevel, L"UI", true);
    pUICamObj->Camera()->SetHDRI(true);
    pUICamObj->Camera()->SetFOV(XMConvertToRadians(90.f));

    NewLevel->AddObject(pUICamObj, 0);

    // =============
    // create Env
    // =============
    // SkyBox
    CGameObject* pSkyBoxObj = CAssetMgr::GetInst()->LoadFBX(L"fbx\\LevelObject\\Skybox\\Day\\Day.fbx")->Instantiate();
    pSkyBoxObj->SetName(L"SkyBox");

    pSkyBoxObj->Transform()->SetLocalRotation(Vec3(-XM_PIDIV2, XM_PI, 0.f));
    pSkyBoxObj->Transform()->SetLocalScale(Vec3(5.f, 5.f, 5.f));

    pSkyBoxObj->GetRenderComponent()->SetFrustumCheck(false);
    pSkyBoxObj->GetRenderComponent()->SetCastShadow(false);

    Ptr<CMaterial> SkyBoxMtrl = pSkyBoxObj->GetRenderComponent()->GetMaterial(0);
    SkyBoxMtrl->SetShader(CAssetMgr::GetInst()->FindAsset<CGraphicsShader>(L"KirbySkySphereShader"));
    SkyBoxMtrl->SetTexParam(TEX_0, CAssetMgr::GetInst()->FindAsset<CTexture>(L"fbx\\LevelObject\\Skybox\\Day\\SkySphere_DayTest.1151417570.png"));
    SkyBoxMtrl->Save(SkyBoxMtrl->GetKey());

    NewLevel->AddObject(pSkyBoxObj, 0);

    // IBL
    CGameObject* pIBLObj = new CGameObject;
    pIBLObj->SetName(L"IBL");
    pIBLObj->AddComponent(new CTransform);
    pIBLObj->AddComponent(new CSkyBox);

    pSkyBoxObj->GetRenderComponent()->SetFrustumCheck(false);
    pSkyBoxObj->GetRenderComponent()->SetCastShadow(false);

    pIBLObj->SkyBox()->SetDiffuseTex(CAssetMgr::GetInst()->FindAsset<CTexture>(L"fbx\\LevelObject\\Map\\Light\\Default\\Diffuse.dds"));
    pIBLObj->SkyBox()->SetSpecularTex(CAssetMgr::GetInst()->FindAsset<CTexture>(L"fbx\\LevelObject\\Map\\Light\\Default\\Specular.dds"));

    NewLevel->AddObject(pIBLObj, 0);

    // Light
    CGameObject* pDirLight = new CGameObject;
    pDirLight->SetName(L"Directional Light");
    pDirLight->AddComponent(new CTransform);
    pDirLight->AddComponent(new CLight);

    pDirLight->Transform()->SetLocalPos(Vec3(0.f, 2000.f, -2000.f));
    pDirLight->Transform()->SetLocalRotation(Vec3(XMConvertToRadians(45.f), 0.f, 0.f));
    pDirLight->Light()->SetLightType(LIGHT_TYPE::DIRECTIONAL);
    pDirLight->Light()->SetRadius(10.f);

    NewLevel->AddObject(pDirLight, 0);

    // UI_TitleLogo
    {
        CGameObject* pUIObject = new CGameObject;
        pUIObject->SetName(L"UI_TitleLogo");
        pUIObject->AddComponent(new CTransform);
        pUIObject->AddComponent(new CMeshRender);

        pUIObject->Transform()->SetLocalPos(Vec3(9582.f, 44.f, 514.f));
        pUIObject->Transform()->SetLocalScale(Vec3(992.f, 634.f, 1.f));

        pUIObject->MeshRender()->SetMesh(CAssetMgr::GetInst()->FindAsset<CMesh>(L"RectMesh"));
        pUIObject->MeshRender()->SetMaterial(CAssetMgr::GetInst()->FindAsset<CMaterial>(L"material\\UI_TitleLogoMtrl.mtrl"), 0);

        NewLevel->AddObject(pUIObject, 15);
    }

    // UI_NintendoCompanyLogo
    {
        CGameObject* pUIObject = new CGameObject;
        pUIObject->SetName(L"UI_NintendoCompanyLogo");
        pUIObject->AddComponent(new CTransform);
        pUIObject->AddComponent(new CMeshRender);

        pUIObject->Transform()->SetLocalPos(Vec3(9331.f, 402.f, 445.f));
        pUIObject->Transform()->SetLocalScale(Vec3(174.f, 44.f, 1.f));

        pUIObject->MeshRender()->SetMesh(CAssetMgr::GetInst()->FindAsset<CMesh>(L"RectMesh"));
        pUIObject->MeshRender()->SetMaterial(CAssetMgr::GetInst()->FindAsset<CMaterial>(L"material\\UI_CompanyLogoMtrl_Nin.mtrl"), 0);

        NewLevel->AddObject(pUIObject, 15);
    }

    // UI_HalCompanyLogo
    {
        CGameObject* pUIObject = new CGameObject;
        pUIObject->SetName(L"UI_HalCompanyLogo");
        pUIObject->AddComponent(new CTransform);
        pUIObject->AddComponent(new CMeshRender);

        pUIObject->Transform()->SetLocalPos(Vec3(10606.f, 341.f, 394.f));
        pUIObject->Transform()->SetLocalScale(Vec3(122.f, 102.f, 1.f));

        pUIObject->MeshRender()->SetMesh(CAssetMgr::GetInst()->FindAsset<CMesh>(L"RectMesh"));
        pUIObject->MeshRender()->SetMaterial(CAssetMgr::GetInst()->FindAsset<CMaterial>(L"material\\UI_CompanyLogoMtrl.mtrl"), 0);

        NewLevel->AddObject(pUIObject, 15);
    }

    // TODO : Prefab
    // UI_StartBtn
    {
        CGameObject* pUIObject = CAssetMgr::GetInst()->FindAsset<CPrefab>(L"prefab\\UI_StartBtn.pref")->Instantiate();
        pUIObject->SetName(L"UI_StartBtn");

        pUIObject->Transform()->SetLocalPos(Vec3(10529.f, 35.f, 850.f));
        pUIObject->Transform()->SetLocalRotation(Vec3(0.f, XMConvertToRadians(5.f), 0.f));
        pUIObject->Transform()->SetLocalScale(Vec3(600.f, 150.f, 1.f));

        NewLevel->AddObject(pUIObject, 15);
    }

    // TODO : Prefab
    // UI_ExitBtn
    {
        CGameObject* pUIObject = CAssetMgr::GetInst()->FindAsset<CPrefab>(L"prefab\\UI_ExitBtn.pref")->Instantiate();
        pUIObject->SetName(L"UI_ExitBtn");

        pUIObject->Transform()->SetLocalPos(Vec3(10529.f, -183.f, 850.f));
        pUIObject->Transform()->SetLocalRotation(Vec3(0.f, XMConvertToRadians(2.12f), 0.f));
        pUIObject->Transform()->SetLocalScale(Vec3(600.f, 150.f, 1.f));

        NewLevel->AddObject(pUIObject, 15);
    }

    return NewLevel;
}

CLevel* CLevelMgr::CreateDefaultRobbyUILevel()
{
    CLevel* NewLevel = new CLevel;
    NewLevel->SetName(L"Default Kirby Robby UI Level");

    // =============
    // create layer
    // =============
    NewLevel->GetLayer(0)->SetName(L"Default");
    NewLevel->GetLayer(1)->SetName(L"Manager");
    NewLevel->GetLayer(2)->SetName(L"World Static");
    NewLevel->GetLayer(3)->SetName(L"World Dynamic");
    NewLevel->GetLayer(4)->SetName(L"Player");
    NewLevel->GetLayer(5)->SetName(L"Player Trigger");
    NewLevel->GetLayer(6)->SetName(L"Player Attack");
    NewLevel->GetLayer(7)->SetName(L"Player Attack Trigger");
    NewLevel->GetLayer(8)->SetName(L"Monster");
    NewLevel->GetLayer(9)->SetName(L"Monster Trigger");
    NewLevel->GetLayer(10)->SetName(L"Monster Attack");
    NewLevel->GetLayer(11)->SetName(L"Monster Attack Trigger");
    NewLevel->GetLayer(12)->SetName(L"Layer 12");
    NewLevel->GetLayer(13)->SetName(L"Layer 13");
    NewLevel->GetLayer(14)->SetName(L"Effect");
    NewLevel->GetLayer(15)->SetName(L"UI");

    // =============
    // world setting
    // =============
    CPhysicsMgr::GetInst()->SetPPM(100.f);

    // Manager
    CGameObject* pManager = new CGameObject;
    pManager->SetName(L"Manager");
    pManager->AddComponent(new CTransform);
    pManager->AddComponent(CScriptMgr::GetScript(UIMANAGERSCRIPT));

    NewLevel->AddObject(pManager, 1);

    // =============
    // create camera
    // =============
    CGameObject* pCamObj = new CGameObject;
    pCamObj->SetName(L"Main Camera");
    pCamObj->AddComponent(new CTransform);
    pCamObj->AddComponent(new CMeshRender);
    pCamObj->AddComponent(new CCamera);

    pCamObj->MeshRender()->SetMesh(CAssetMgr::GetInst()->FindAsset<CMesh>(L"PointMesh"));
    pCamObj->MeshRender()->SetMaterial(CAssetMgr::GetInst()->FindAsset<CMaterial>(L"CameraIconMtrl"), 0);
    pCamObj->MeshRender()->SetFrustumCheck(false);
    pCamObj->MeshRender()->SetCastShadow(false);

    pCamObj->Camera()->SetProjType(PROJ_TYPE::PERSPECTIVE);
    pCamObj->Camera()->SetCameraPriority(0);
    pCamObj->Camera()->LayerMaskAll();
    pCamObj->Camera()->LayerMask(NewLevel, L"UI", false);
    pCamObj->Camera()->SetHDRI(true);
    pCamObj->Camera()->SetFOV(XMConvertToRadians(60.f));

    NewLevel->AddObject(pCamObj, 0);

    // UI Camera
    CGameObject* pUICamObj = new CGameObject;
    pUICamObj->SetName(L"UI Camera");
    pUICamObj->AddComponent(new CTransform);
    pUICamObj->AddComponent(new CMeshRender);
    pUICamObj->AddComponent(new CCamera);

    pUICamObj->Transform()->SetLocalPos(Vec3(10000.f, 0.f, -5.f));

    pUICamObj->MeshRender()->SetMesh(CAssetMgr::GetInst()->FindAsset<CMesh>(L"PointMesh"));
    pUICamObj->MeshRender()->SetMaterial(CAssetMgr::GetInst()->FindAsset<CMaterial>(L"CameraIconMtrl"), 0);
    pUICamObj->MeshRender()->SetFrustumCheck(false);
    pUICamObj->MeshRender()->SetCastShadow(false);

    pUICamObj->Camera()->SetProjType(PROJ_TYPE::PERSPECTIVE);
    pUICamObj->Camera()->SetCameraPriority(1);
    pUICamObj->Camera()->LayerMask(NewLevel, L"UI", true);
    pUICamObj->Camera()->SetHDRI(true);
    pUICamObj->Camera()->SetFOV(XMConvertToRadians(90.f));

    NewLevel->AddObject(pUICamObj, 0);

    // Light
    CGameObject* pDirLight = new CGameObject;
    pDirLight->SetName(L"Directional Light");
    pDirLight->AddComponent(new CTransform);
    pDirLight->AddComponent(new CLight);

    pDirLight->Transform()->SetLocalPos(Vec3(0.f, 2000.f, -2000.f));
    pDirLight->Transform()->SetLocalRotation(Vec3(XMConvertToRadians(45.f), 0.f, 0.f));
    pDirLight->Light()->SetLightType(LIGHT_TYPE::DIRECTIONAL);
    pDirLight->Light()->SetRadius(10.f);

    NewLevel->AddObject(pDirLight, 0);

    // TODO : Prefab
    // UI_RobbyBG1
    {
        CGameObject* pUIObject = new CGameObject;
        pUIObject->SetName(L"UI_RobbyBG1");
        pUIObject->AddComponent(new CTransform);
        pUIObject->AddComponent(new CMeshRender);
        pUIObject->AddComponent(CScriptMgr::GetScript(UITEXSCRIPT));

        pUIObject->Transform()->SetLocalPos(Vec3(10000.f, 0.f, 1005.f));
        pUIObject->Transform()->SetLocalRotation(Vec3(0.f, 0.f, 0.f));
        pUIObject->Transform()->SetLocalScale(Vec3(3840.f, 2160.f, 1.f));

        pUIObject->MeshRender()->SetMesh(CAssetMgr::GetInst()->FindAsset<CMesh>(L"RectMesh"));
        pUIObject->MeshRender()->SetMaterial(CAssetMgr::GetInst()->FindAsset<CMaterial>(L"material\\UI_RobbyBGMtrl1.mtrl"), 0);

        NewLevel->AddObject(pUIObject, 15);
    }

    // TODO : Prefab
    // UI_StageNameBG
    {
        CGameObject* pUIObject = new CGameObject;
        pUIObject->SetName(L"UI_StageNameBG");
        pUIObject->AddComponent(new CTransform);
        pUIObject->AddComponent(new CMeshRender);
        pUIObject->AddComponent(CScriptMgr::GetScript(UITEXSCRIPT));

        pUIObject->Transform()->SetLocalPos(Vec3(9806.f, 279.f, 520.f));
        pUIObject->Transform()->SetLocalRotation(Vec3(XMConvertToRadians(2.62f), XMConvertToRadians(-8.14f), XMConvertToRadians(2.09f)));
        pUIObject->Transform()->SetLocalScale(Vec3(1424.f, 396.f, 1.f));

        pUIObject->MeshRender()->SetMesh(CAssetMgr::GetInst()->FindAsset<CMesh>(L"RectMesh"));
        pUIObject->MeshRender()->SetMaterial(CAssetMgr::GetInst()->FindAsset<CMaterial>(L"material\\UI_StageNameBgMtrl.mtrl"), 0);

        NewLevel->AddObject(pUIObject, 15);
    }

    // TODO : Prefab
    // UI_StageNameBG1
    {
        CGameObject* pUIObject = new CGameObject;
        pUIObject->SetName(L"UI_StageNameBG1");
        pUIObject->AddComponent(new CTransform);
        pUIObject->AddComponent(new CMeshRender);
        pUIObject->AddComponent(CScriptMgr::GetScript(UITEXSCRIPT));

        pUIObject->Transform()->SetLocalPos(Vec3(9382.f, 252.f, 449.f));
        pUIObject->Transform()->SetLocalRotation(Vec3(XMConvertToRadians(2.62f), XMConvertToRadians(-8.14f), XMConvertToRadians(2.09f)));
        pUIObject->Transform()->SetLocalScale(Vec3(396.f, 396.f, 1.f));

        pUIObject->MeshRender()->SetMesh(CAssetMgr::GetInst()->FindAsset<CMesh>(L"RectMesh"));
        pUIObject->MeshRender()->SetMaterial(CAssetMgr::GetInst()->FindAsset<CMaterial>(L"material\\UI_StageNameBgMtrl1.mtrl"), 0);

        NewLevel->AddObject(pUIObject, 15);
    }

    // TODO : Prefab
    // UI_StageNameStartBtn
    {
        CGameObject* pUIObject = new CGameObject;
        pUIObject->SetName(L"UI_StageNameStartBtn");
        pUIObject->AddComponent(new CTransform);
        pUIObject->AddComponent(new CMeshRender);
        pUIObject->AddComponent(CScriptMgr::GetScript(STAGECHANGEBUTTONSCRIPT));

        pUIObject->Transform()->SetLocalPos(Vec3(10690.f, -429.f, 605.f));
        pUIObject->Transform()->SetLocalRotation(Vec3(0.f, XMConvertToRadians(2.12f), 0.f));
        pUIObject->Transform()->SetLocalScale(Vec3(530.f, 174.f, 1.f));

        pUIObject->MeshRender()->SetMesh(CAssetMgr::GetInst()->FindAsset<CMesh>(L"RectMesh"));
        pUIObject->MeshRender()->SetMaterial(CAssetMgr::GetInst()->FindAsset<CMaterial>(L"material\\UI_StageNameStartBtn.mtrl"), 0);

        NewLevel->AddObject(pUIObject, 15);
    }

    return NewLevel;
}

CLevel* CLevelMgr::CreateDefaultPlayUILevel()
{
    CLevel* NewLevel = CreateDefaultKirbyLevel();

    NewLevel->SetName(L"Create Default Play UI Level");

    // Monster
    CGameObject* pMonsterAbility = CAssetMgr::GetInst()->LoadFBX(L"fbx\\Characters\\Monster\\NormalEnemy\\NormalEnemy.fbx")->Instantiate();
    pMonsterAbility->SetName(L"Monster (Track)");
    pMonsterAbility->AddComponent(new CRigidbody);
    pMonsterAbility->AddComponent(new CCapsuleCollider);
    pMonsterAbility->AddComponent(CScriptMgr::GetScript(NORMALENEMYSCRIPT));

    pMonsterAbility->Transform()->SetLocalPos(Vec3(280.f, 100.f, 280.f));
    pMonsterAbility->Transform()->SetLocalRotation(Vec3(0.f, XMConvertToRadians(180.f), 0.f));
    pMonsterAbility->Transform()->SetLocalScale(Vec3(100.f, 100.f, 100.f));

    pMonsterAbility->MeshRender()->GetMaterial(0)->SetShader(CAssetMgr::GetInst()->FindAsset<CGraphicsShader>(L"NormalEnemyBodyShader"));
    pMonsterAbility->MeshRender()->GetMaterial(0)->SetTexParam(
        TEX_0, CAssetMgr::GetInst()->FindAsset<CTexture>(L"fbx\\Characters\\Monster\\NormalEnemy\\NormalEnemyEye.00.png"));
    pMonsterAbility->MeshRender()->GetMaterial(0)->SetTexParam(
        TEX_1, CAssetMgr::GetInst()->FindAsset<CTexture>(L"fbx\\Characters\\Monster\\NormalEnemy\\BodyC_BaseColor._919281726.png"));
    pMonsterAbility->MeshRender()->GetMaterial(0)->SetTexParam(
        TEX_2, CAssetMgr::GetInst()->FindAsset<CTexture>(L"fbx\\Characters\\Monster\\NormalEnemy\\BodyC_Normal.1361449125.png"));
    pMonsterAbility->MeshRender()->GetMaterial(0)->SetTexParam(
        TEX_3, CAssetMgr::GetInst()->FindAsset<CTexture>(L"fbx\\Characters\\Monster\\NormalEnemy\\BodyC_MRA.711223188.png"));

    NewLevel->AddObject(pMonsterAbility, 8);

    // TODO : Prefab
    // UI_PlayerHP
    {
        CGameObject* pUIObject = new CGameObject;
        pUIObject->SetName(L"UI_PlayerHP");
        pUIObject->AddComponent(new CTransform);
        pUIObject->AddComponent(new CMeshRender);
        pUIObject->AddComponent(CScriptMgr::GetScript(STAGECHANGEBUTTONSCRIPT));

        pUIObject->Transform()->SetLocalPos(Vec3(10690.f, -429.f, 605.f));
        pUIObject->Transform()->SetLocalRotation(Vec3(0.f, XMConvertToRadians(2.12f), 0.f));
        pUIObject->Transform()->SetLocalScale(Vec3(530.f, 174.f, 1.f));

        pUIObject->MeshRender()->SetMesh(CAssetMgr::GetInst()->FindAsset<CMesh>(L"RectMesh"));
        pUIObject->MeshRender()->SetMaterial(CAssetMgr::GetInst()->FindAsset<CMaterial>(L"material\\UI_StageNameStartBtn.mtrl"), 0);

        NewLevel->AddObject(pUIObject, 15);
    }

    return NewLevel;
}
