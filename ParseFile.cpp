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
                while(std::getline(ss, token, ','))
                {
                    mFeatures.push_back(token);
                }
                numLine++;
            }
            else
            {
                std::vector<char> myData;
                while(std::getline(ss, token, ','))
                {
                    myData.push_back(token[0]);
                }
                mDataSet.push_back(myData);
            }
        }
        inFile.close();
    }
    /*
    for(auto i : mFeatures)
    {
        std::cout<<i<<",";
    }
    std::cout<<std::endl;
    for(int i = 0; i < mDataSet.size(); i++)
    {
        for(int j = 0; j < mDataSet[i].size(); j++)
        {
            std::cout<<mDataSet[i][j]<<",";
        }
        std::cout<<std::endl;
    }*/
    std::mt19937 g(static_cast<uint32_t>(time(0)));
    std::shuffle(mDataSet.begin(), mDataSet.end(), g);
    CreateSets();
}

TreeNode ParseFile::DecesionTreeLearning(std::vector<std::vector<char> >& dataSet,
                                         std::vector<std::vector<char> >& parentSet)
{
    if(dataSet.size() == 0)     //if we have no more expamles
    {
        TreeNode newNode(-1, CalculatePlurality(parentSet));    //leaf node, p or not p
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
        int val = static_cast<int>(classification);
        TreeNode newNode(-1, val);  //leaf node, p or not p
        return newNode;
    }
    else if(mChosenSet.size() == mFeatures.size())   //we have used all features
    {
        TreeNode newNode(-1, CalculatePlurality(dataSet));
        return newNode;
    }
    else
    {
        double max = -1.0;
        int maxIdx = -1;    //the index of the feature with the largest infogain in mFeature
        for(int i = 0; i < mFeatures.size(); i++)
        {
            if(mChosenSet.find(i) == mChosenSet.end())    //if we have not use this feature
            {
                double infoGain = CalculateInfoGain(i, dataSet);
                if(infoGain > max)
                {
                    max = infoGain;
                    maxIdx = i;
                }
            }
        }
        mChosenSet.insert(maxIdx);
        std::vector<char> subVal;   //store the different value that the current featrue have
        std::vector<std::vector<int> > subValIndex; //store a vector of the index for each val of feature
        for(int i = 0; i < dataSet.size(); i++)
        {
            if (std::find(subVal.begin(), subVal.end(), dataSet[i][maxIdx]) != subVal.end())    //we have found this value before
            {
                int indexSubVal = static_cast<int>(distance(subVal.begin(), find(subVal.begin(), subVal.end(), dataSet[i][maxIdx])));
                subValIndex[indexSubVal].push_back(i);
            }
            else    //we find a new value for this feature
            {
                subVal.push_back(dataSet[i][maxIdx]);
                std::vector<int> newValIndex;
                newValIndex.push_back(i);
                subValIndex.push_back(newValIndex);
            }
        }
        //for each value of ...
        TreeNode newNode(maxIdx, 2);
        std::vector<std::vector<char> > subDataSet;
        for(int i = 0; i < subVal.size(); i++)
        {
            subDataSet.clear();
            for(int j = 0; j < subValIndex[i].size(); j++)
            {
                subDataSet.push_back(dataSet[subValIndex[i][j]]);   //generate the subdataset
            }
            newNode.AddChild(subVal[i], DecesionTreeLearning(subDataSet,
                                                              dataSet));
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
    int testSize = size * 0.2;  //0 - (testSize - 1) is the test set
    int start = testSize;
    std::copy(mDataSet.begin() + start, mDataSet.end(), std::back_inserter(mTrainSet));
    std::vector<std::vector<char> > parentSet;
    TreeNode root = DecesionTreeLearning(mTrainSet, parentSet);
    for(int i = 0; i < root.GetChilds().size(); i++)
    {
        TreeNode tempNode = root.GetChilds()[i].second;
        if(tempNode.GetFeature() == -1)
        {
            std::cout<<root.GetChilds()[i].first<<", "<<tempNode.GetVal()<<std::endl;
        }
        else
        {
            std::cout<<root.GetChilds()[i].first<<", "<<mFeatures[tempNode.GetFeature()]<<" "<<tempNode.GetChilds().size()<<std::endl;
        }
    }
}

//Proved correct calculation
double ParseFile::CalculateEntropy(int p, int n)
{
    double result = 0.0;
    int sum = p + n;
    result -= ((double)p/sum)*(log2((double)p/sum)) + ((double)n/sum)*(log2((double)n/sum));
    return result;
}

double ParseFile::CalculateInfoGain(int index, std::vector<std::vector<char> >& dataSet)
{
    //std::cout<<dataSet.size()<<std::endl;
    //int size = dataSet.size();
    double result;
    std::vector<std::pair<int, int>> subtree;   //number of p and n
    std::vector<char> subFeature;
    int totP = 0;
    for(int i = 0; i < dataSet.size(); i++)
    {
        if (std::find(subFeature.begin(), subFeature.end(), dataSet[i][index]) != subFeature.end())
        {
            int indexSubtree = static_cast<int>(distance(subFeature.begin(), find(subFeature.begin(), subFeature.end(), dataSet[i][index])));
            std::pair<int, int> myPair = subtree[indexSubtree];
            if(dataSet[i][0] == '0')
            {
                myPair.first++;
                totP++;
            }
            else
            {
                myPair.second++;
            }
            subtree[indexSubtree] = myPair;
        }
        else
        {
            subFeature.push_back(dataSet[i][index]);
            if(dataSet[i][0] == '0')
            {
                subtree.push_back(std::make_pair(1, 0));
            }
            else
            {
                subtree.push_back(std::make_pair(0, 1));
            }
        }
    }
    result = CalculateEntropy(totP, dataSet.size() - totP);
    for(int i = 0; i < subtree.size(); i++)
    {
        int subP = subtree[i].first;
        int subN = subtree[i].second;
        int subSum = subP + subN;
        result -= (double)subSum / dataSet.size() * CalculateEntropy(subP, subN);
    }
    return result;
}


