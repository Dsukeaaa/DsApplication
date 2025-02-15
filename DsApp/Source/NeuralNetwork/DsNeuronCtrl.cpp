#include "DsAppPch.h"
#ifndef _DS_NEURON_CTRL_
#include "NeuralNetwork/DsNeuronCtrl.h"
#endif

#ifndef _DS_NEURON_
#include "NeuralNetwork/DsNeuron.h"
#endif

using namespace DsApp;


DsNeuralNetwork* DsNeuronCtrl::CreateNeuralNetwork()
{
	DsNeuron* pInput = new DsNeuron[m_inputNum];
	DS_ASSERT(pInput, "メモリ確保失敗");
	DsNeuron* pOutput = new DsNeuron[m_outputNum];
	DS_ASSERT(pOutput, "メモリ確保失敗");
	DsNeuron** pInner = new DsNeuron*[m_innerLayerNum];
	DS_ASSERT(pInner, "メモリ確保失敗");
	for (int i = 0; i < m_innerLayerNum; ++i) {
		pInner[i] = new DsNeuron[m_innerNeuronNum];
		DS_ASSERT(pInner[i], "メモリ確保失敗");
	}

	//入力層と中間層の結び
	for (int i = 0; i < m_innerNeuronNum; ++i) {
		//全結合
		for (int j = 0; j < m_inputNum; ++j) {
			DsNeuronInput child;
			child.w = 0.5;//重さは適当
			child.pNeuron = &pInput[j];
			pInner[0][i].input.push_back(child);
		}
	}

	//中間層と中間層
	for (int i = 1; i < m_innerLayerNum; ++i) {
		for (int j = 0; j < m_innerNeuronNum; ++j) {
			for (int k = 0; k < m_innerNeuronNum; ++k) {
				DsNeuronInput child;
				child.w = 0.5;
				child.pNeuron = &pInner[i - 1][k];
				pInner[i][j].input.push_back(child);
			}
		}
	}

	//出力層と中間層
	for (int i = 0; i < m_outputNum; ++i) {
		for (int j = 0; j < m_innerNeuronNum; ++j) {
			DsNeuronInput child;
			child.w = 0.5;
			child.pNeuron = &pInner[m_innerLayerNum-1][j];
			pOutput[i].input.push_back(child);
		}
	}


	DsNeuralNetwork* ret = new DsNeuralNetwork;
	DS_ASSERT(ret, "メモリ確保失敗");
	ret->pInput = pInput;
	ret->pInner = pInner;
	ret->pOutput = pOutput;
	ret->inputNum = m_inputNum;
	ret->innerLayerNum = m_innerLayerNum;
	ret->innerNeuronNum = m_innerNeuronNum;
	ret->outputNum = m_outputNum;

	return ret;
}

void DsNeuronCtrl::CalcOutput(DsNeuralNetwork* pNet)
{
	const int outputNum = pNet->outputNum;
	const int inputNum = pNet->inputNum;
	const int innnerNum = pNet->innerNeuronNum;
	const int innerLayerNum = pNet->innerLayerNum;

	DsNeuron* pOutput = pNet->pOutput;
	DsNeuron** pInner = pNet->pInner;
	DsNeuron* pInput = pNet->pInput;

	//入力層-中間層
	for (int layerNum = 0; layerNum < innerLayerNum; ++layerNum) {
		for (int innerIdx = 0; innerIdx < innnerNum; ++innerIdx) {
			float sum = 0;
			for (const DsNeuronInput& input : pInner[layerNum][innerIdx].input) {
				sum += input.w * input.pNeuron->outVal;
			}

			//シグモイド関数
			pInner[layerNum][innerIdx].outVal = 0;

		}
	}

	//中間層-出力層
	for (int outIdx = 0; outIdx < outputNum; ++outIdx) {
		float sum = 0;
		for (const DsNeuronInput& input : pOutput[outIdx].input) {
			sum += input.w * input.pNeuron->outVal;
		}
		pOutput[outIdx].outVal = 0;
	}
}
