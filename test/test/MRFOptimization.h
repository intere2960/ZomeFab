#ifndef __MRF_OPTIMIZATION_H
#define __MRF_OPTIMIZATION_H

#include <iostream>
#include <assert.h>
#include <vector>
#include <map>
#include <string.h>
#include <math.h>

// #include "LinAlgVectorReal.h"
// #include "LinAlgMatrixReal.h"
#include "VKGraph.h"

class MRFProblemSmoothness;
// class MRFProblemMatrix;
// class MRFProblemDenseMatrix;
// class MRFProblemSparseMatrix;

/**
 * Formulate MRF problem
 */
class MRFProblem
{
	public:
		MRFProblem(int nNodes, int nLabels, const VKGraph * neighborhood, const std::vector<double> & dataTerm);
		// MRFProblem(int nNodes, int nLabels, const VKGraph * neighborhood, const LinAlgVectorReal & dataTerm);
		virtual ~MRFProblem();
	
		virtual const MRFProblemSmoothness * SafeCastToSmoothness() const;
		// virtual const MRFProblemMatrix * SafeCastToMatrix() const;
		// virtual const MRFProblemSparseMatrix * SafeCastToSparseMatrix() const;
		
		virtual double NodeCost(int nodeID, int labelID) const;
		virtual double EdgeCost(int nodeID1, int nodeID2, int label1ID, int label2ID) const = 0;
		virtual int NNeighbors(int nodeID) const;
		virtual int Neighbor(int nodeID, int nID) const;
		virtual double LabelCost(int labelID) const;	 // returns 0 if not implemented
	
		virtual void UpdateDataCosts(const std::vector<double> & dataCosts);
		// virtual void UpdateDataCosts(const LinAlgVectorReal & dataCosts);
	
		virtual int NNodes() const;
		virtual int NLabels() const;
	
	protected:
		int m_iNNodes;
		int m_iNLabels;
		std::vector<double> m_DataTerm;
		// LinAlgVectorReal m_DataTerm;
		const VKGraph * m_pGraph;
};

/**
 * Optimizer: solve MRF problem
 */
class MRFOptimizer
{
	public:
		MRFOptimizer(const MRFProblem * problem);
		virtual ~MRFOptimizer();
		virtual void Optimize(int nIterations = -1) = 0;	// actual algorithm -- has to fill m_aNodeLabels
		virtual void SetLabel(int nodeID, int labelID);
		virtual int Label(int nodeID) const;
	
	// additional
		virtual void UpdateDataCosts();
		virtual double Energy(double * data = NULL, double * smooth = NULL, double * label = NULL) const;
		virtual void PerNodeEnergy(std::vector<double> & dataEnergies, std::vector<double> & smoothEnergies) const;
	
	protected:
		const MRFProblem * m_pProblem;
		int * m_aNodeLabels;
};

#endif



