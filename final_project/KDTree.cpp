#include "stdafx.h"
#include "common.h"
#include "KDTree.h"

using namespace std;

bool KDTree::Insert(KDTreeNode *newNode)
{
	if(m_trunk == NULL)
	{
		m_trunk = newNode;
		m_trunk->m_discriminator = Latitude;
		return true;
	}

	return Insert(newNode, *m_trunk);
}

bool KDTree::Insert(KDTreeNode *newNode, KDTreeNode & trunk)
{
	KDTreeNode * parentNode;
	KDTreeNode * currentNode = &trunk;
	Direction direction;
	while(currentNode != NULL && !newNode->isEqual(*currentNode, *m_verticies))
	{
		parentNode = currentNode;
		direction = Compare(*newNode, *currentNode);
		currentNode = (direction == Left) ? currentNode->m_left : currentNode->m_right;
	}

	const bool nodeNotInTree = (currentNode == NULL);
	if(nodeNotInTree)
	{
		newNode->m_discriminator = parentNode->getNextDiscriminator();
		
		if(direction == Left)
		{
			parentNode->m_left = newNode;
		}
		else
		{
			parentNode->m_right = newNode;
		}
		return 1;
	}

	return 0;
}

Direction KDTree::Compare(const KDTreeNode & p, const KDTreeNode & q) const
{
	const dnl::Point &pPoint = (*m_verticies)[p.m_vertexIndex];
	const dnl::Point &qPoint = (*m_verticies)[q.m_vertexIndex];
	switch(q.m_discriminator)
	{
	case Latitude:
		return (pPoint.m_x < qPoint.m_x) ? Left : Right;
	case Longitude:
		return (pPoint.m_y < qPoint.m_y) ? Left : Right;
	case Unspecified:
	default:
		// Invalid state, fail.
		assert(0);
		return Left;
	}
}

Direction KDTree::Compare(const double pX, const double pY, const KDTreeNode & q) const 
{
	const dnl::Point &qPoint = (*m_verticies)[q.m_vertexIndex];
	switch(q.m_discriminator)
	{
	case Latitude:
		return (pX < qPoint.m_x) ? Left : Right;
	case Longitude:
		return (pY < qPoint.m_y) ? Left : Right;
	case Unspecified:
	default:
		// Invalid state, fail.
		assert(0);
		return Left;
	}
}

int KDTree::getHeight() const 
{
	return getHeight(m_trunk);
}

int KDTree::getHeight(KDTreeNode const * const trunk) const 
{
	if(trunk == NULL)
	{
		return 0;
	}

	const int leftHeight = 1 + getHeight(trunk->m_left);
	const int rightHeight = 1 + getHeight(trunk->m_right);

	return (leftHeight > rightHeight) ? leftHeight : rightHeight;
}

int KDTree::getSize() const 
{
	return getSize(m_trunk);
}

int KDTree::getSize(KDTreeNode const * const trunk) const 
{
	if(trunk == NULL)
	{
		return 0;
	}

	const int leftSize = getSize(trunk->m_left);
	const int rightSize = getSize(trunk->m_right);

	return leftSize + rightSize + 1;
}

vector<KDTreeNode *> KDTree::Search( 
	const double latitudeMin,
	const double longitudeMin,
	const double latitudeMax,
	const double longitudeMax) const
{
	return Search(latitudeMin, longitudeMin, latitudeMax, longitudeMax, m_trunk);
}

vector<KDTreeNode *> KDTree::Search( 
	const double latitudeMin,
	const double longitudeMin,
	const double latitudeMax,
	const double longitudeMax,
	KDTreeNode * currentNode ) const
{
	vector<KDTreeNode *> nodesFound;

	if ( currentNode != NULL )
	{
		if (Compare(latitudeMin, longitudeMin, *currentNode) == Left)
		{
			nodesFound = Search(latitudeMin, longitudeMin,
				latitudeMax, longitudeMax, currentNode->m_left);
		}

		const dnl::Point &currentPoint = (*m_verticies)[currentNode->m_vertexIndex];
		const bool largerThenAllMins = 
			currentPoint.m_x >= latitudeMin && 
			currentPoint.m_y >= longitudeMin;
		const bool smallerThenAllMaxs = 
			currentPoint.m_x <= latitudeMax && 
			currentPoint.m_y <= longitudeMax;
		if (largerThenAllMins && smallerThenAllMaxs )
		{
			nodesFound.push_back(currentNode);
		}

		if (Compare(latitudeMax, longitudeMax, *currentNode) == Right)
		{
			vector<KDTreeNode *> tempVector;
			tempVector = Search(latitudeMin, longitudeMin,
				latitudeMax, longitudeMax, currentNode->m_right);

			nodesFound.insert(nodesFound.end(), tempVector.begin(), tempVector.end());
		}
	}

    return nodesFound;   
}

/*void KDTree::Delete(int number)
{
	KDTreeNode * parent;
	Direction parentDirection;
	KDTreeNode * nodeToDelete = Search(number, m_trunk, parent, parentDirection);
	assert(nodeToDelete != NULL);

	Delete(nodeToDelete, parent, parentDirection);	
}

void KDTree::Delete(KDTreeNode *nodeToDelete, KDTreeNode *parent, Direction parentDirection)
{
	KDTreeNode *replacementNode = Delete1(nodeToDelete);
	if(parent == NULL)
	{
		m_trunk = replacementNode;
	}
	else
	{
		if(parentDirection == Left)
		{
			parent->m_left = replacementNode;
		}
		else
		{
			parent->m_right = replacementNode;
		}
	}
}*/

KDTreeNode * KDTree::FindFather(KDTreeNode *child, KDTreeNode *currentNode, KDTreeNode *F)
{
	if(child == NULL || currentNode == NULL || F == NULL)
	{
		return NULL;
	}

	if(child->isEqual(*currentNode, *m_verticies))
	{
		return F;
	}

	return FindFather(child, 
		(Compare(*child, *currentNode) == Left) ? currentNode->m_left : currentNode->m_right, 
		currentNode);
}

Direction KDTree::ChildType(KDTreeNode *child, KDTreeNode *parent) const
{
	assert(child != NULL);
	assert(parent != NULL);

	if(parent->m_left != NULL && parent->m_left->isEqual(*child, *m_verticies))
	{
		return Left;
	}
	else if(parent->m_right != NULL && parent->m_right->isEqual(*child, *m_verticies))
	{
		return Right;
	}

	// Should never get here if child is actually a child of parent.	
	assert(0);
	return Left;
}

// Delete nodeToDelete and return a pointer to the root of the resulting subtree.
KDTreeNode * KDTree::Delete1(KDTreeNode *nodeToDelete)
{
	KDTreeNode *fatherOfReplacement;
	KDTreeNode *replacement;
	DType discriminator;
	
	// If both subtrees are null, this is a leaf.
	if(nodeToDelete->m_left == NULL && nodeToDelete->m_right == NULL)
	{
		return NULL;
	}
	
	discriminator = nodeToDelete->m_discriminator;

	// If higher subtree is null, switch for lower subtree.
	if(nodeToDelete->m_right == NULL)
	{
		nodeToDelete->m_right = nodeToDelete->m_left;
		nodeToDelete->m_left = NULL;
	}

	replacement = FindDiscriminatorMinimum(nodeToDelete->m_right, discriminator);
	fatherOfReplacement = FindFather(replacement, nodeToDelete->m_right, nodeToDelete);

	if(ChildType(replacement, fatherOfReplacement) == Left)
	{
		fatherOfReplacement->m_left = Delete1(replacement);
	}
	else
	{
		fatherOfReplacement->m_right = Delete1(replacement);
	}
	
	replacement->m_left = nodeToDelete->m_left;
	replacement->m_right = nodeToDelete->m_right;
	replacement->m_discriminator = nodeToDelete->m_discriminator;

	return replacement;
}

KDTreeNode * KDTree::FindDiscriminatorMinimum(KDTreeNode *currentNode, DType discriminator)
{
	if(currentNode == NULL)
	{
		return NULL;
	}

	if(currentNode->m_discriminator == discriminator)
	{
		if(currentNode->m_left == NULL)
		{
			return currentNode;
		}
		else
		{
			currentNode = currentNode->m_left;
		}
	}

	KDTreeNode * leftMinNode = FindDiscriminatorMinimum(currentNode->m_left, discriminator);
	KDTreeNode * rightMinNode = FindDiscriminatorMinimum(currentNode->m_left, discriminator);
	
	if(rightMinNode != NULL && rightMinNode->compare(*currentNode, discriminator, *m_verticies) < 0)
	{
		currentNode = rightMinNode;
	}
	if(leftMinNode != NULL && leftMinNode->compare(*currentNode, discriminator, *m_verticies) < 0)
	{
		currentNode = leftMinNode;
	}	
	
	return currentNode;
}
/*
KDTreeNode * KDTree::Search( 
	int number,
	KDTreeNode * currentNode,
	KDTreeNode * & parent,
	Direction & parentDirection) const
{
	if(currentNode == NULL)
	{
		return NULL;
	}

	// Check if this node is the parent of the one we're looking for
	if(currentNode->m_left != NULL && currentNode->m_left->m_number == number)
	{
		parent = currentNode;
		parentDirection = Left;
		return currentNode->m_left;
	}
	if(currentNode->m_right != NULL && currentNode->m_right->m_number == number)
	{
		parent = currentNode;
		parentDirection = Right;
		return currentNode->m_right;
	}

	// Perform recursive search
	KDTreeNode * leftResult = Search(number, currentNode->m_left, parent, parentDirection);
	if(leftResult != NULL)
	{
		return leftResult;
	}
	return Search(number, currentNode->m_right, parent, parentDirection);
}
*/


