#include "TestPch.h"
#ifndef _DS_WINDOW_MANAGER_H_
#include "System/DsWindowManager.h"
#endif

#include<windows.h>

using namespace DsApp;

class TestMainLoop : public DsMainLoop
{
public:
	TestMainLoop()
		: DsMainLoop()
		//, m_pCreator(NULL)
		, m_gameWorld()
	{

	}

	~TestMainLoop()
	{
		//delete m_pCreator;
		//m_pCreator = NULL;
	}

public:
	virtual void Initialize(DsMainLoopArgs& args) override;
	virtual void BeforeWindowUpdate(DsMainLoopArgs& args) override;

private:
	DsGameWorld m_gameWorld;

private:
	//DsFieldObjectCreator* m_pCreator;
	//DsHinge2Joint* m_joint;
	//DsHinge2Joint* m_joint2;
	//DsAppCollisionCallback m_callback;
	//
	//DsCamera m_cam;
	//DsRenderCamCaptureImage* m_pImage;
	//DsCamera m_cam2;
	//DsRenderCamCaptureImage* m_pImage2;
};

void TestMainLoop::Initialize(DsMainLoopArgs& args)
{
	//const bool isSuccessSetUp = DsPhysicsManager::SetUpDefault();
	//
	//if (isSuccessSetUp)
	//{
	//	DsPhysicsWorld* pWorld = DsPhysicsManager::GetDefaultWorld();
	//	if (pWorld)
	//	{
	//		pWorld->SetCollisionCallback(&m_callback);
	//
	//		DsActorId act1;
	//		DsActorId act2;
	//		DsActorId act3;
	//		DsVec3d vertex[8];
	//		DsActor::Option option = DsActor::Option::Default();
	//		//option.isGravity = false;
	//
	//		m_joint = new DsHinge2Joint(*pWorld);
	//		m_joint2 = new DsHinge2Joint(*pWorld);
	//		if(0){//�W���C���g�e�X�g
	//			DsRigidBox::GetVertex(vertex, 0.5, 0.5, 0.5);
	//			{//���P
	//				DsRigidBox::DsRigidBoxFactory factory(vertex, 1.0, "���P");
	//				factory.InitPos(DsVec3d(2.0, 1.0, 5.0));
	//				factory.SetOption(option);
	//				act1 = pWorld->CreateActor(factory);
	//				act1.GetActor()->SetMaterial(DsActorMaterial::Aluminum());
	//			}
	//			{//���Q
	//				DsRigidBox::DsRigidBoxFactory factory(vertex, 1.0, "���Q");
	//				factory.InitPos(DsVec3d(1.0, 1.0, 5.0));
	//				factory.SetOption(option);
	//				act2 = pWorld->CreateActor(factory);
	//				act2.GetActor()->SetMaterial(DsActorMaterial::Aluminum());
	//			}
	//			{//���R
	//				DsRigidBox::DsRigidBoxFactory factory(vertex, 1.0, "���R");
	//				factory.InitPos(DsVec3d(0.0, 1.0, 5.0));
	//				factory.SetOption(option);
	//				act3 = pWorld->CreateActor(factory);
	//				act3.GetActor()->SetMaterial(DsActorMaterial::Aluminum());
	//			}
	//			m_joint->AttachJoint(act1, act2, act1.GetActor()->GetPosition() + DsVec3d(-0.5, 0, 0), DsVec3d(0,0,1), DsVec3d(1, 0, 0));
	//			m_joint2->AttachJoint(act2, act3, act2.GetActor()->GetPosition() + DsVec3d(-0.5, 0, 0), DsVec3d(0, 0, 1), DsVec3d(1, 0, 0));
	//		}
	//
	//		if(1){//�Փ˃e�X�g
	//			DsRigidBox::GetVertex(vertex, 1.5, 0.5, 1.5);
	//			for(int i=0; i<0; ++i){
	//				DsRigidBox::DsRigidBoxFactory factory(vertex, 1.0, "���P");
	//				factory.InitPos(DsVec3d(0.0, 0.0 + (0.5)*(double)i, 2.0));
	//				factory.SetOption(option);
	//				DsActorId actorId = pWorld->CreateActor(factory);
	//				DsActor* pActor = actorId.GetActor();
	//				pActor->SetMaterial(DsActorMaterial::Aluminum());
	//			}
	//			for (int i = 0; i<0; ++i) {
	//				DsRigidCapsule::DsRigidCapsuleFactory factory(0.5, 1.0, 1000.0, "���P");
	//				factory.InitPos(DsVec3d(-1.0, 1.6 + (2.0)*(double)i, 4.0));
	//				factory.InitRot(DsMat33d::RotateZ(0.3));
	//				factory.SetOption(option);
	//				DsActorId actorId = pWorld->CreateActor(factory);
	//				DsActor* pActor = actorId.GetActor();
	//				pActor->SetMaterial(DsActorMaterial::Aluminum());
	//			}
	//		}
	//
	//		const char* dataPath = "testmap.dmap";
	//		m_pCreator = new DsFieldObjectCreator(args.sys);
	//		m_pCreator->Create(dataPath, *pWorld);
	//	}
	//}
	//
	//const DsCamera& sysCam = args.sys.RefCam();
	//m_cam = sysCam;
	//m_pImage = DsRenderCamCaptureImage::Create(m_cam, 128, 128);
	//args.sys.RefRender().RegisterCaptureImage(m_pImage);
	//
	//m_cam2 = sysCam;
	//m_pImage2 = DsRenderCamCaptureImage::Create(m_cam2, 128, 128);
	//args.sys.RefRender().RegisterCaptureImage(m_pImage2);


	m_gameWorld.Initialize(args.sys);
}


void TestMainLoop::BeforeWindowUpdate(DsMainLoopArgs& args)
{
	m_gameWorld.Update(args.dt);
	
	//DsPhysicsWorld* pWorld = DsPhysicsManager::GetDefaultWorld();
	//if (pWorld) {
	//	const DsPhysicsWorld::Actors& actors = pWorld->GetActors();
	//	for (DsActor* pActor : actors) {
	//		args.drawCom.SetColor(DsVec3d(1, 0, 0));
	//		pActor->Draw(args.drawCom);
	//	}
	//}


	DsPerf::RefPerfTotalFps().DbgDrawFps();

	//args.drawCom.SetColor(DsVec3d(0, 0, 0)).DrawTextScreen(DsVec2d(-0.33, 0.36), L" w");
	//args.drawCom.SetColor(DsVec3d(0, 0, 0)).DrawTextScreen(DsVec2d(-0.33, 0.32), L"a d  �ړ�");
	//args.drawCom.SetColor(DsVec3d(0, 0, 0)).DrawTextScreen(DsVec2d(-0.33, 0.28), L" s");
	//args.drawCom.SetColor(DsVec3d(0, 0, 0)).DrawTextScreen(DsVec2d(-0.33, 0.20), L"drag ���_��]");
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow)
{
	HWND hwnd = DsWindowManager::MainWindowCreate(hInstance, lpCmdLine, nCmdShow);
	if (0 == hwnd) {
		return 0;
	}

	DsPath::SetAssetPath(DsPath::GetCurrentPath() + L"\\Asset");
	DsSys* pSys = new DsSys();
	DS_ASSERT(pSys, "�������m�ێ��s");
	TestMainLoop* pLoop = new TestMainLoop();
	DS_ASSERT(pLoop, "�������m�ێ��s");
	DsSysArgs args;
	args.argc = 0;
	args.argv = &lpCmdLine;
	args.pLoop = pLoop;
	args.pConfPath = "config.txt";
	args.windowType = DS_WINDOW_SYSTEM_TYPE_GL;
	args.windowHandle = (ds_uint64)hwnd;
	pSys->Setup(args);

	while(DsWindowManager::MainWindowUpdate(hwnd, *pSys));
	
	delete pLoop;
	delete pSys;
	return 0;
}
