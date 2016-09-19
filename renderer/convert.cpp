#include "convert.h"

#include <cmath>

#include <string>

float randomN(float min, float max)
{
    return rand()/(float(RAND_MAX)+1)*(max-min) + min;
}

int RoundUp(double x)
{
    return (int)(x + 0.5);
}

int nextpoweroftwo(int x)
{
    double logbase2 = log(x) / log(2);
    return RoundUp(pow(2,ceil(logbase2)));
}


double d2r(double a)
{ //degrees to radians
    return 0.0174532925 * a;
}

double r2d(double a)
{ //radians to degrees
    return 57.2957795 * a;
}

double ft2m (double feet)
{
    return feet*0.3048;
}

double m2ft (double meters)
{
    return meters*3.2808399;
}

double ftSq2mSq (double feetsquared)
{
    return feetsquared*(1/10.7639104);
}

double mSq2ftSq (double meterssquared)
{
    return meterssquared*(10.7639104);
}

double days2s(double days )
{
    return days*24*60*60;
}

double hr2s(double hours)
{
    return hours*60*60;
}

double hr2min(double hours)
{
    return hours*60;
}

double hr2days(double hours)
{
    return hours / 24.0;
}

double ft2mi(double feet)
{
    return feet/5280;
}

double mi2ft(double miles)
{
    return miles*5280;
}

double ft2in(double feet)
{
    return feet*12;
}

double in2ft(double inches)
{
    return inches/12;
}

double inSq2ftSq(double inchesSq)
{
    return inchesSq/144;
}

double ftSq2inSq(double feetSq)
{
    return feetSq*144;
}

double min2s(double minutes)
{
    return 60*minutes;
}

double s2min(double seconds)
{
    return seconds / 60.0;
}

double s2hr(double seconds)
{
    return seconds / 3600.0;
}
