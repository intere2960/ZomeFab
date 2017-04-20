/***************************************************************************
 *   Copyright (C) 2008 by Vladimir Kim									   *
 *   Author: Vladimir Kim (vk@princeton.edu)							   *
 ***************************************************************************/

#ifndef __VKGraph_H
#define __VKGraph_H

#include <vector>
#include <assert.h>
#include "VkFunctions.h"

class VKGraphEdge;

class VKGraphNode
{
	public:
		VKGraphNode();
		VKGraphNode(const VKGraphNode & anotherNode);
		int NEdges() const;
		VKGraphNode * Neighbor(int eID);
		VKGraphEdge * Edge(int eID);
		const VKGraphNode * Neighbor(int eID) const;
		const VKGraphEdge * Edge(int eID) const;
		int id;
		int original_id;	
		std::vector<VKGraphEdge*> edges;
};

class VKShortestPathNode
{
	public:
		int id;
		double distance;
		bool selected;
		VKShortestPathNode ** heappointer;
};

class VKGraphEdge
{
	public:
		double weight;
		VKGraphNode * n1;
		VKGraphNode * n2;
};


class VKGraph 
{
	public:
		VKGraph(int nNodes);
		virtual ~VKGraph();
	
		int NumNodes() const;
		VKGraphNode * Node(int nodeID);
		const VKGraphNode * Node(int nodeID) const;
	
		int NumEdges() const;
		VKGraphEdge * Edge(int edgeID);
		const VKGraphEdge * Edge(int edgeID) const;
		
		void AddEdge(int n1, int n2, double weight);
		bool Adjacent(int n1, int n2, double * weight = NULL) const;
		
		int FindConnectedComponents(std::vector<std::vector<int> > * comp2Node = NULL, std::vector<int> * ccs = NULL) const;
		void RemoveIsolated();
	
		void ShortestPathDistances(int nodeFrom, double * shortestDistances);
		void ThresholdedShortestPath(int nodeFrom, double threshold, double * shortestDistances);
		
		void EdgeRank(const char * dataPrefix, std::vector<int> & candidateEdges);

	
//	// NOTE: re-implement to use region-growing (currently too slow)
//		static int UpdateCCsAddedEdge(int node1, int node2, 
//									  std::vector<std::vector<int> > & ccToNodes,
//									  std::vector<int> & nodeToCC);
	
	protected:
//		void FillComponent2NodeMap(std::vector<std::vector<int> > & comp2Node,
//								   std::vector<int> & ccs);
		VKGraphNode * m_Nodes;
		std::vector<VKGraphEdge*> m_Edges;
		int m_iNNodes;
		VKShortestPathNode * m_aSPData;
};


#endif


