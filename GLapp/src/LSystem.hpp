#pragma once

#include <cstddef>
#include <string>
#include <fstream>
#include <cmath>
#include <unordered_map>

#define _USE_MATH_DEFINES

struct LSystem 
{
    struct Settings
    {
        float twistAngle; // rotation around the current branch axis; 103 degrees as radians
        float turnAngle; // rotation around the branch x axis; 30 degrees as radians
        std::size_t generations;
        Settings(float _twistAngle = 4 * M_PI / 7, float _turnAngle = M_PI / 6) : twistAngle(_twistAngle), turnAngle(_turnAngle){};
    };
    
    static Settings settings;

    LSystem(std::string, int _generations = 4);
    std::unordered_map<char, std::string> rules;
    std::string generated;
};
