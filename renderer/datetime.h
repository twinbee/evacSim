#ifndef DATETIME_H
#define DATETIME_H

#include <string>

enum {DTWEEKDAY = 1, DTMONTH = 2,DTDAY = 4,DTHOUR = 8, DT12HR = 16,
      DTMIN = 32,DTSEC = 64,DTYEAR = 128
     };

std::string datetime (int flags = (DTMONTH | DTDAY | DTHOUR | DT12HR |
                                   DTMIN | DTSEC) );
std::string datetime_ (int flags = (DTMONTH | DTDAY | DTHOUR | DT12HR |
                                   DTMIN | DTSEC) );

#endif
