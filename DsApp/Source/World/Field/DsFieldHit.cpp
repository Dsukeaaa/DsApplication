#include "DsAppPch.h"
#ifndef _DS_FIELD_HIT_H_
#include "World/Field/DsFieldHit.h"
#endif

#ifndef _DS_ANIM_RES_
#include "Res/DsAnimRes.h"
#endif
#ifndef _DS_HITRES_H_
#include "Res/DsHitRes.h"
#endif
#ifndef _DS_MODEL_
#include "Model/DsModel.h"
#endif
#ifndef __DS_APP_COLLISION_FILTER__
#include "World/Physics/DsAppCollisionFilter.h"
#endif
#include "World/Physics/DsPhysicsCreator.h"


using namespace DsLib;
using namespace DsPhysics;
using namespace DsApp;

DsFieldHit::DsFieldHit(DsSys& sys, DsPhysicsWorld& world)
	: DsFieldObj(sys, world)
{
	
}

DsFieldHit::~DsFieldHit()
{
	m_actorId = m_world.DeleteActor(m_actorId);
	m_sys.RefResource().UnregisterItem<DsHitRes>(m_hitName.c_str());
}

//virtual
void DsFieldHit::Initialize(const DsFieldInitInfo& initInfo)
{
	m_hitName = initInfo.hitName;
	const DsHitRes* pHitRes = m_sys.RefResource().RegisterItem<DsHitRes>(m_hitName.c_str());
	if (pHitRes) {
		m_actorId = DsPhysicsCreator::Create(m_world, *pHitRes, initInfo.physicsType, initInfo.pos, initInfo.ang, initInfo.name.c_str());
	}
	else {
		DS_LOG("オブジェ初期化情報が無かったのでオブジェは作られませんでした %s", m_name.c_str());
	}
	DsFieldObj::Initialize(initInfo);

	//地形あたりは内部のものは全て当たらない
	m_actorId.GetActor()->SetCollisionFilter(DsAppCollisionFilter::CalcFilterGroupInsideNoHit(COLLISION_GROUP::HIT));
}

//virtual 
void DsFieldHit::Update1(double dt)
{
	DsFieldObj::Update1(dt);
}
//virtual 
void DsFieldHit::Update2(double dt)
{
	DsFieldObj::Update2(dt);
}
//virtual 
void DsFieldHit::Update3(double dt)
{
	DsFieldObj::Update3(dt);
}

//virtual
void DsFieldHit::SetPosition(const DsVec3d& pos)
{
	DsActor* pActor = m_world.GetActor(m_actorId);
	if (pActor)
	{
		pActor->SetPosition(pos);
	}

	if (m_pAnimation)
	{
		m_pAnimation->SetRootMatrix(GetPosition(), GetRotation());
	}
}
//virtual
void DsFieldHit::SetRotation(const DsMat33d& rot)
{
	DsActor* pActor = m_world.GetActor(m_actorId);
	if (pActor)
	{
		pActor->SetRotation(rot);
	}

	if (m_pAnimation)
	{
		m_pAnimation->SetRootMatrix(GetPosition(), GetRotation());
	}
}

//virtual
DsVec3d DsFieldHit::GetPosition() const
{
	DsActor* pActor = m_world.GetActor(m_actorId);
	if (pActor)
	{
		return pActor->GetPosition();
	}
	else
	{
		return DsVec3d::Zero();
	}
}

//virtual
DsMat33d DsFieldHit::GetRotation() const
{
	DsActor* pActor = m_world.GetActor(m_actorId);
	if (pActor)
	{
		return pActor->GetRotation();
	}
	else
	{
		return DsMat33d::Identity();
	}
}

//virtual
DsVec3d DsFieldHit::GetChrSize() const
{
	//大体のサイズ
	DsVec3d ret = DsVec3d::Zero();
	const DsActor* pActor = m_world.GetActor(m_actorId);
	if (pActor) {
		const DsAabb aabb = pActor->RefAabb();
		ret = aabb.GetMax() - aabb.GetMin();
	}
	return ret;
}


//virtual 
void DsFieldHit::DbgDraw(DsLib::DsDrawCommand& com)
{
	DsActor* pActor = m_world.GetActor(m_actorId);
	if (pActor)
	{
		//メッシュは描画モデルと被るので弾く
		if (pActor->GetType() != DsActor::RIGID_MESH)
		{
			pActor->SetColor(DsVec4d(1, 1, 1, 0));
			pActor->SetLineColor(DsVec4d(0, 0, 0, 0));
			pActor->Draw(com);
		}
	}
}

//virtual
void DsFieldHit::DbgSetStatic(bool isStatic)
{
	m_actorId.GetActor()->RefOption().isStatic = isStatic;
}
//virtual
void DsFieldHit::DbgSetGravity(bool isGravity)
{
	m_actorId.GetActor()->RefOption().isGravity = isGravity;
}

//virtual
void DsFieldHit::DbgSetDrawWireFrame(bool isWireFrame)
{
	m_actorId.GetActor()->RefOption().isDrawWireFrame = isWireFrame;
}