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
    /*
    std::cout<<"Start: "<<mAttributeVal.size()<<std::endl;
    for(auto i : mAttributeVal)
    {
        for(auto j : i)
        {
            std::cout<<j<<",";
        }
        std::cout<<std::endl;
    }
    
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
   // std::cout<<"Here: "<<CalculateInfoGain(0, mDataSet)<<std::endl;
    std::mt19937 g(static_cast<uint32_t>(time(0)));
    std::shuffle(mDataSet.begin(), mDataSet.end(), g);
    CreateSets();
}

TreeNode* ParseFile::DecesionTreeLearning(std::vector<std::vector<char> > dataSet,
                                         std::vector<std::vector<char> > parentSet,
                                         std::set<int> chosenSet)
{
    numO++;
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
        double max = -1.0;
        int maxIdx = -1;    //the index of the feature with the largest infogain in mFeature
        for(int i = 0; i < mFeatures.size(); i++)
        {
            if(chosenSet.find(i) == chosenSet.end())    //if we have not use this feature
            {
                double infoGain = CalculateInfoGain(i+1, dataSet);  //we dont have classifcation in mFeature, so + 1
                if(infoGain > max)
                {
                    max = infoGain;
                    maxIdx = i;
                }
            }
        }
        chosenSet.insert(maxIdx);
        maxIdx++;
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
        if(subVal.size() != mAttributeVal[maxIdx - 1].size())
        {
            for(auto i : mAttributeVal[maxIdx - 1])
            {
                if(std::find(subVal.begin(), subVal.end(), i) == subVal.end())
                {
                    subVal.push_back(i);
                }
            }
        }
        //for each value of ...
        TreeNode* newNode = new TreeNode(maxIdx - 1, 2);
        std::vector<std::vector<char> > subDataSet;
        
        for(int i = 0; i < subVal.size(); i++)
        {
            subDataSet.clear();
            if(i < subValIndex.size())
            {
                for(int j = 0; j < subValIndex[i].size(); j++)
                {
                    subDataSet.push_back(dataSet[subValIndex[i][j]]);   //generate the subdataset
                }
            }
            newNode->AddChild(subVal[i], DecesionTreeLearning(subDataSet,
                                                              dataSet,
                                                              chosenSet));
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
    std::copy(mDataSet.begin(), mDataSet.begin() + start, std::back_inserter(mTestSet));
    std::vector<std::vector<char> > parentSet;
    std::set<int> chosenSet;
    TreeNode* root = DecesionTreeLearning(mTrainSet, parentSet, chosenSet);
    int correct = 0;
    for(int i = testSize; i < mDataSet.size(); i++)
    {
        TreeNode* testRoot = root;
        int result = -1;
        while(result == -1)
        {
            int indexFeature = testRoot->GetFeature();  //the index of the attribute
            for(int j = 0; j < testRoot->GetChilds().size(); j++)
            {
                if(testRoot->GetChilds()[j].first == mDataSet[i][indexFeature + 1])
                {
                    testRoot = testRoot->GetChilds()[j].second;
                    if(testRoot->GetVal() != 2)
                    {
                        result = testRoot->GetVal();
                    }
                }
            }
        }
        if(result == static_cast<int>(mDataSet[i][0]) - 48)
        {
            correct++;
        }
    }
    double result = (double)correct / mTrainSet.size();
    std::cout<<result<<std::endl;
    //TreeNode* root = DecesionTreeLearning(mDataSet, parentSet, chosenSet);      //Change it later
    //std::cout<<root->GetFeature()<<std::endl;
    /*
    TreeNode* childRoot = nullptr;
    for(int i = 0; i < root->GetChilds().size(); i++)
    {
        TreeNode* tempNode = root->GetChilds()[i].second;
        if(tempNode->GetFeature() == -1)
        {
            std::cout<<root->GetChilds()[i].first<<", "<<tempNode->GetVal()<<std::endl;
        }
        else
        {
            std::cout<<root->GetChilds()[i].first<<", "<<mFeatures[tempNode->GetFeature()]<<" "<<tempNode->GetChilds().size()<<std::endl;
            childRoot = tempNode;
        }
    }
    
    TreeNode* childchildRoot = nullptr;
    for(int i = 0; i < childRoot->GetChilds().size(); i++)
    {
        TreeNode* tempNode = childRoot->GetChilds()[i].second;
        if(tempNode->GetFeature() == -1)
        {
            //std::cout<<childRoot->GetChilds()[i].first<<", "<<tempNode->GetVal()<<std::endl;
        }
        else
        {
            //std::cout<<childRoot->GetChilds()[i].first<<", "<<mFeatures[tempNode->GetFeature()]<<" "<<tempNode->GetChilds().size()<<std::endl;
            childchildRoot = tempNode;
        }
    }
    
    for(int i = 0; i < childchildRoot->GetChilds().size(); i++)
    {
        TreeNode* tempNode = childchildRoot->GetChilds()[i].second;
        if(tempNode->GetFeature() == -1)
        {
            std::cout<<childchildRoot->GetChilds()[i].first<<", "<<tempNode->GetVal()<<std::endl;
        }
        else
        {
            std::cout<<childchildRoot->GetChilds()[i].first<<", "<<mFeatures[tempNode->GetFeature()]<<" "<<tempNode->GetChilds().size()<<std::endl;
        }
    }
    std::cout<<childchildRoot->GetChilds().size()<<std::endl;*/
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
    double result;
    std::vector<std::pair<int, int>> subtree;   //number of p and n
    std::vector<char> subFeature;
    int totP = 0;
    for(int i = 0; i < dataSet.size(); i++)
    {
        if (std::find(subFeature.begin(), subFeature.end(), dataSet[i][index]) != subFeature.end()) //if you find the feature
        {
            int indexSubtree = static_cast<int>(distance(subFeature.begin(), find(subFeature.begin(), subFeature.end(), dataSet[i][index])));
            std::pair<int, int> myPair = subtree[indexSubtree];
            if(dataSet[i][0] == '1')
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
                subtree.push_back(std::make_pair(0, 1));
            }
            else
            {
                subtree.push_back(std::make_pair(1, 0));
                totP++;
            }
        }
    }
    result = CalculateEntropy(totP, static_cast<int>(dataSet.size() - totP));
    for(int i = 0; i < subtree.size(); i++)
    {
        int subP = subtree[i].first;
        int subN = subtree[i].second;
        int subSum = subP + subN;
        result -= (double)subSum / dataSet.size() * CalculateEntropy(subP, subN);
    }
    return std::round(result * 10000.0 ) / 10000.0;;
}


