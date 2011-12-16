
#pragma once 

#include "common.h"
#include "KDTreeNode.h"

class KDTree
{
public:
	KDTree() : m_trunk(NULL), m_verticies(NULL) { }
	
	// Publicly exposed functions
	bool Insert(KDTreeNode *newNode);
	bool Insert(KDTreeNode *newNode, KDTreeNode & trunk);
	Direction Compare(const KDTreeNode & p, const KDTreeNode & q) const;
	Direction Compare(const double pX, const double pY, const KDTreeNode & q) const;
	int getHeight() const;
	int getSize() const;
	std::vector<KDTreeNode *> Search( 
		const double latitudeMin,
		const double longitudeMin,
		const double latitudeMax,
		const double longitudeMax) const;

	void Delete(int number);
	KDTreeNode * Search( 
		int number,
		KDTreeNode * currentNode,
		KDTreeNode * & parent,
		Direction & parentDirection) const;
	KDTreeNode * FindFather(KDTreeNode *child, KDTreeNode *currentNode, KDTreeNode *F);

	const std::vector<dnl::Point> *m_verticies;


private:
	// Data
	KDTreeNode *m_trunk;

	// Internal functions
	int getHeight(KDTreeNode const * const trunk) const;
	int getSize(KDTreeNode const * const trunk) const;
	std::vector<KDTreeNode *> Search( 
		const double latitudeMin,
		const double longitudeMin,
		const double latitudeMax,
		const double longitudeMax,
		KDTreeNode * T ) const;
	KDTreeNode * FindDiscriminatorMinimum(KDTreeNode *currentNode, DType discriminator);
	void Delete(KDTreeNode *nodeToDelete, KDTreeNode *parent, Direction parentDirection);
	KDTreeNode * Delete1(KDTreeNode *nodeToDelete);
	Direction ChildType(KDTreeNode *child, KDTreeNode *parent) const;
};
