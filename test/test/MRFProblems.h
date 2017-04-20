#ifndef __MRFProblems_H
#define __MRFProblems_H

#include <vector>
#include "MRFOptimization.h"
// #include "LinAlgMatrixReal.h"
// #include "LinAlgVectorReal.h"
// #include "LinAlgMatrixSparseReal.h"
#include "Sortable.h"

/**
 * A simple MRF problem with smoothness constraints
 */
class MRFProblemSmoothness : public MRFProblem
{
public:
	/** format: datacost[nodeID*nlabels + labelID] */	
	MRFProblemSmoothness(int nNodes, int nLabels, const VKGraph * neighborhood, const std::vector<double> & dataCosts);
	// MRFProblemSmoothness(int nNodes, int nLabels, const VKGraph * neighborhood, const LinAlgVectorReal & dataCosts);
	virtual ~MRFProblemSmoothness();
	virtual const MRFProblemSmoothness * SafeCastToSmoothness() const;
	virtual double EdgeCost(int nodeID1, int nodeID2, int label1ID, int label2ID) const;
};

/**
 * A full MRF problem with a value for (node1, node2, label1, label2) quadruplets
 */
// class MRFProblemMatrix : public MRFProblem
// {
// public:
// 	/** format: datacost[nodeID*nlabels + labelID], allpairscost[node1ID*nLabels+label1ID, node2ID*nLabels+label2ID] */
// 	MRFProblemMatrix(int nNodes, int nLabels, const VKGraph * neighborhood,
// 					 const LinAlgVectorReal & dataCosts, const LinAlgMatrixReal & allpairsCosts);
// 	virtual ~MRFProblemMatrix();
// 	virtual const MRFProblemMatrix * SafeCastToMatrix() const;
// 	virtual double EdgeCost(int nodeID1, int nodeID2, int label1ID, int label2ID) const;
// 	virtual const LinAlgMatrixReal * Matrix() const;
	
// protected:
// 	const LinAlgMatrixReal & m_AllPairsTerm;
// };

/**
 * A full MRF problem with a value for (node1, node2, label1, label2) quadruplets -- sparse representation
 */
// class MRFProblemSparseMatrix : public MRFProblem
// {
// public:
// 	/** format: datacost[nodeID*nlabels + labelID],
// 	 * allpairscost[node1ID*nLabels+label1ID, node2ID*nLabels+label2ID]  should be LinAlgMatrixSparseRealGeneral */
// 	MRFProblemSparseMatrix(int nNodes, int nLabels, const VKGraph * neighborhood,
// 						   const LinAlgVectorReal & dataCosts, const LinAlgMatrixSparseReal & allpairsCosts);
// 	virtual ~MRFProblemSparseMatrix();
// 	virtual const MRFProblemSparseMatrix * SafeCastToSparseMatrix() const;
// 	virtual double EdgeCost(int nodeID1, int nodeID2, int label1ID, int label2ID) const;
// 	virtual const LinAlgMatrixSparseReal * SparseMatrix() const;
	
// protected:
// 	const LinAlgMatrixSparseReal & m_AllPairsTerm;
// };

#endif


