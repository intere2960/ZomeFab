#include "MRFOptimization.h"

/////  MRFProblem /////
MRFProblem::MRFProblem(int nNodes, int nLabels, const VKGraph * neighborhood, const std::vector<double> & dataTerm)
// MRFProblem::MRFProblem(int nNodes, int nLabels, const VKGraph * neighborhood, const LinAlgVectorReal & dataTerm)
: m_DataTerm(dataTerm)
{
	m_iNNodes = nNodes;
	m_iNLabels = nLabels;
	m_pGraph = neighborhood;
}

MRFProblem::~MRFProblem()
{
	
}

const MRFProblemSmoothness * MRFProblem::SafeCastToSmoothness() const
{
	return NULL;
}

// const MRFProblemMatrix * MRFProblem::SafeCastToMatrix() const
// {
// 	return NULL;
// }

// const MRFProblemSparseMatrix * MRFProblem::SafeCastToSparseMatrix() const
// {
// 	return NULL;
// }


double MRFProblem::NodeCost(int nodeID, int labelID) const
{
	return m_DataTerm.at(nodeID * NLabels()+labelID);
}

double MRFProblem::LabelCost(int labelID) const
{
	return 0;
}

int MRFProblem::NNodes() const
{
	return m_iNNodes;
}

int MRFProblem::NLabels() const
{
	return m_iNLabels;
}

int MRFProblem::NNeighbors(int nodeID) const
{
	assert(m_pGraph!=NULL);
	return m_pGraph->Node(nodeID)->NEdges();
}

int MRFProblem::Neighbor(int nodeID, int nID) const
{
	assert(m_pGraph!=NULL);
	const VKGraphNode * n = m_pGraph->Node(nodeID); //n->Edge(nID)->weight;
	return n->Neighbor(nID)->id;
}

void MRFProblem::UpdateDataCosts(const std::vector<double> & dataCosts)
{
	m_DataTerm = dataCosts;
}

/////  MRFOptimizer /////
MRFOptimizer::MRFOptimizer(const MRFProblem * problem)
{
	m_pProblem = problem;
	m_aNodeLabels = new int[problem->NNodes()];
}

MRFOptimizer::~MRFOptimizer()
{
	delete [] m_aNodeLabels;	// problem should be deleted elswhere...
}


double MRFOptimizer::Energy(double * data, double * smooth, double * label) const
{
	double dsum=0;
	double lsum=0;
	double ssum=0;
	for (int i=0; i<m_pProblem->NNodes(); i++)
	{
		dsum += m_pProblem->NodeCost(i, Label(i));
		lsum += m_pProblem->LabelCost(Label(i));
		for (int n=0; n<m_pProblem->NNeighbors(i); n++)
		{
			int neighbor = m_pProblem->Neighbor(i, n);
			if (i<neighbor)
				ssum += m_pProblem->EdgeCost(i, neighbor, Label(i), Label(neighbor));
		}
	}
	
	if (data!=NULL)
		*data = dsum;
	if (smooth!=NULL)
		*smooth = ssum;
	if (label!=NULL)
		*label = lsum;
	return dsum+ssum+lsum;
}

void MRFOptimizer::PerNodeEnergy(std::vector<double> & dataEnergies, std::vector<double> & smoothEnergies) const
{
	for (int i=0; i<m_pProblem->NNodes(); i++)
	{
		dataEnergies.push_back(m_pProblem->NodeCost(i, Label(i))+m_pProblem->LabelCost(Label(i)));
		
		double ssum=0;
		for (int n=0; n<m_pProblem->NNeighbors(i); n++)
		{
			int neighbor = m_pProblem->Neighbor(i, n);
			ssum += m_pProblem->EdgeCost(i, neighbor, Label(i), Label(neighbor));
		}
		smoothEnergies.push_back(ssum);
	}
}


void MRFOptimizer::UpdateDataCosts()
{
	assert(false);
}

void MRFOptimizer::SetLabel(int nodeID, int labelID)
{
	m_aNodeLabels[nodeID] = labelID;
}

int MRFOptimizer::Label(int nodeID) const
{
	return m_aNodeLabels[nodeID];
}






