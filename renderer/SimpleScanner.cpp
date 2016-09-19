/*********************************************************
** SimpleScanner.h
** Primary author: Landon K
** Description: Scans text for patterns (int, float, vector)
*********************************************************/

#include "SimpleScanner.h"
#include <cctype>

bool SimpleScanner::isInt(std::string s)
{
    int length = s.length();
    if (length <= 0)
        return false;
    for (int x = 0; x < length; x++)
        if (std::isdigit(s[x]) == 0 && (s[0] != '-'))
            return false;
    return true;
}

bool SimpleScanner::isFloat(std::string s)
{
    std::string s1, s2;
    int loc;
    loc = s.find(".");
    if (loc == std::string::npos)
        return isInt(s);
    s1 = s.substr(0, loc);
    if (loc < s.length() - 1)
        s2 = s.substr(loc + 1, s.length() - loc );
    if (isInt(s1))
    {
        if (s2.length() == 0 || !isInt(s2))
            return false;
        return true;
    }
}

bool SimpleScanner::hasNextInt()
{
    if (tokens.empty())
        return 0;

    return isInt(tokens.front());
}


bool SimpleScanner::hasNext()
{
    if (tokens.empty())
        return 0;
    return(tokens.front().length() > 0);
}

bool SimpleScanner::hasNextFloat()
{
    if (tokens.empty())
        return 0;
    return isFloat(tokens.front());
}

std::string SimpleScanner::next()
{
    if (tokens.empty())
    {
        throw("Exception: no more tokens.");
        return "";
    }
    else
    {
        std::string s = tokens.front();
        tokens.pop();
        return s;
    }
}

int SimpleScanner::nextInt()
{
    if (tokens.empty())
    {
        throw("Exception: no more tokens.");
        return 0;
    }
    else
    {
        std::string s = tokens.front();
        if (std::isdigit(s[0]) == 0 && s[0] != '-')
        {
            throw("Exception: type mismatch");
            return 0;
        }
        else
        {
            tokens.pop();
            return atoi(s.c_str());
        }
    }
}

float SimpleScanner::nextFloat()
{
    if (tokens.empty())
    {
        throw("Exception: no more tokens.");
        return 0;
    }
    else
    {
        std::string s = tokens.front();
        if ((std::isdigit(s[0]) == 0) && (s[0] != '-') )
        {
            throw("Exception: type mismatch");
            return 0;
        }
        else
        {
            tokens.pop();
            return atof(s.c_str());
        }
    }
}

void SimpleScanner::compile(std::string input, std::string dLim)
{
    while (!tokens.empty())
        tokens.pop();
    if (dLim.length() > 0)
        delimiter = dLim;
    if (delimiter.length() <= 0)
        delimiter = " ";
    int start = 0;
    int end = input.length();
    int substrEnd = 0;
    int space = 0;
    std::string temp;
    while (input.length() > 0)
    {
        substrEnd = input.find(delimiter);
        if (substrEnd != std::string::npos)
        {
            if (substrEnd  == 0)
            {
                input.erase(0,delimiter.length());
            }
            else if (substrEnd > 0)
            {
                if (substrEnd < input.length())
                {
                    temp = input.substr(0, substrEnd);
                    if (temp.length() > 0)
                        tokens.push(temp);
                    input.erase(0,substrEnd + delimiter.length());
                }
            }
        }
        else
        {
            temp = input.substr(0, input.length());
            if (temp.length() > 0)
                tokens.push(temp);
            input.erase(0, input.length());
        }

    }
     
}

bool SimpleScanner::protectedNext(std::string& s)
{
    if (!hasNext())
        return false;
    try
    {
        s = next();
        return true;
    }
    catch (std::string ex)
    {
        //fl_alert(ex.c_str());
        return false;
    }
}

bool SimpleScanner::protectedNextInt(int& i)
{
    if (!hasNextInt())
        return false;
    try
    {
        i = nextInt();
        return true;
    }
    catch (std::string ex)
    {
        //fl_alert(ex.c_str());
        return false;
    }
}

bool SimpleScanner::protectedNextFloat(float& f)
{
    if (!hasNextFloat())
        return false;
    try
    {
        f = nextFloat();
        return true;
    }
    catch (std::string ex)
    {
        //fl_alert(ex.c_str());
        return false;
    }
}

bool SimpleScanner::protectedNextVector(float v[3])
{
    float temp;
    for (int x  = 0; x < 3; x++)
        if (!protectedNextFloat(temp))
            return false;
        else
            v[x] = temp;

    return true;
}
