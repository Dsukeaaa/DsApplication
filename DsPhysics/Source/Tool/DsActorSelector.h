#ifndef _DS_ACTOR_SELECTOR_
#define _DS_ACTOR_SELECTOR_

namespace DsPhysics
{
	class DsActorId;
	class DsPhysicsWorld;
}

namespace DsPhysics
{

	class DsActorSelector
	{
	public:
		DsActorSelector();
		virtual ~DsActorSelector();

	public:
		/*
		@param[out] depth 採用衝突点までの長さ
		*/
		static DsActorId Select( const DsPhysicsWorld& world, double* depth=NULL );
		static void MoveActor( const DsPhysicsWorld& world );

	};
}


#endif