#ifndef __MRFGraphCut_H
#define __MRFGraphCut_H

#include "GCoptimization.h"

#include <vector>
#include "MRFOptimization.h"
// #include "LinAlgMatrixReal.h"
// #include "LinAlgVectorReal.h"
#include "Sortable.h"

/**
 * Optimizer: solve MRF problem using graph cuts (just a wrapper).
 * See GCO_README.TXT for citations, code license, etc...
 */
class MRFGraphCut : public MRFOptimizer
{
	public:
		MRFGraphCut(const MRFProblem * problem, double real2int=1000);
		MRFGraphCut(const MRFProblem * problem, float label, double real2int = 1000);
		virtual ~MRFGraphCut();
		
		virtual void Optimize(int nIterations = -1);
	
		virtual void UpdateDataCosts();	// format: newcosts[nodeID * nlabels + labelID]
		virtual void SetLabel(int nodeID, int labelID);
		virtual double Energy(double * data = NULL, double * smooth = NULL, double * label = NULL) const;
		
	protected:
		double m_fReal2Int;
		GCoptimizationGeneralGraph * m_pGeneralGraphOptim;
		int * m_aDataCost;
		int * m_aLabelCost;
};

#endif


