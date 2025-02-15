#include "DsPhysicsPch.h"
#ifndef __DS_RAY__
#include "Actor/DsRay.h"
#endif
using namespace DsPhysics;


DsRay::DsRay( const DsActorId& id )
: DsActor(id)
, m_rot(DsMat33d::Identity())
, m_pos(DsVec3d::Zero())
, m_line()
, m_aabb()
, m_pCollisionGeometry(NULL)
{
	m_vertex[0] = DsVec3d::Zero();
	m_vertex[1] = DsVec3d::Zero();
	m_line.index[0]=0;
	m_line.index[1]=1;
}

DsRay::DsRay()
: DsActor(DsActorId())
, m_rot(DsMat33d::Identity())
, m_pos(DsVec3d::Zero())
, m_line()
, m_aabb()
, m_pCollisionGeometry(NULL)
{
	m_vertex[0] = DsVec3d::Zero();
	m_vertex[1] = DsVec3d::Zero();
	m_line.index[0] = 0;
	m_line.index[1] = 1;
}

DsRay::~DsRay()
{
	delete m_pCollisionGeometry; m_pCollisionGeometry = NULL;
}

//virtual
void DsRay::Update()
{

}

//virtual 
const DsVec3d& DsRay::GetPosition() const
{
	return m_pos;
}

void DsRay::Create( const DsVec3d& start, const DsVec3d& end )
{
	m_vertex[0] = start;
	m_vertex[1] = end;
	m_line.index[0]=0;
	m_line.index[1]=1;

	m_pos = (m_vertex[0] + m_vertex[1])*0.5;

	//姿勢
	//レイの基本方向はY軸＋
	DsMat33d rot = DsMat33d::Identity();
	const DsVec3d dir = DsVec3d::Normalize(end - start);
	const double dot = DsVec3d::Dot(DsVec3d::GetY(), dir);
	if (fabs(dot) < (1.0 - FLT_MIN)) {
		const double rad = DsACos(dot);
		const DsVec3d axis = DsVec3d::Cross(DsVec3d::GetY(), dir);
		m_rot = DsMat33d::RotateAxis(axis, rad);
	}
	else {
		//ほぼ真上に向かってて基本姿勢なので単位行列のまま
	}

	//AABB
	DsVec3d max = DsVec3d(-DBL_MAX, -DBL_MAX, -DBL_MAX);
	DsVec3d min = DsVec3d(DBL_MAX, DBL_MAX, DBL_MAX);
	for (int i = 0; i < 2; ++i)
	{
		const DsVec3d v = m_vertex[i];
		max.x = max(max.x, v.x);
		max.y = max(max.y, v.y);
		max.z = max(max.z, v.z);
		min.x = min(min.x, v.x);
		min.y = min(min.y, v.y);
		min.z = min(min.z, v.z);
	}
	m_aabb.Setup(max, min);
	m_pCollisionGeometry = new DsCollisionGeometry(m_vertex, GetVertexNum(), NULL, 0, &m_line, 1, GetId(), m_vertex, DsVec3d::Zero(), NULL, &m_aabb, m_rot);

}