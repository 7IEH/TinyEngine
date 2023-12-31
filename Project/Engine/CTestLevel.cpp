#include "pch.h"
#include "CTestLevel.h"

#include "CGameObject.h"
#include "CTransform.h"
#include "CMeshRender.h"
#include "CAssetMgr.h"
#include "CPlayerScript.h"
#include "CLight3D.h"
#include "CCamera.h"
#include "CCameraMoveScript.h"
#include "CLevelMgr.h"
#include "CRenderMgr.h"

CTestLevel::CTestLevel()
{
}

CTestLevel::~CTestLevel()
{
}

void CTestLevel::begin()
{
    // Camera Object ����
    CGameObject* pCamObj = new CGameObject;
    pCamObj->SetName(L"Main Camera");
    pCamObj->AddComponent(new CTransform);
    pCamObj->AddComponent(new CCamera);
    pCamObj->AddComponent(new CCameraMoveScript);

    pCamObj->Transform()->SetRelativePos(Vec3(0.f, 0.f, -100.f));
    pCamObj->Transform()->SetRelativeRotation(Vec3(0.f, 0.f, 0.f));

    pCamObj->Camera()->SetCameraPriority(0);
    pCamObj->Camera()->LayerCheckAll();
    pCamObj->Camera()->LayerCheck(L"UI", false);

    AddObject(pCamObj, L"Camera");

    // UI �� ������
    pCamObj = new CGameObject;
    pCamObj->SetName(L"UI Camera");
    pCamObj->AddComponent(new CTransform);
    pCamObj->AddComponent(new CCamera);

    pCamObj->Transform()->SetRelativePos(Vec3(0.5f, 0.f, 0.f));
    pCamObj->Transform()->SetRelativeRotation(Vec3(0.f, 0.f, 0.f));

    pCamObj->Camera()->SetCameraPriority(1);
    pCamObj->Camera()->LayerCheck(L"UI", true);

    AddObject(pCamObj, L"Camera");

    // Lights
    CGameObject* pLights = new CGameObject;
    pLights->SetName(L"Lights");

    pLights->AddComponent(new CTransform);
    pLights->AddComponent(new CMeshRender);

    pLights->Transform()->SetRelativePos(Vec3(0.f, 300.f, 200.f));
    pLights->Transform()->SetRelativeScale(Vec3(10.f, 10.f, 10.f));

    CGameObject* pLight1 = new CGameObject;
    pLight1->SetName(L"Light 1");

    pLight1->AddComponent(new CTransform);
    pLight1->AddComponent(new CMeshRender);
    pLight1->AddComponent(new CLight3D(LIGHT_TYPE::POINT, 0));
    pLight1->Light3D()->SetColor(Vec3(1.f, 1.f, 1.f));

    pLight1->Transform()->SetRelativePos(Vec3(-750.f, 0.f, 100.f));
    pLight1->Transform()->SetRelativeScale(Vec3(10.f, 10.f, 10.f));

    pLight1->MeshRender()->SetMesh(CAssetMgr::GetInst()->FindAsset<CMesh>(L"SphereMesh"));
    pLight1->MeshRender()->SetMaterial(CAssetMgr::GetInst()->FindAsset<CMaterial>(L"Basic"));
    pLights->AddChild(pLight1);

    CGameObject* pLight2 = new CGameObject;
    pLight2->SetName(L"Light 2");

    pLight2->AddComponent(new CTransform);
    pLight2->AddComponent(new CMeshRender);
    pLight2->AddComponent(new CLight3D(LIGHT_TYPE::POINT, 1));
    pLight2->Light3D()->SetColor(Vec3(1.f, 1.f, 1.f));

    pLight2->Transform()->SetRelativePos(Vec3(0.f, 0.f, 100.f));
    pLight2->Transform()->SetRelativeScale(Vec3(10.f, 10.f, 10.f));

    pLight2->MeshRender()->SetMesh(CAssetMgr::GetInst()->FindAsset<CMesh>(L"SphereMesh"));
    pLight2->MeshRender()->SetMaterial(CAssetMgr::GetInst()->FindAsset<CMaterial>(L"Basic"));
    pLights->AddChild(pLight2);

    CGameObject* pLight3 = new CGameObject;
    pLight3->SetName(L"Light 3");

    pLight3->AddComponent(new CTransform);
    pLight3->AddComponent(new CMeshRender);
    pLight3->AddComponent(new CLight3D(LIGHT_TYPE::POINT, 2));
    pLight3->Light3D()->SetColor(Vec3(1.f, 1.f, 1.f));

    pLight3->Transform()->SetRelativePos(Vec3(750.f, 0.f, 100.f));
    pLight3->Transform()->SetRelativeScale(Vec3(10.f, 10.f, 10.f));

    pLight3->MeshRender()->SetMesh(CAssetMgr::GetInst()->FindAsset<CMesh>(L"SphereMesh"));
    pLight3->MeshRender()->SetMaterial(CAssetMgr::GetInst()->FindAsset<CMaterial>(L"Basic"));
    pLights->AddChild(pLight3);

    AddObject(pLights, L"Light");

    AddMeshes();

    // AddModels();

    // SkyBox
    CGameObject* pSkyBox = new CGameObject;
    pSkyBox->SetName(L"SkyBox");

    pSkyBox->AddComponent(new CTransform);
    pSkyBox->AddComponent(new CMeshRender);

    pSkyBox->Transform()->SetRelativePos(Vec3(0.f, 0.f, 0.f));
    pSkyBox->Transform()->SetRelativeScale(Vec3(5000.f, 5000.f, 5000.f));
    pSkyBox->Transform()->SetAbsolute(true);

    pSkyBox->MeshRender()->SetMesh(CAssetMgr::GetInst()->FindAsset<CMesh>(L"SphereMesh"));
    pSkyBox->MeshRender()->SetMaterial(CAssetMgr::GetInst()->FindAsset<CMaterial>(L"Skybox"));

    AddObject(pSkyBox, L"SkyBox");

    CGameObject* pObj = new CGameObject;
    pObj->SetName(L"UI");

    pObj->AddComponent(new CTransform);
    pObj->AddComponent(new CMeshRender);

    pObj->Transform()->SetRelativePos(Vec3(-590, 310.f, 500.f));
    pObj->Transform()->SetRelativeScale(Vec3(50.f, 50.f, 1.f));

    pObj->MeshRender()->SetMesh(CAssetMgr::GetInst()->FindAsset<CMesh>(L"RectMesh"));
    pObj->MeshRender()->SetMaterial(CAssetMgr::GetInst()->FindAsset<CMaterial>(L"Std2DMtrl"));

    AddObject(pObj, L"UI", false);

    GamePlayStatic::DrawDebugRect(Vec3(0.f, 0.f, 0.f), Vec3(200.f, 200.f, 1.f), Vec3(0.f, 0.f, 0.f),
                                  Vec3(1.f, 1.f, 1.f), true, 20);

    CLevel::begin();
}

void CTestLevel::tick()
{
    CLevel::tick();
}

void CTestLevel::finaltick()
{
    CLevel::finaltick();
}

void CTestLevel::AddMeshes()
{
    // Meshes
    CGameObject* pMeshes = new CGameObject;
    pMeshes->SetName(L"Meshes");

    pMeshes->AddComponent(new CTransform);
    pMeshes->AddComponent(new CMeshRender);
    pMeshes->AddComponent(new CPlayerScript);

    pMeshes->Transform()->SetRelativePos(Vec3(0.f, 0.f, 0.f));
    pMeshes->Transform()->SetRelativeScale(Vec3(10.f, 10.f, 10.f));

    pMeshes->MeshRender()->SetMesh(CAssetMgr::GetInst()->FindAsset<CMesh>(L"SphereMesh"));
    pMeshes->MeshRender()->SetMaterial(CAssetMgr::GetInst()->FindAsset<CMaterial>(L"BlinnPhong"));

    // Rect
    CGameObject* pSquare = new CGameObject;
    pSquare->SetName(L"Rect");

    pSquare->AddComponent(new CTransform);
    pSquare->AddComponent(new CMeshRender);
    pSquare->AddComponent(new CPlayerScript);

    pSquare->Transform()->SetRelativePos(Vec3(-1000.f, 0.f, 500.f));
    pSquare->Transform()->SetRelativeScale(Vec3(100.f, 100.f, 100.f));
    pSquare->Transform()->SetAbsolute(true);

    pSquare->MeshRender()->SetMesh(CAssetMgr::GetInst()->FindAsset<CMesh>(L"RectMesh"));
    pSquare->MeshRender()->SetMaterial(CAssetMgr::GetInst()->FindAsset<CMaterial>(L"BlinnPhong"));

    pMeshes->AddChild(pSquare);

    // SquareGrid
    CGameObject* pSquareGrid = new CGameObject;
    pSquareGrid->SetName(L"SquareGrid");

    pSquareGrid->AddComponent(new CTransform);
    pSquareGrid->AddComponent(new CMeshRender);
    pSquareGrid->AddComponent(new CPlayerScript);

    pSquareGrid->Transform()->SetRelativePos(Vec3(-750.f, 0.f, 500.f));
    pSquareGrid->Transform()->SetRelativeScale(Vec3(100.f, 100.f, 100.f));
    pSquareGrid->Transform()->SetAbsolute(true);

    pSquareGrid->MeshRender()->SetMesh(CAssetMgr::GetInst()->FindAsset<CMesh>(L"SquareGridMesh"));
    pSquareGrid->MeshRender()->SetMaterial(CAssetMgr::GetInst()->FindAsset<CMaterial>(L"BlinnPhong"));

    pMeshes->AddChild(pSquareGrid);

    // Box
    CGameObject* pBox = new CGameObject;
    pBox->SetName(L"Box");

    pBox->AddComponent(new CTransform);
    pBox->AddComponent(new CMeshRender);
    pBox->AddComponent(new CPlayerScript);

    pBox->Transform()->SetRelativePos(Vec3(-500.f, 0.f, 500.f));
    pBox->Transform()->SetRelativeScale(Vec3(100.f, 100.f, 100.f));
    pBox->Transform()->SetAbsolute(true);

    pBox->MeshRender()->SetMesh(CAssetMgr::GetInst()->FindAsset<CMesh>(L"BoxMesh"));
    pBox->MeshRender()->SetMaterial(CAssetMgr::GetInst()->FindAsset<CMaterial>(L"BlinnPhong"));

    pMeshes->AddChild(pBox);

    // Cylinder
    CGameObject* pCylinder = new CGameObject;
    pCylinder->SetName(L"Cylinder");

    pCylinder->AddComponent(new CTransform);
    pCylinder->AddComponent(new CMeshRender);
    pCylinder->AddComponent(new CPlayerScript);

    pCylinder->Transform()->SetRelativePos(Vec3(-250.f, 0.f, 500.f));
    pCylinder->Transform()->SetRelativeScale(Vec3(100.f, 100.f, 100.f));
    pCylinder->Transform()->SetAbsolute(true);

    pCylinder->MeshRender()->SetMesh(CAssetMgr::GetInst()->FindAsset<CMesh>(L"CylinderMesh"));
    pCylinder->MeshRender()->SetMaterial(CAssetMgr::GetInst()->FindAsset<CMaterial>(L"BlinnPhong"));

    pMeshes->AddChild(pCylinder);

    // Sphere
    CGameObject* pSphere = new CGameObject;
    pSphere->SetName(L"Sphere");

    pSphere->AddComponent(new CTransform);
    pSphere->AddComponent(new CMeshRender);
    pSphere->AddComponent(new CPlayerScript);

    pSphere->Transform()->SetRelativePos(Vec3(0.f, 0.f, 500.f));
    pSphere->Transform()->SetRelativeScale(Vec3(100.f, 100.f, 100.f));
    pSphere->Transform()->SetAbsolute(true);

    pSphere->MeshRender()->SetMesh(CAssetMgr::GetInst()->FindAsset<CMesh>(L"SphereMesh"));
    pSphere->MeshRender()->SetMaterial(CAssetMgr::GetInst()->FindAsset<CMaterial>(L"BlinnPhong"));

    pMeshes->AddChild(pSphere);

    // Tetrahedron
    CGameObject* pTetrahedron = new CGameObject;
    pTetrahedron->SetName(L"Tetrahedron");

    pTetrahedron->AddComponent(new CTransform);
    pTetrahedron->AddComponent(new CMeshRender);
    pTetrahedron->AddComponent(new CPlayerScript);

    pTetrahedron->Transform()->SetRelativePos(Vec3(250.f, 0.f, 500.f));
    pTetrahedron->Transform()->SetRelativeScale(Vec3(100.f, 100.f, 100.f));
    pTetrahedron->Transform()->SetAbsolute(true);

    pTetrahedron->MeshRender()->SetMesh(CAssetMgr::GetInst()->FindAsset<CMesh>(L"TetrahedronMesh"));
    pTetrahedron->MeshRender()->SetMaterial(CAssetMgr::GetInst()->FindAsset<CMaterial>(L"BlinnPhong"));

    pMeshes->AddChild(pTetrahedron);

    // Icosahedron
    CGameObject* pIcosahedron = new CGameObject;
    pIcosahedron->SetName(L"Icosahedron");

    pIcosahedron->AddComponent(new CTransform);
    pIcosahedron->AddComponent(new CMeshRender);
    pIcosahedron->AddComponent(new CPlayerScript);

    pIcosahedron->Transform()->SetRelativePos(Vec3(500.f, 0.f, 500.f));
    pIcosahedron->Transform()->SetRelativeScale(Vec3(100.f, 100.f, 100.f));
    pIcosahedron->Transform()->SetAbsolute(true);

    pIcosahedron->MeshRender()->SetMesh(CAssetMgr::GetInst()->FindAsset<CMesh>(L"IcosahedronMesh"));
    pIcosahedron->MeshRender()->SetMaterial(CAssetMgr::GetInst()->FindAsset<CMaterial>(L"BlinnPhong"));

    pMeshes->AddChild(pIcosahedron);

    // Subdivide SphereMesh
    CGameObject* pSubdivideSphere = new CGameObject;
    pSubdivideSphere->SetName(L"Subdivide SphereMesh");

    pSubdivideSphere->AddComponent(new CTransform);
    pSubdivideSphere->AddComponent(new CMeshRender);
    pSubdivideSphere->AddComponent(new CPlayerScript);

    pSubdivideSphere->Transform()->SetRelativePos(Vec3(750.f, 0.f, 500.f));
    pSubdivideSphere->Transform()->SetRelativeScale(Vec3(100.f, 100.f, 100.f));
    pSubdivideSphere->Transform()->SetAbsolute(true);

    pSubdivideSphere->MeshRender()->SetMesh(CAssetMgr::GetInst()->FindAsset<CMesh>(L"SubdivideSphereMesh"));
    pSubdivideSphere->MeshRender()->SetMaterial(CAssetMgr::GetInst()->FindAsset<CMaterial>(L"BlinnPhong"));

    pMeshes->AddChild(pSubdivideSphere);

    // Circle SphereMesh
    CGameObject* pCircle = new CGameObject;
    pCircle->SetName(L"Circle");

    pCircle->AddComponent(new CTransform);
    pCircle->AddComponent(new CMeshRender);
    pCircle->AddComponent(new CPlayerScript);

    pCircle->Transform()->SetRelativePos(Vec3(1000.f, 0.f, 500.f));
    pCircle->Transform()->SetRelativeScale(Vec3(100.f, 100.f, 100.f));
    pCircle->Transform()->SetAbsolute(true);

    pCircle->MeshRender()->SetMesh(CAssetMgr::GetInst()->FindAsset<CMesh>(L"CircleMesh"));
    pCircle->MeshRender()->SetMaterial(CAssetMgr::GetInst()->FindAsset<CMaterial>(L"BlinnPhong"));

    pMeshes->AddChild(pCircle);

    AddObject(pMeshes, L"Mesh");
}

void CTestLevel::AddModels()
{
    //// Zelda Model
    // CGameObject* pZelda =
    //     CAssetMgr::GetInst()->LoadModel("Assets\\Models\\zeldaPosed001\\", "zeldaPosed001.fbx", L"Zelda");
    // if (nullptr != pZelda)
    //{
    //     pZelda->Transform()->SetRelativePos(Vec3(-500.f, 250.f, 0.f));
    //     pZelda->Transform()->SetRelativeScale(Vec3(100.f, 100.f, 100.f));

    //    AddObject(pZelda, L"Model");
    //}

    //// Damaged Helmet
    // CGameObject* pDamagedHelmet =
    //     CAssetMgr::GetInst()->LoadModel("Assets\\Models\\damaged-helmet\\", "DamagedHelmet.gltf", L"Damaged Helmet");
    // if (nullptr != pDamagedHelmet)
    //{
    //     pDamagedHelmet->Transform()->SetRelativePos(Vec3(-250.f, 250.f, 0.f));
    //     pDamagedHelmet->Transform()->SetRelativeScale(Vec3(100.f, 100.f, 100.f));

    //    AddObject(pDamagedHelmet, L"Model");
    //}

    //// blue whale
    // CGameObject* pblueWhale =
    //     CAssetMgr::GetInst()->LoadModel("Assets\\Models\\blue_whale\\", "scene.gltf", L"blue whale");
    // if (nullptr != pblueWhale)
    //{
    //     pblueWhale->Transform()->SetRelativePos(Vec3(0.f, 250.f, 0.f));
    //     pblueWhale->Transform()->SetRelativeScale(Vec3(100.f, 100.f, 100.f));

    //    AddObject(pblueWhale, L"Model");
    //}

    //// torii gate
    // CGameObject* ptoriigate =
    //     CAssetMgr::GetInst()->LoadModel("Assets\\Models\\torii_gate\\", "scene.gltf", L"torii gate", true);
    // if (nullptr != ptoriigate)
    //{
    //     ptoriigate->Transform()->SetRelativePos(Vec3(250.f, 250.f, 0.f));
    //     ptoriigate->Transform()->SetRelativeScale(Vec3(100.f, 100.f, 100.f));

    //    AddObject(ptoriigate, L"Model");
    //}

    // dragon warrior
    CGameObject* pDragonWarrior =
        CAssetMgr::GetInst()->LoadModel("Assets\\Models\\dragon_warrior\\", "scene.gltf", L"Dragon Warrior", true);
    if (nullptr != pDragonWarrior)
    {
        pDragonWarrior->Transform()->SetRelativePos(Vec3(500.f, 250.f, 0.f));
        pDragonWarrior->Transform()->SetRelativeScale(Vec3(100.f, 100.f, 100.f));

        AddObject(pDragonWarrior, L"Model");
    }
}