//
//  main.cpp
//  DecisionTree
//
//  Created by Xiaoxi Dai on 21/10/2017.
//  Copyright © 2017 Xiaoxi Dai. All rights reserved.
//

#include <iostream>
#include "ParseFile.h"

int main(int argc, const char * argv[]) {
    // insert code here...
    if(argc == 2)
    {
        std::string fileName = argv[1];
        ParseFile pf(fileName);
    }
    return 0;
}
