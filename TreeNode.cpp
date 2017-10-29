//
//  TreeNode.cpp
//  DecisionTree
//
//  Created by Xiaoxi Dai on 22/10/2017.
//  Copyright © 2017 Xiaoxi Dai. All rights reserved.
//

#include "TreeNode.h"
#include <iostream>
TreeNode::TreeNode(int feature, int val)
{
    mFeature = feature;
    mVal = val;
}

void TreeNode::AddChild(char featureVal, TreeNode* child)
{
    mChilds.push_back(std::make_pair(featureVal, child));
}

int TreeNode::GetFeature() const
{
    return mFeature;
}

int TreeNode::GetVal() const
{
    return mVal;
}

std::vector<std::pair<char, TreeNode*> >& TreeNode::GetChilds()
{
    return mChilds;
}

