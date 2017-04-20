#include "MRFProblems.h"

////////////////////////////////////////
////////////////////////////////////////
////////    MRFProblemSmoothness   /////
////////////////////////////////////////
////////////////////////////////////////
MRFProblemSmoothness::MRFProblemSmoothness(int nNodes, int nLabels, const VKGraph * neighborhood, const std::vector<double> & dataCosts)
// MRFProblemSmoothness::MRFProblemSmoothness(int nNodes, int nLabels, const VKGraph * neighborhood, const LinAlgVectorReal & dataCosts)
: MRFProblem(nNodes, nLabels, neighborhood, dataCosts)
{
}

MRFProblemSmoothness::~MRFProblemSmoothness()
{
}

const MRFProblemSmoothness * MRFProblemSmoothness::SafeCastToSmoothness() const
{
	return this;
}

double MRFProblemSmoothness::EdgeCost(int nodeID1, int nodeID2, int label1ID, int label2ID) const
{
	double weight;
	if (label1ID!=label2ID && m_pGraph->Adjacent(nodeID1, nodeID2, &weight))
		return weight;
	return 0;
}

////////////////////////////////////////
////////////////////////////////////////
/////////    MRFProblemMatrix   ////////
////////////////////////////////////////
////////////////////////////////////////
// MRFProblemMatrix::MRFProblemMatrix(int nNodes, int nLabels, const VKGraph * neighborhood,
// 								   const LinAlgVectorReal & dataCosts, const LinAlgMatrixReal & allpairsCosts)
// : MRFProblem(nNodes, nLabels, neighborhood, dataCosts), m_AllPairsTerm(allpairsCosts)
// {
// }

// MRFProblemMatrix::~MRFProblemMatrix()
// {
	
// }

// const MRFProblemMatrix * MRFProblemMatrix::SafeCastToMatrix() const
// {
// 	return this;
// }

// double MRFProblemMatrix::EdgeCost(int nodeID1, int nodeID2, int label1ID, int label2ID) const
// {
// 	int mtxRow = nodeID1*NLabels()+label1ID;
// 	int mtxCol = nodeID2*NLabels()+label2ID;
// 	return m_AllPairsTerm(mtxRow, mtxCol);
// }

// const LinAlgMatrixReal * MRFProblemMatrix::Matrix() const
// {
// 	return &m_AllPairsTerm;
// }

// ////////////////////////////////////////
// ////////////////////////////////////////
// /////// MRFProblemSparseMatrix   ///////
// ////////////////////////////////////////
// ////////////////////////////////////////
// MRFProblemSparseMatrix::MRFProblemSparseMatrix(int nNodes, int nLabels, const VKGraph * neighborhood,
// 											   const LinAlgVectorReal & dataCosts,
// 											   const LinAlgMatrixSparseReal & allpairsCosts)
// : MRFProblem(nNodes, nLabels, neighborhood, dataCosts), m_AllPairsTerm(allpairsCosts)
// {
	
// }

// MRFProblemSparseMatrix::~MRFProblemSparseMatrix()
// {
	
// }

// const MRFProblemSparseMatrix * MRFProblemSparseMatrix::SafeCastToSparseMatrix() const
// {
// 	return this;
// }

// double MRFProblemSparseMatrix::EdgeCost(int nodeID1, int nodeID2, int label1ID, int label2ID) const
// {
// 	int mtxRow = nodeID1*NLabels()+label1ID;
// 	int mtxCol = nodeID2*NLabels()+label2ID;
// 	return m_AllPairsTerm.GetVal(mtxRow, mtxCol);
// }

// const LinAlgMatrixSparseReal * MRFProblemSparseMatrix::SparseMatrix() const
// {
// 	return &m_AllPairsTerm;
// }








