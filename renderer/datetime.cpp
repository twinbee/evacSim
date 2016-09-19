#include "datetime.h"

#include "stringutil.h"

#include <ctime>
#include <cstdio>

std::string datetime(int flags)
//format:  Www Mmm dd hh:mm:ss (AM/PM) yyyy
{
    using namespace std;

    time_t rawtime;

    time ( &rawtime );

    vector <string> tokens = tokenize(  ctime (&rawtime), " :");
    if (tokens.size() < 6) return string("Error: Time format is unexpected");

    string output;

    if (flags & DTWEEKDAY) output += (tokens[0] + " "); //weekday WWW
    if (flags & DTMONTH  ) output += (tokens[1] + " "); //month MMM
    if (flags & DTDAY    ) output += (tokens[2] + " "); //day of month dd
    if (flags & DTHOUR   )
    {
        int hour = int(toInt(tokens[3]));
        if (flags & DT12HR)
        {
            output += ( hour > 12 ? stringify(hour - 12) : tokens[3] ); //hour hh
        }
        else output += (tokens[3]); //hour hh
        if (flags & DTMIN    )
        {
            output += ("." + tokens[4]); //min mm
            if (flags & DTSEC    ) output += ("." + tokens[5]); //sec ss
        }
        if (flags & DT12HR) output += (hour > 12 ? " PM" : " AM");
        output += " ";
    }
    if (flags & DTYEAR   ) output += (tokens[6] + " "); //year yyyy

    return output;
}

std::string datetime_(int flags)
//format:  Www_Mmm_dd_hh:mm:ss_(AM/PM)_yyyy
{
    using namespace std;
   vector<string> in = tokenize(datetime(flags), " ");
	string out;   
   int i;
for (int i = 0; i < in.size()-1; i++)
	{
    out += in[i] + "_";
	}
  out += in[i+1];
  return out;
}
