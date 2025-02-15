#ifndef __DS_COLLISION_DETECTION__
#define __DS_COLLISION_DETECTION__

namespace DsPhysics
{
	struct DsQuad;
	struct DsLine;
	class DsCollisionResult;
	class DsActorId;
	class DsCollisionGeometry;
}

namespace DsPhysics
{
	bool DsCD_PointFace( const DsCollisionGeometry* pBox1, const DsCollisionGeometry* pBox2, DsCollisionResult& info );
	bool DsCD_PointFace( const DsActorId& p1Id, const DsVec3d& p1, const DsCollisionGeometry* pBox2, DsCollisionResult& info);
	bool DsCD_LineFace( const DsCollisionGeometry* pBox1, const DsCollisionGeometry* pBox2, DsCollisionResult& info );
	bool DsCD_LineFaceVelocity(const DsCollisionGeometry* pBox1, const DsCollisionGeometry* pBox2, DsCollisionResult& info);
	bool DsCD_TriangleFace(const DsCollisionGeometry* pMesh, const DsCollisionGeometry* pBox, DsCollisionResult& info);

	//位置p1を基準としてpConvexのnormal方向の一番深い深度を求める
	double DsCD_NormalFaceDepthMax(const DsVec3d& p1, const DsVec3d& normal, const DsCollisionGeometry* pConvex);

	//便利系
	/*
		@param[in] pa 辺１の始点
		@param[in] ua 辺１
		@param[in] pb 辺２の始点
		@param[in] ub 辺２
		@param[out] alpha 辺１の交差点への長さ
		@param[out] beta 辺２の交差点への長さ
		@par 線分をはみ出たときのことは考慮されていない
	*/
	inline bool CalcNearestPoint(const DsVec3d& pa, const DsVec3d& ua, const DsVec3d& pb, const DsVec3d& ub, double& alpha, double& beta)
	{
		const DsVec3d p = pb - pa;
		double uaub = DsVec3d::Dot(ua, ub);
		double q1 = DsVec3d::Dot(ua, p);
		double q2 = -DsVec3d::Dot(ub, p);
		double d = 1.0 - uaub*uaub;
		if (d <= DBL_EPSILON) {
			alpha = 0;
			beta = 0;
			return false;
		}
		else {
			d = 1.0 / d;
			alpha = (q1 + uaub*q2)*d;
			beta = (uaub*q1 + q2)*d;
			return true;
		}
	}

}

#endif