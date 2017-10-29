//
//  TreeNode.h
//  DecisionTree
//
//  Created by Xiaoxi Dai on 22/10/2017.
//  Copyright © 2017 Xiaoxi Dai. All rights reserved.
//

#ifndef TreeNode_h
#define TreeNode_h
#include <string>
#include <vector>

class TreeNode {
public:
    TreeNode(int feature, int val);     //feature is the index of attribute, -1 means it is a leaf
                                        //val: 0 means no, 1 means yes, 2 means it is a root for a tree or subtree.
    void AddChild(char featureVal, TreeNode* child);
    int GetFeature() const;
    int GetVal() const;
    std::vector<std::pair<char, TreeNode*> >& GetChilds();
private:
    int mFeature;
    int mVal;    //Yes, No, OR cant decide
    std::vector<std::pair<char, TreeNode*> > mChilds;   //char is the value on the edge
};

#endif /* TreeNode_h */
