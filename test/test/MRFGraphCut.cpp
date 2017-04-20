#include "MRFGraphCut.h"

/////  MRFOptimizer /////
MRFGraphCut::MRFGraphCut(const MRFProblem * problem, double real2int)
: MRFOptimizer(problem)
{
	m_fReal2Int = real2int;
	try
	{
		m_pProblem = problem;
		assert(m_pProblem!=NULL && m_pProblem->NLabels()>0 && m_pProblem->NNodes());
		m_aDataCost = new int[m_pProblem->NNodes()*m_pProblem->NLabels()];
		m_aLabelCost = new int[m_pProblem->NLabels()*m_pProblem->NLabels()];
		
		m_pGeneralGraphOptim = new GCoptimizationGeneralGraph(m_pProblem->NNodes(), m_pProblem->NLabels());
		
		UpdateDataCosts();
		
		for (int i=0; i<m_pProblem->NLabels(); i++)
		for (int j=0; j<m_pProblem->NLabels(); j++)
			m_aLabelCost[i*m_pProblem->NLabels()+j] = (i==j? 0 : 1);

		m_pGeneralGraphOptim->setSmoothCost(m_aLabelCost);
		m_pGeneralGraphOptim->setLabelCost(0);
		
		for (int n1=0; n1<m_pProblem->NNodes(); n1++)
		for (int nn=0; nn<m_pProblem->NNeighbors(n1); nn++)
		{
			int n2 = m_pProblem->Neighbor(n1, nn);
			assert(n2 < m_pProblem->NNodes());
			assert(m_pProblem->SafeCastToSmoothness() != NULL);		// i.e. different labeling will have the same penalty
			double w = m_pProblem->EdgeCost(n1, n2, 0, 1);			// just any different labels
			m_pGeneralGraphOptim->setNeighbors(n1, n2, (int)(w*m_fReal2Int));
		}
	}
	catch (GCException exception)
	{
		exception.Report();
		assert(false);
	}
	catch (...)
	{
		std::cout<<"[ERROR] MRF Initialization failed with unknown exception"<<std::endl;
		assert(false);
	}
}

MRFGraphCut::~MRFGraphCut()
{
	delete m_pGeneralGraphOptim;
}

void MRFGraphCut::Optimize(int nIterations)
{
	// run expansion for 2 iterations. For swap use gc->swap(num_iterations);
	try
	{
		m_pGeneralGraphOptim->expansion(nIterations);
		for (int nid=0; nid<m_pProblem->NNodes(); nid++)
			m_aNodeLabels[nid]=m_pGeneralGraphOptim->whatLabel(nid);
	}
	catch (GCException exception)
	{
		exception.Report();
		assert(false);
	}
	catch (...)
	{
		std::cout<<"[ERROR] MRF Optimization failed with unknown exception"<<std::endl;
		assert(false);
	}
}

double MRFGraphCut::Energy(double * data, double * smooth, double * label) const
{
	if (data!=NULL)
		*data = ((double)m_pGeneralGraphOptim->giveDataEnergy())/m_fReal2Int;
	if (smooth!=NULL)
		*smooth = ((double)m_pGeneralGraphOptim->giveSmoothEnergy())/m_fReal2Int;
	if (label!=NULL)
		*label = ((double)m_pGeneralGraphOptim->giveLabelEnergy())/m_fReal2Int;
	return ((double)m_pGeneralGraphOptim->compute_energy())/m_fReal2Int;
}

void MRFGraphCut::UpdateDataCosts()
{
	for (int i=0; i<m_pProblem->NNodes(); i++)
	for (int l=0; l<m_pProblem->NLabels(); l++)
		m_aDataCost[i*m_pProblem->NLabels()+l] = (int)(m_pProblem->NodeCost(i, l) * m_fReal2Int);
	m_pGeneralGraphOptim->setDataCost(m_aDataCost);
}

void MRFGraphCut::SetLabel(int nodeID, int labelID)
{
	m_aNodeLabels[nodeID] = labelID;
	m_pGeneralGraphOptim->setLabel(nodeID, labelID);
}







