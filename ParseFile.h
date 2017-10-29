//
//  ParseFile.h
//  DecisionTree
//
//  Created by Xiaoxi Dai on 22/10/2017.
//  Copyright © 2017 Xiaoxi Dai. All rights reserved.
//

#ifndef ParseFile_h
#define ParseFile_h
#include <string>
#include <vector>
#include <set>
#include "TreeNode.h"

class ParseFile
{
public:
    ParseFile(std::string const fileName);
    void CreateSets();
    double CalculateEntropy(int p, int n);
    double CalculateInfoGain(int index, std::vector<std::vector<char> >& dataSet);
    TreeNode DecesionTreeLearning(std::vector<std::vector<char> >& dataSet,
                                  std::vector<std::vector<char> >& parentSet);
    int CalculatePlurality(std::vector<std::vector<char> > dataSet);
private:
    std::vector<std::vector<char> > mDataSet;
    std::vector<std::vector<char> > mTrainSet;
    std::vector<std::vector<char> > mVaildSet;
    std::vector<std::vector<char> > mTestSet;
    std::set<int> mChosenSet;
    std::vector<std::string> mFeatures;
};
#endif /* ParseFile_h */
