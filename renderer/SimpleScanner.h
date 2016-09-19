/*********************************************************
** SimpleScanner.h
** Primary author: Landon K
** Description: Scans text for patterns (int, float, vector)
*********************************************************/

#ifndef SIMPLE_SCANNER_H
#define SIMPLE_SCANNER_H

#include <queue>
#include <string>

class SimpleScanner
{
    private:
        std::string delimiter;
        std::queue<std::string> tokens;
    public:
        std::string next();
        int nextInt();
        float nextFloat();

        bool hasNext();
        bool hasNextInt();
        bool hasNextFloat();

        bool protectedNext(std::string&);
        bool protectedNextInt(int&);
        bool protectedNextFloat(float&);
        bool protectedNextVector(float[3]);


        static bool isInt(std::string);
        static bool isFloat(std::string);

        void setDelimiter(std::string s){delimiter = s;}
		
        void compile(std::string, std::string = "");
        SimpleScanner(){}
        SimpleScanner(std::string str, std::string delim = ""){compile(str,delim);}

};
#endif
