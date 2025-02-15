#ifndef _DS_FIELD_CHR_H_
#define _DS_FIELD_CHR_H_

#ifndef _DS_FIELD_OBJ_H_
#include "World/Field/DsFieldObj.h"
#endif

namespace DsLib
{
	class DsHitRes;
}

namespace DsPhysics
{
	class DsRagdoll;
}

namespace DsApp
{
	class DsAnimRagdollModifier;
	class DsActionRequest;
	class DsActionCtrl;
}

namespace DsApp
{
	class DsFieldChr : public DsFieldObj
	{
	public:
		DsFieldChr(DsLib::DsSys& sys, DsPhysics::DsPhysicsWorld& world);
		virtual ~DsFieldChr();

	public:
		virtual void Initialize(const DsFieldInitInfo& initInfo) override;
		virtual void Update1(double dt);
		virtual void Update2(double dt);
		virtual void Update3(double dt);

	protected:
		virtual DsActionRequest* _CreareActionRequest();

	public:
		virtual DsActionRequest* GetActionRequest() const override { return m_pActReq; }

	public:
		virtual DsVec3d GetPosition() const override;
		virtual DsMat33d GetRotation() const override;
		virtual void SetPosition(const DsVec3d& pos) override;
		virtual void SetRotation(const DsMat33d& rot) override;
		virtual DsVec3d GetChrSize() const override;

	protected:
		DsVec3d m_vel;
		DsVec3d m_ang;
		DsPhysics::DsRagdoll* m_pRagdoll;
		DsAnimRagdollModifier* m_pAnimRagdollModifier;
		DsActionRequest* m_pActReq;
		DsActionCtrl* m_pActCtrl;
		DsPhysics::DsChrProxy* m_pProxy;



	public://�f�o�b�O
		virtual void DbgDraw(DsLib::DsDrawCommand& com);
		virtual void DbgSetStatic(bool isStatic) override;
		virtual void DbgSetGravity(bool isGravity) override;
		virtual void DbgSetDrawWireFrame(bool isWireFrame) override;
	};
}


#endif
