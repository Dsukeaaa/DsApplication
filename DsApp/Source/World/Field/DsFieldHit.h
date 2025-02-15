#ifndef _DS_FIELD_HIT_H_
#define _DS_FIELD_HIT_H_

#ifndef _DS_FIELD_OBJ_H_
#include "World/Field/DsFieldObj.h"
#endif

namespace DsApp
{
	class DsFieldHit : public DsFieldObj
	{
	public:
		DsFieldHit(DsLib::DsSys& sys, DsPhysics::DsPhysicsWorld& world);
		virtual ~DsFieldHit();

	public:
		virtual void Update1(double dt) override;
		virtual void Update2(double dt) override;
		virtual void Update3(double dt) override;
		virtual void Initialize(const DsFieldInitInfo& initInfo) override;
		virtual void SetPosition(const DsVec3d& pos) override;
		virtual void SetRotation(const DsMat33d& rot) override;
		virtual DsVec3d GetPosition() const override;
		virtual DsMat33d GetRotation() const override;
		virtual DsVec3d GetChrSize() const override;

	private:
		DsPhysics::DsActorId m_actorId;
		std::string m_hitName;


	public:
		virtual void DbgDraw(DsLib::DsDrawCommand& com) override;
		virtual void DbgSetStatic(bool isStatic) override;
		virtual void DbgSetGravity(bool isGravity) override;
		virtual void DbgSetDrawWireFrame(bool isWireFrame) override;
	};
}


#endif
