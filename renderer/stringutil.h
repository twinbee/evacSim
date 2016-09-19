/*********************************************************
** stringutil.h
** Primary author: Matthew B
** Description: string tokenizer for <string> class of STL
** used everywhere, along with converters for std::string
*********************************************************/

#ifndef STRINGUTIL_H
#define STRINGUTIL_H

#include <cctype>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>

inline std::vector<std::string> tokenize(const std::string & str, const std::string & delim)
{
    using namespace std;
    vector<string> tokens;

    size_t p0 = 0, p1 = string::npos;
    while (p0 != string::npos)
    {
        p1 = str.find_first_of(delim, p0);
        if (p1 != p0)
        {
            string token = str.substr(p0, p1 - p0);
            tokens.push_back(token);
        }
        p0 = str.find_first_not_of(delim, p1);
    }

    return tokens;
}

inline double toDouble(const std::string& s)
{
    std::istringstream i(s);
    double x;
    i >> x;
    return x;
}

inline int toInt(const std::string& s)
{
    std::istringstream i(s);
    int x;
    i >> x;
    return x;
}

inline std::string stringify(double x)
{
    std::ostringstream o;
    o << x;
    return o.str();
}

inline std::string stringify(int x)
{
    std::ostringstream o;
    o << x;
    return o.str();
}


inline std::string uppercase(std::string myString)
{
    const int length = myString.length();
    for (int i=0; i!=length; ++i)
    {
        myString[i] = std::toupper(myString[i]);
    }
    return myString;
}

inline std::string lowercase(std::string myString)
{
    const int length = myString.length();
    for (int i=0; i!=length; ++i)
    {
        myString[i] = std::tolower(myString[i]);
    }
    return myString;
}

#endif
