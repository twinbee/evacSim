#ifndef CONVERT_H
#define CONVERT_H

#include <string>

float randomN(float min, float max);
int RoundUp(double x);
int nextpoweroftwo(int x);

double d2r(double a);
double r2d(double a);

//length
double m2ft(double meters);
double ft2m(double feet);
double ft2mi(double feet);
double mi2ft(double miles);
double ft2in(double feet);
double in2ft(double inches);

//area
double mSq2ftSq(double meterssquared);
double ftSq2mSq(double feetsquared);
double inSq2ftSq(double inchesSq);
double ftSq2inSq(double feetSq);

//time
double days2s(double days );
double hr2s(double hours);
double hr2min(double hours);
double hr2days(double hours);
double min2s(double minutes);
double s2min(double seconds);
double s2hr(double seconds);

#endif
