#include "LSystem.hpp"
#include <string>
#include <iostream>
#include <fstream>

LSystem::Settings LSystem::settings;

LSystem::LSystem(std::string _filename)
{
    std::ifstream file(_filename);
    
    // first line contains start string
    std::getline(file, generated);

    // the following lines are non-terminals for each production rule
    std::string rule;
    while(std::getline(file, rule))
    {
        std::string nt = rule.substr(0, rule.find('='));
        std::string replacement = rule.substr(rule.find('=') + 1);

        // erase whitespace
        std::erase(nt, ' ');
        std::erase(replacement, ' ');

        rules[char(nt.c_str()[0])] = replacement;
    }

    // generate final string
    for (size_t idx = settings.generations; idx > 0; idx--)
    {
        std::string generation;
        
        for (std::string::iterator it = generated.begin(); it != generated.end(); ++it)
        {
            // return terminal, replace non-terminal
            if (rules.find((*it)) != rules.end())
            {
                generation += rules[(*it)];
                continue;
            }
            generation += (*it);
        }
        
        generated = generation;
    }
}
