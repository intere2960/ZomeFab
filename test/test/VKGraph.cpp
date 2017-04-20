
#include "VKGraph.h"
#include <string>
#include <list>
#include "LinAlg.h"
#include "RNBasics/RNBasics.h"

// nodes
VKGraphNode::VKGraphNode()
{
	id = -1;
	original_id = -1;
}

VKGraphNode::VKGraphNode(const VKGraphNode & anotherNode)
{
	id = anotherNode.id;
	original_id = anotherNode.original_id;
	edges = anotherNode.edges;
}

int VKGraphNode::NEdges() const
{
	return (int)edges.size();
}
VKGraphEdge * VKGraphNode::Edge(int eID)
{
	assert(eID < NEdges() && eID>=0);
	return edges[eID];
}

VKGraphNode * VKGraphNode::Neighbor(int eID)
{
	VKGraphEdge * edge = Edge(eID);
	return ((edge->n1==this) ? (edge->n2) : (edge->n1));
}

const VKGraphEdge * VKGraphNode::Edge(int eID) const
{
	assert(eID < NEdges() && eID>=0);
	return edges[eID];
}

const VKGraphNode * VKGraphNode::Neighbor(int eID) const
{
	const VKGraphEdge * edge = Edge(eID);
	return ((edge->n1==this) ? (edge->n2) : (edge->n1));
}

// graph
VKGraph::VKGraph(int nNodes)
{
	m_aSPData = NULL;
	m_Nodes = new VKGraphNode[nNodes];
	m_iNNodes = nNodes;
	for (int i=0; i<nNodes; i++)
		m_Nodes[i].id = i;
}

VKGraph::~VKGraph()
{
	delete [] m_Nodes;
	for (int i=0; i<(int)m_Edges.size(); i++)
		delete [] m_Edges[i];
	if (m_aSPData!=NULL)
		delete [] m_aSPData;
}

int VKGraph::NumNodes() const
{
	return m_iNNodes;
}

VKGraphNode * VKGraph::Node(int nodeID)
{
	assert(nodeID < m_iNNodes);
	return &(m_Nodes[nodeID]);
}

const VKGraphNode * VKGraph::Node(int nodeID) const
{
	assert(nodeID < m_iNNodes);
	return &(m_Nodes[nodeID]);
}

int VKGraph::NumEdges() const
{
	return (int)m_Edges.size();
}

VKGraphEdge * VKGraph::Edge(int edgeID)
{
	assert(edgeID>=0 && edgeID < (int)m_Edges.size());
	return m_Edges[edgeID];
}

const VKGraphEdge * VKGraph::Edge(int edgeID) const
{
	assert(edgeID>=0 && edgeID < (int)m_Edges.size());
	return m_Edges[edgeID];
}

void VKGraph::AddEdge(int n1, int n2, double weight)
{
	assert(!Adjacent(n1, n2));
	VKGraphEdge * newEdge = new VKGraphEdge();
	newEdge->weight = weight;
	newEdge->n1 = Node(n1);
	newEdge->n2 = Node(n2);

	m_Edges.push_back(newEdge);
	
	Node(n1)->edges.push_back(newEdge);
	Node(n2)->edges.push_back(newEdge);
}

bool VKGraph::Adjacent(int n1, int n2, double * weight) const
{
	for (int i=0; i<(int)Node(n1)->edges.size(); i++)
		if (Node(n1)->edges[i]->n1->id == n2 || 
			Node(n1)->edges[i]->n2->id == n2)
		{
			if (weight!=NULL)
				*weight = Node(n1)->edges[i]->weight;
			return true;
		}
	return false;
}

int VKGraph::FindConnectedComponents(std::vector<std::vector<int> > * comp2Node, std::vector<int> * ccs) const
{
//	ccs.clear();
//	comp2Node.clear();
//	for (int i=0; i<m_iNNodes; i++)
//	{
//		ccs.push_back(i);
//		comp2Node.push_back(std::vector<int>());
//		comp2Node[i].push_back(i);
//	}
//	
//	for (int i=0; i<(int)m_Edges.size(); i++)
//		UpdateCCsAddedEdge(m_Edges[i]->n1->id, m_Edges[i]->n2->id, comp2Node, ccs);
//	
//	return (int)comp2Node.size();
	
	bool * visited = new bool[m_iNNodes];
	for (int i=0; i<m_iNNodes; i++)
		visited[i] = false;

	bool * inQueue = new bool[m_iNNodes];
	for (int i=0; i<m_iNNodes; i++)
		inQueue[i] = false;
	
	if (ccs!=NULL)
	{
		for (int i=0; i<m_iNNodes; i++)
			ccs->push_back(-1);
	}
	
	int ccID = -1;
	for (int i=0; i<m_iNNodes; i++)
	{
		if (visited[i])
			continue;
		
		// start new connected component
		if (comp2Node!=NULL)
		{
			ccID = (int)comp2Node->size();
			comp2Node->push_back(std::vector<int>());
		}
		else
			ccID++;
		
		std::list<int> toVisit;
		toVisit.push_back(i);
		inQueue[i] = true;
		
		while (toVisit.size()>0)
		{
			int vID = toVisit.front();
			toVisit.pop_front();
			assert(!visited[vID] && inQueue[vID]);
			visited[vID] = true;
			
			if (comp2Node!=NULL)
				(*comp2Node)[ccID].push_back(vID);
			
			if (ccs!=NULL)
				(*ccs)[vID] = ccID;
			
			// neighbors:
			for (int n=0; n<m_Nodes[vID].NEdges(); n++)
			{
				int nID = m_Nodes[vID].Neighbor(n)->id;
				if (!inQueue[nID])
				{
					inQueue[nID] = true;
					toVisit.push_back(nID);
				}
			}
		}
	}
	
	delete [] inQueue;
	delete [] visited;
	return (ccID+1);
}

void VKGraph::ShortestPathDistances(int nodeFromID, double * shortestDistances)
{
	if (m_aSPData==NULL)
		m_aSPData = new VKShortestPathNode [m_iNNodes];
	assert(m_aSPData!=NULL);
	for (int i=0; i<m_iNNodes; i++)
	{
		m_aSPData[i].id = m_Nodes[i].id;
		m_aSPData[i].distance = FLT_MAX;
		m_aSPData[i].selected = false;
		m_aSPData[i].heappointer = NULL;
	}

	// Initialize priority queue
	VKShortestPathNode *data = &(m_aSPData[nodeFromID]);
	assert(data!=NULL);
	data->distance = 0;
	RNHeap<VKShortestPathNode *> heap(data, &(data->distance), &(data->heappointer));
	heap.Push(data);		
	
	// Visit other nodes computing shortest distance
	while (!heap.IsEmpty()) 
	{
		VKShortestPathNode *data = heap.Pop();
		int nodeID = data->id;
		
		for (int j = 0; j < (int)m_Nodes[nodeID].NEdges(); j++) 
		{
			VKGraphEdge * edge = m_Nodes[nodeID].Edge(j);
			VKGraphNode *neighbor = m_Nodes[nodeID].Neighbor(j);
			VKShortestPathNode *neighbor_data = &(m_aSPData[neighbor->id]);
			RNScalar old_distance = neighbor_data->distance;
			
			RNScalar new_distance = edge->weight + data->distance;
			assert(new_distance>=0);
			if (new_distance < old_distance) 
			{
				neighbor_data->distance = new_distance;
				if (old_distance < FLT_MAX) heap.Update(neighbor_data);
				else heap.Push(neighbor_data);
			}
		}
	}
	
	for (int i=0; i<m_iNNodes; i++)
		shortestDistances[i] = m_aSPData[i].distance;
}

//void VKGraph::FillComponent2NodeMap(std::vector<std::vector<int> > & comp2Node,
//									std::vector<int> & ccs)
//{
//	comp2Node.clear();
//	for (int i=0; i<(int)comp2ID.size(); i++)
//	{
//		int cID = comp2ID[i];
//		while (cID >= (int)comp2Node.size())
//			comp2Node.push_back(std::vector<int>());
//		comp2Node[cID].push_back(i);
//	}
//}

void VKGraph::RemoveIsolated()
{
	// count isolated nodes
	int newNodeCount = 0;
	for (int i=0; i<m_iNNodes; i++)
	{
		if (m_Nodes[i].edges.size()>0)
			newNodeCount++;
	}
	
	//fill isolated nodes
	std::vector<int> oldNodeToNewNode;
	VKGraphNode * newNodes = new VKGraphNode[newNodeCount];
	int idOffset = 0;
	for (int i=0; i<m_iNNodes; i++)
	{
		if (m_Nodes[i].edges.size()>0)
		{
			newNodes[idOffset] = m_Nodes[i];
			newNodes[idOffset].id = idOffset;
			oldNodeToNewNode.push_back(idOffset);
			idOffset++;
		}
		else
			oldNodeToNewNode.push_back(-1);
	}
	
	// modify pointers to nodes (in edges)
	for (int i=0; i<(int)m_Edges.size(); i++)
	{
		int new1 = oldNodeToNewNode[m_Edges[i]->n1->id];
		int new2 = oldNodeToNewNode[m_Edges[i]->n2->id];
		assert(new1>=0 && new2>=0);
		m_Edges[i]->n1 = &(newNodes[new1]);
		m_Edges[i]->n2 = &(newNodes[new2]);
	}

	// re-assign node arrays and free data
	VKGraphNode * deleteMe = m_Nodes;
	m_Nodes = newNodes;
	m_iNNodes = newNodeCount;

	delete [] m_aSPData;
	m_aSPData = NULL;
	delete [] deleteMe;
}



///**
// * Helper function to findign connected components
// * updates connected components if edge is added from node1 to node2
// */
//int VKGraph::UpdateCCsAddedEdge(int node1, int node2, 
//								std::vector<std::vector<int> > & ccToNodes,
//								std::vector<int> & nodeToCC)
//{
//	if (ccToNodes.size()==1)
//		return -1;
//
//	assert(node1>=0 && node2>=0);
//	assert(node1 < (int)nodeToCC.size());
//	assert(node2 < (int)nodeToCC.size());
//	
//	int ccID1 = nodeToCC[node1];
//	int ccID2 = nodeToCC[node2];	
//	if (ccID1==ccID2)
//		return -1;
//	
//	int eraseCC = vkMax(ccID1, ccID2);
//	int singleCC = vkMin(ccID1, ccID2);
//	assert(ccID1 < (int)ccToNodes.size());
//	assert(ccID2 < (int)ccToNodes.size());
//	
//	for (int i=0; i<(int)ccToNodes[eraseCC].size(); i++)
//	{
//		int nodeID = ccToNodes[eraseCC][i];
//		nodeToCC[nodeID] = singleCC;
//		ccToNodes[singleCC].push_back(nodeID);
//	}
//	
//	for (int i=eraseCC+1; i<(int)ccToNodes.size(); i++)
//		for (int j=0; j<(int)ccToNodes[i].size(); j++)
//		{
//			int nodeID = ccToNodes[i][j];
//			nodeToCC[nodeID]--;
//		}
//	ccToNodes.erase(ccToNodes.begin() + eraseCC);
//	return eraseCC;
//}

/**
 * format: candidateEdges - (nodeID1, nodeID2) pairs of ints
 */
void VKGraph::EdgeRank(const char * dataPrefix,
					   std::vector<int> & candidateEdges)
{
	assert(candidateEdges.size()>=0);
	// write edges
	std::string graphEdges(dataPrefix);
	graphEdges = graphEdges + ".graph.edges.bin";
	std::string candEdgesFileName(dataPrefix);
	candEdgesFileName = candEdgesFileName + ".candidate.edges.bin";
	std::string sortedEdgesFileName(dataPrefix);
	sortedEdgesFileName = sortedEdgesFileName + ".ordered.edges.bin";
    
	// write graph edges
	FILE * edgesFile = fopen(graphEdges.c_str(), "wb");
	assert(edgesFile!=NULL);
	int * edgesToWrite = new int[2*m_Edges.size()];
//	std::cout<<"Writing: L=[ ";
	for (int i=0; i<(int)m_Edges.size(); i++)
	{
		edgesToWrite[2*i+0] = m_Edges[i]->n1->id;
		edgesToWrite[2*i+1] = m_Edges[i]->n2->id;
		assert(edgesToWrite[2*i+0]>=0);
		assert(edgesToWrite[2*i+1]>=0);
//		std::cout<<edgesToWrite[2*i+0]<<" -> "<<edgesToWrite[2*i+1]<<", "<<std::flush;
	}
//	std::cout<<"]"<<std::endl;
	int nEdges = 2*(int)m_Edges.size();
	fwrite(&nEdges, sizeof(int), 1, edgesFile);
	fwrite(edgesToWrite, sizeof(int), nEdges, edgesFile);
	fclose(edgesFile);
	delete [] edgesToWrite;

	// write candidate edges
	FILE * candEdgesFile = fopen(candEdgesFileName.c_str(), "wb");
	assert(candEdgesFile!=NULL);
	edgesToWrite = new int[candidateEdges.size()];
	for (int i=0; i<(int)candidateEdges.size(); i++)
		edgesToWrite[i] = candidateEdges[i];
	nEdges = (int)candidateEdges.size();
	fwrite(&nEdges, sizeof(int), 1, candEdgesFile);
	fwrite(edgesToWrite, sizeof(int), nEdges, candEdgesFile);
	fclose(candEdgesFile);
	delete [] edgesToWrite;
	
	// run matlab script
	bool bReady = LinAlg::RunMatlabScript("edgeRank", dataPrefix);
	if (!bReady)
		LinAlg::WaitForFile(sortedEdgesFileName.c_str(), 5, 8000);
	
	// read sorted edges
	FILE * sortedEdgesFile = fopen(	sortedEdgesFileName.c_str(), "rb");
	assert(sortedEdgesFile!=NULL);
	fread(&nEdges, sizeof(int), 1, sortedEdgesFile);
	edgesToWrite = new int[nEdges];
	fread(edgesToWrite, sizeof(int), nEdges, sortedEdgesFile);
	fclose(sortedEdgesFile);
	
	assert(nEdges==(int)candidateEdges.size());
	candidateEdges.clear();
	for (int i=0; i<nEdges; i++)
		candidateEdges.push_back(edgesToWrite[i]);
	
	delete [] edgesToWrite;
	
	// copy / rename files
	std::string graphEdges2			 = graphEdges + ".copy";
	std::string candEdgesFileName2	 = candEdgesFileName + ".copy";
	std::string sortedEdgesFileName2 = sortedEdgesFileName + ".copy";
	bool ok = rename(graphEdges.c_str(), graphEdges2.c_str());
	assert(ok==0);
	ok = rename(candEdgesFileName.c_str(), candEdgesFileName2.c_str());
	assert(ok==0);
	ok = rename(sortedEdgesFileName.c_str(), sortedEdgesFileName2.c_str());
	assert(ok==0);	
}

void VKGraph::ThresholdedShortestPath(int nodeFromID, double threshold, double * shortestDistances)
{
	if (m_aSPData==NULL)
		m_aSPData = new VKShortestPathNode [m_iNNodes];
	assert(m_aSPData!=NULL);
	for (int i=0; i<m_iNNodes; i++)
	{
		m_aSPData[i].id = m_Nodes[i].id;
		m_aSPData[i].distance = FLT_MAX;
		m_aSPData[i].selected = false;
		m_aSPData[i].heappointer = NULL;
	}
	
	// Initialize priority queue
	VKShortestPathNode *data = &(m_aSPData[nodeFromID]);
	assert(data!=NULL);
	data->distance = 0;
	RNHeap<VKShortestPathNode *> heap(data, &(data->distance), &(data->heappointer));
	heap.Push(data);
	
	// Visit other nodes computing shortest distance
	while (!heap.IsEmpty())
	{
		VKShortestPathNode *data = heap.Pop();
		int nodeID = data->id;
		
		if (data->distance > threshold)
			break;
		
		for (int j = 0; j < (int)m_Nodes[nodeID].NEdges(); j++)
		{
			VKGraphEdge * edge = m_Nodes[nodeID].Edge(j);
			VKGraphNode *neighbor = m_Nodes[nodeID].Neighbor(j);
			VKShortestPathNode *neighbor_data = &(m_aSPData[neighbor->id]);
			RNScalar old_distance = neighbor_data->distance;
			
			RNScalar new_distance = edge->weight + data->distance;
			assert(new_distance>=0);
			if (new_distance < old_distance && new_distance < threshold)
			{
				neighbor_data->distance = new_distance;
				if (old_distance < FLT_MAX) heap.Update(neighbor_data);
				else heap.Push(neighbor_data);
			}
		}
	}
	
	for (int i=0; i<m_iNNodes; i++)
		shortestDistances[i] = m_aSPData[i].distance;
	
}

