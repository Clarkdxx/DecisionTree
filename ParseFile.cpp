//
//  ParseFile.cpp
//  DecisionTree
//
//  Created by Xiaoxi Dai on 22/10/2017.
//  Copyright © 2017 Xiaoxi Dai. All rights reserved.
//

#include <fstream>
#include <sstream>
#include <algorithm>
#include <random>
#include <ctime>
#include <math.h>
#include <iostream>
#include "ParseFile.h"

ParseFile::ParseFile(std::string const fileName)
{
    std::ifstream inFile(fileName);
    if (inFile.is_open())
    {
        std::string line;
        int numLine = 0;
        while (static_cast<bool>(std::getline(inFile, line)))
        {
            std::istringstream ss(line);
            std::string token;
            if(numLine == 0)    //read the features
            {
                int num = 0;
                while(std::getline(ss, token, ','))
                {
                    if(num != 0)
                    {
                        mFeatures.push_back(token);
                    }
                    num = 1;
                }
                numLine++;
            }
            else
            {
                std::vector<char> myData;
                int num = 0;
                while(std::getline(ss, token, ','))
                {
                    if(numLine == 1 && num != 0)
                    {
                        std::vector<char> attribute;
                        attribute.push_back(token[0]);
                        mAttributeVal.push_back(attribute);
                    }
                    else if(num != 0)
                    {
                        if(std::find(mAttributeVal[num - 1].begin(), mAttributeVal[num - 1].end(), token[0]) == mAttributeVal[num - 1].end())
                        {
                            mAttributeVal[num - 1].push_back(token[0]);
                        }
                    }
                    num++;
                    myData.push_back(token[0]);
                }
                mDataSet.push_back(myData);
                numLine++;
            }
        }
        inFile.close();
    }
    std::mt19937 g(static_cast<uint32_t>(time(0)));
    std::shuffle(mDataSet.begin(), mDataSet.end(), g);
    CreateSets();
}

TreeNode* ParseFile::DecesionTreeLearning(std::vector<std::vector<char> > dataSet,
                                         std::vector<std::vector<char> > parentSet,
                                         std::set<int> chosenSet,
                                         int depth)
{
    numOperation++;
    if(dataSet.size() == 0)     //if we have no more expamles
    {
        TreeNode* newNode = new TreeNode(-1, CalculatePlurality(parentSet));    //leaf node, p or not p
        return newNode;
    }
    char classification = dataSet[0][0];
    bool bSameClass = true;
    if(dataSet.size() > 1)
    {
        for(int i = 1; i < dataSet.size(); i++)
        {
            if(classification != dataSet[i][0])
            {
                bSameClass = false;
                break;
            }
        }
    }
    if(bSameClass)  //if all data have same classification
    {
        int val = static_cast<int>(classification) - 48;
        TreeNode* newNode = new TreeNode(-1, val);  //leaf node, p or not p
        return newNode;
    }
    else if(chosenSet.size() == mFeatures.size())   //we have used all features
    {
        TreeNode* newNode = new TreeNode(-1, CalculatePlurality(dataSet));
        return newNode;
    }
    else
    {
        if(depth == mDepthBound)
        {
            return new TreeNode(-1, CalculatePlurality(dataSet));
        }
        double max = -1.0;
        int maxIdx = -1;    //the index of the feature with the largest infogain in mFeature
        for(int i = 0; i < mFeatures.size(); i++)
        {
            if(chosenSet.find(i) == chosenSet.end())    //if we have not use this feature
            {
                double infoGain = CalculateInfoGain(i + 1, dataSet);  //we dont have classifcation in mFeature, so + 1
                if(infoGain > max)
                {
                    max = infoGain;
                    maxIdx = i;
                }
            }
        }
        chosenSet.insert(maxIdx);
        
        TreeNode* newNode = new TreeNode(maxIdx, 2);
        for(int i = 0; i < mAttributeVal[maxIdx].size(); i++)
        {
            std::vector<std::vector<char> > subDataSet;
            for(int j = 0; j < dataSet.size(); j++)
            {
                if(dataSet[j][maxIdx + 1] == mAttributeVal[maxIdx][i])
                {
                    subDataSet.push_back(dataSet[j]);
                }
            }
            newNode->AddChild(mAttributeVal[maxIdx][i], DecesionTreeLearning(subDataSet,
                                                              dataSet,
                                                              chosenSet,
                                                              depth + 1));
        }
        return newNode;
    }
}

int ParseFile::CalculatePlurality(std::vector<std::vector<char> > dataSet)
{
    int p = 0;
    int n = 0;
    for(int i = 0; i < dataSet.size(); i++)
    {
        if(dataSet[i][0] == '0')
        {
            n++;
        }
        else
        {
            p++;
        }
    }
    if(p < n)
    {
        return 0;
    }
    return 1;
}

void ParseFile::CreateSets()
{
    int size = static_cast<int>(mDataSet.size());
    int testSize = size * 0.8;  //0 - (testSize - 1) is the test set
    std::copy(mDataSet.begin() + testSize, mDataSet.end(), std::back_inserter(mTestSet));
    std::copy(mDataSet.begin(), mDataSet.begin() + testSize, std::back_inserter(mTrainSet));
    
    std::vector<std::vector<char> > parentSet;
    std::set<int> chosenSet;
    mDepthBound = -1;
    TreeNode* root = DecesionTreeLearning(mTrainSet, parentSet, chosenSet, 0);
    double result = RunTest(mTrainSet, root); //(double)correct / mTrainSet.size();
    std::cout<<"Part 1 80%: "<<result<<" "<<std::endl;
    result = RunTest(mTestSet, root);
    numOperation = 0;
    std::cout<<"Part 1 20%: "<<result<<" "<<std::endl;
    DeleteAll(root);
    
    std::mt19937 g(static_cast<uint32_t>(time(0)));     //shuffle the training set
    std::shuffle(mTrainSet.begin(), mTrainSet.end(), g);
    testSize = mTrainSet.size() * 0.25;
    std::copy(mTrainSet.begin(), mTrainSet.begin() + testSize, std::back_inserter(mVaildSet));
    std::vector<std::vector<char> > newTrainSet(mTrainSet.begin() + testSize, mTrainSet.end());     //the new training set
    
    std::cout<<"depth    "<<"train%   "<<"vaild%"<<std::endl;
    double maxAccuracy = -1.0;
    int maxDepth = 0;
    for(int i = 1; i < 16; i++)
    {
        parentSet.clear();
        chosenSet.clear();
        mDepthBound = i;
        TreeNode* newRoot = DecesionTreeLearning(newTrainSet, parentSet, chosenSet, 0);
        double resultVaild = RunTest(mVaildSet, newRoot);
        std::cout<<i<<"        "<<RunTest(newTrainSet, newRoot)<<"    "<<resultVaild<<" "<<std::endl;
        if(resultVaild > maxAccuracy)
        {
            maxAccuracy = resultVaild;
            maxDepth = i;
        }
        DeleteAll(newRoot);
    }
    parentSet.clear();
    chosenSet.clear();
    mDepthBound = maxDepth;
    TreeNode* newRoot = DecesionTreeLearning(mTrainSet, parentSet, chosenSet, 0);
    std::cout<<"Depth for max accuracy: "<<maxDepth<<" Max Accuracy: "<<RunTest(mTestSet, newRoot)<<std::endl;
    DeleteAll(newRoot);
}

double ParseFile::RunTest(std::vector<std::vector<char> > testSet, TreeNode* root)
{
    int correct = 0;
    for(int i = 0; i < testSet.size(); i++)
    {
        TreeNode* testRoot = root;
        int result = -1;
        if(root->GetVal() != 2) //we are at depth 0
        {
            result = root->GetVal();
        }
        while(result == -1)
        {
            int indexFeature = testRoot->GetFeature();  //the index of the attribute
            for(int j = 0; j < testRoot->GetChilds().size(); j++)
            {
                if(testRoot->GetChilds()[j].first == testSet[i][indexFeature + 1])
                {
                    testRoot = testRoot->GetChilds()[j].second;
                    if(testRoot->GetVal() != 2)
                    {
                        result = testRoot->GetVal();
                    }
                    break;
                }
            }
        }
        if(result == static_cast<int>(testSet[i][0]) - 48)
        {
            correct++;
        }
    }
    return (double)correct / testSet.size() * 100;
}

//Proved correct calculation
double ParseFile::CalculateEntropy(int p, int n)
{
    double result = 0.0;
    int sum = p + n;
    if(p == 0 || n == 0)
    {
        return 0.0;
    }
    result -= ((double)p/sum)*(log2((double)p/sum)) + ((double)n/sum)*(log2((double)n/sum));
    return result;
}

double ParseFile::CalculateInfoGain(int index, std::vector<std::vector<char> > dataSet)
{
    double positive = 0, negative = 0;
    for(int i = 0; i < dataSet.size(); i++)
    {
        if(dataSet[i][0] == '0')
        {
            negative += 1;
        }
        else
        {
            positive += 1;
        }
    }
    double entropy;
    if(positive == 0 || negative == 0)
    {
        entropy = 0;
    }
    else
    {
        entropy = CalculateEntropy(positive, negative);
    }
    double remainder = 0;
    for(int i = 0; i < mAttributeVal[index - 1].size(); i++)
    {
        double negativeK = 0, positiveK = 0;
        double entropyK;
        for(int j = 0; j < dataSet.size(); j++)
        {
            if(dataSet[j][index] == mAttributeVal[index - 1][i])
            {
                if(dataSet[j][0] == '0')
                {
                    negativeK += 1;
                }
                else
                {
                    positiveK += 1;
                }
            }
        }
        if(positiveK == 0 || negativeK == 0)
        {
            entropyK = 0;
        }
        else
        {
            entropyK = CalculateEntropy(positiveK, negativeK);
        }
        remainder += (positiveK + negativeK) / (positive + negative) * entropyK;
    }
    return entropy - remainder;
}

void ParseFile::DeleteAll(TreeNode* root)
{
    if(root != nullptr)
    {
        for(int i = 0; i < root->GetChilds().size(); i++)
        {
            DeleteAll(root->GetChilds()[i].second);
        }
        delete root;
    }
}
