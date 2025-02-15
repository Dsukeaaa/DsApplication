#include "DsPch.h"

#ifndef _DS_TRACE_PARTICLE_EMITTER_
#include "Graphics/Effect/Particle/DsTraceParticleEmitter.h"
#endif
//他のヘッダ


using namespace DsLib;

namespace
{
	//パラメータでもいいかもだけど今はとりあえず決め打ち
	static const double INTERVAL_TIME = 0.01;//発生間隔[秒]
	static const double PARTICLE_LIFE_TIME = 0.20;//パーティクル寿命

	static const int BLOCK_SIZE = 256;//この個数単位でメモリ確保される
	static const double DELETE_BLOCK_TIME = 5.0;//ブロックがこの時間以上空だったら削除する
}


DsTraceParticleEmitter::DsTraceParticleEmitter()
	: m_particle()
	, m_emitPos()
	, m_emitPrePos()
	, m_nextEmitTime(0)
	, m_isRequestEmit(false)
	, m_isRequestEmitPre(false)
{
	m_albedoTexPath = "TestParticle.tga";
	//m_normalTexPath = "TestParticle_normal.tga";
	m_isUseNormalMap = false;
}

DsTraceParticleEmitter::~DsTraceParticleEmitter()
{
	for (ParticleBlock& block : m_particle) {
		delete block.pDataTop;
	}
	m_particle.clear();
}

void DsTraceParticleEmitter::Update(double dt)
{
	/*
	|                     1 block  block内のメモリは連続                      |...
	|pHead->pNext->pNext->...pUseTail->NULL   pFreeTail->pNext->pNext->...NULL|
	*/


	//更新/削除
	for (ParticleBlock& block : m_particle) {
		DsSquareParticle* pParticle = block.pUseHead;
		DsSquareParticle* pPre = NULL;
		while (pParticle) {
			pParticle->lifeTime -= dt;

			//寿命尽きたので消す(フリーリストに移動させる)
			if (pParticle->lifeTime <= 0.0) {
				DsSquareParticle* pNext = pParticle->pNext;

				//使用中リストからはずす
				if (pPre) {//中間が消えた
					pPre->pNext = pNext;
				}
				else {//先頭が消えた
					block.pUseHead = pNext;
				}

				//消えたのが使用中末尾なら後退
				if (pParticle == block.pUseTail) {
					block.pUseTail = pPre;//先頭==使用中末尾ならpPreはNULLで使用中末尾はNULL
					if (block.pUseTail) {
						block.pUseTail->pNext = NULL;
					}
				}

				//フリーリストに追加
				pParticle->pNext = block.pFreeTail;
				block.pFreeTail = pParticle;

				//進んでないので一個前(pPre)は更新しない
				pParticle = pNext;
			}
			else {
				pPre = pParticle;
				pParticle = pParticle->pNext;
			}
		}
	}

	//リクエストが止まってたので一つ前とは繋がらないように
	if (!m_isRequestEmitPre ) {
		m_emitPrePos[0] = m_emitPos[0];
		m_emitPrePos[1] = m_emitPos[1];
	}

	//生成処理
	//停止中も次の発生時間はカウントする。再開したときにしばらく待たないと生成しないのが不自然だから
	if (0.0 <= m_nextEmitTime) {
		m_nextEmitTime -= dt;
	}
	if (m_isRequestEmit) {
		if (m_nextEmitTime <= 0.0) {

			//塊で確保
			//空いているブロックを探す
			ParticleBlock* pFreeBlock = NULL;
			for (ParticleBlock& block : m_particle) {
				if (block.pFreeTail) {
					pFreeBlock = &block;
					break;
				}
			}
			if (NULL == pFreeBlock) {
				DsSquareParticle* pParticle = new DsSquareParticle[BLOCK_SIZE];
				DS_ASSERT(pParticle, "メモリ確保失敗");
				for (int idx = 0; idx < BLOCK_SIZE-1; ++idx) {
					pParticle[idx].pNext = pParticle + idx + 1;
				}
				ParticleBlock block;
				block.pDataTop = pParticle;
				block.pUseHead = pParticle;
				block.pUseTail = NULL;
				block.pFreeTail = pParticle;
				m_particle.push_back(block);
				pFreeBlock = &m_particle.back();
			}
			
			DsSquareParticle* pParticle = pFreeBlock->pFreeTail;
			pParticle->pos[0] = m_emitPos[0];
			pParticle->pos[1] = m_emitPos[1];
			pParticle->pos[2] = m_emitPrePos[1];
			pParticle->pos[3] = m_emitPrePos[0];
			pParticle->speed = DsVec3d::Zero();//軌跡はその場にとどまるだけなので速度はなし
			pParticle->lifeTime = PARTICLE_LIFE_TIME;

			//フリーリストを消費
			pFreeBlock->pFreeTail = pFreeBlock->pFreeTail->pNext;

			//もし使用中先頭が消えてしまっていたら新規追加したこれが先頭
			if (NULL == pFreeBlock->pUseHead) {
				pFreeBlock->pUseHead = pParticle;
			}

			//使用中の末尾+1に追加
			if (pFreeBlock->pUseTail) {
				pFreeBlock->pUseTail->pNext = pParticle;
			}
			
			//使用中末尾を更新
			pFreeBlock->pUseTail = pParticle;
			//末尾の次はNULL
			pParticle->pNext = NULL;

			m_nextEmitTime = INTERVAL_TIME;

			m_emitPrePos[0] = m_emitPos[0];
			m_emitPrePos[1] = m_emitPos[1];
		}
	}


	//ブロックのメモリ開放
	{
		auto it = m_particle.begin();
		auto end = m_particle.end();
		while (it != end) {
			bool isErase = false;
			
			if (it->pUseTail == NULL) {
				it->deleteBlockTime -= dt;
				if (it->deleteBlockTime <= 0.0) {
					delete it->pDataTop;
					it = m_particle.erase(it);
					isErase = true;
				}
			}
			else {
				it->deleteBlockTime = DELETE_BLOCK_TIME;
			}

			if (!isErase) {
				++it;
			}
		}
	}

	m_isRequestEmitPre = m_isRequestEmit;
	m_isRequestEmit = false;
}

void DsTraceParticleEmitter::RequestEmit(const DsVec3d& pos1, const DsVec3d& pos2)
{
	m_isRequestEmit = true;//毎フレクリアされる
	m_emitPos[0] = pos1;
	m_emitPos[1] = pos2;
}

//virtual
double DsTraceParticleEmitter::GetAlpha(const double lifeTime)const //override
{
	return lifeTime / PARTICLE_LIFE_TIME;
}


bool DsTraceParticleEmitter::IsEmpty()const
{
	for (const ParticleBlock& block : m_particle) {
		if (block.pUseTail != NULL) {
			return false;
		}
	}
	return true;
}

//virtual
void DsTraceParticleEmitter::EnumParticle(const EnumType& func) const //override
{
	for (const ParticleBlock& block : m_particle) {
		const DsSquareParticle* pParticle = block.pUseHead;
		const DsSquareParticle* pPre = NULL;
		while (pParticle) {
			func(*pParticle);
			pParticle = pParticle->pNext;
		}
	}
}
