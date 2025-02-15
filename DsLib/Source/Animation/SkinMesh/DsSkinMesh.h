#ifndef _DS_SKINMESH_
#define _DS_SKINMESH_

namespace DsLib
{
	class DsKeyframeAnimSet;
	class DsAnimSkeleton;
	class DsModel;
}

namespace DsLib
{
	class DsSkinMesh
	{
	public:
		DsSkinMesh(const DsModel& model);
		virtual ~DsSkinMesh();

	public:
		void Initialize();

		void ApplySkeleton(const DsAnimSkeleton& skeleton);
		DsModel* GetSkinMeshModel() { return m_pModel; }

	public:
		void DbgDraw() const;

	public:
		const DsModel& m_srcModel;//変形前
		DsModel* m_pModel;//変形後
	};
}

#endif