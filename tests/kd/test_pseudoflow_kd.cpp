#include <tuple>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <iostream>
#include <iomanip>
#include <cstdint>
#include <cmath>
#include <locale>

#include "mineflow.hpp"
using namespace mvd::mineflow;

#include "catch2/catch_test_macros.hpp"
#include "testClock.h"

typedef uint64_t uint64;
typedef int64_t int64;
typedef uint8_t uint8;

TEST_CASE("pseudoflow_kd", "[pseudoflow][kd]")
{
    // inputs 
    std::string valueFile = "kd/kd.upit";
    std::string precFile = "kd/kd.prec";
    double objectiveValue = 652'195'037;

    // initialise pseudoflow input data structures
    uint64 numNodesPseudoFlow = 0;
    uint64 numArcsPseudoFlow = 0;
    double *nodeValueArray = NULL;
    uint64 *arcArray = NULL;

    // read in precedences from .prec file
    // initialise vector to store precs
    // each block index has a vector of preceding block indexes
    std::vector<std::vector<uint64>> precsIn;

    std::ifstream file;      // file stream
    file.open(precFile);     // open file
    std::string line = "";   // line variable
    uint64 countLine = 0;    // variable to count number of lines
    uint64 countCol = 0;     // variable to count number of columns in each line
    // check file is readable
    REQUIRE( file.is_open() );
    if(file.is_open()) //checking whether the file is open
    {
        while (std::getline(file, line)) 
        {
            precsIn.push_back(std::vector<uint64>());
            countLine++;  // increment the line count
            std::string buf;                                // Have a buffer string
            std::stringstream string_stream(line);          // Insert the line into a stream

            while (string_stream >> buf)
            {
                countCol++;  // increment the column count within the line
                switch(countCol)  // inspect which column
                {
                    case 1: // block id column
                        break;
                    case 2: // total number of precedences column
                        break;
                    default: // all the precedence blocks
                        precsIn[countLine-1].push_back(std::stoi(buf));
                        break;
                }
            }
            countCol = 0;  // reset line count of columns
        }
    }
    file.close();

    for (auto& arcsList: precsIn)
    {
        numNodesPseudoFlow++;
        numArcsPseudoFlow += arcsList.size();
    }

    // initialise node and arc arrays
    auto vals = std::make_shared<VecBlockValues>(numNodesPseudoFlow);
    auto precedence = std::make_shared<ExplicitPrecedence>(numNodesPseudoFlow);

    // add physical block precedence arcs
    for (uint64 i = 0; i < precsIn.size(); i++)
    {
        uint64 fromBlockIdx = i;  // from-TRONpseudoflowNode (source)
        for (uint64 j = 0; j < precsIn[i].size(); j++)
        {
            uint64 toBlockIdx = precsIn[i][j];  // to-TRONpseudoflowNode (sink)
            precedence->AddPrecedenceConstraint(fromBlockIdx, toBlockIdx);
        }
    }

    // read in block values for pseudoflow (.upit file)
    std::vector<double> values(numNodesPseudoFlow, 0.0);
    countLine = 0;
    countCol = 0;
    line = "";

    file.open(valueFile);     // open file
    // check file is readable
    REQUIRE( file.is_open() );
    if(file.is_open()) //checking whether the file is open
    {
        // strip first four lines of .upit file
        for (uint64 i = 0; i < 4; i++)
        {
            std::getline(file, line);
        }

        while (std::getline(file, line)) 
        {
            countLine++;  // increment the line count
            std::string buf;                                // Have a buffer string
            std::stringstream string_stream(line);          // Insert the line into a stream

            while (string_stream >> buf)
            {
                countCol++;  // increment the column count within the line
                switch(countCol)  // inspect which column
                {
                    case 1: // block id column
                        break;
                    case 2: // block value column
                        values[countLine-1] = std::stod(buf);
                        break;
                    default:
                        break;
                }
            }
            countCol = 0;  // reset line count of columns
        }
    }
    file.close();

    // add value arcs to problem data
    for (uint64 i = 0; i < values.size(); i++)
    {
        vals->SetBlockValueSI(i, values[i]);
    }

    // solve pseudoflow
    uint64 startTime = GetTimeMs64();

    PseudoSolver solver(precedence, vals);
    solver.Solve();
    
    uint64 endTime = GetTimeMs64();
    std::cout << "solve time:     " << (double)(endTime - startTime)/1000 << std::endl;

    // get min-cut from pseudoflow calc
    std::vector<uint8> result(numNodesPseudoFlow, 0);
    for (uint64 i = 0; i < numNodesPseudoFlow; i++)
    {
        result[i] = solver.InMinimumCut(i);
    }

    // get min cut value to check result
    double minCutValue = 0.0;
    for (uint64 i = 0; i < numNodesPseudoFlow; i++)
    {
        minCutValue += result[i] * values[i];
    }

    std::cout << "Value:          " << (int)minCutValue << std::endl;
    std::cout << "Expected Value: " << (int)objectiveValue << std::endl;

    double percDifference = abs(minCutValue-objectiveValue)/((minCutValue+objectiveValue)/2.0)*100.0;
    CHECK( percDifference < 0.01 );  // less than 0.01% percentage difference
}
