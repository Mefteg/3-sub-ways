#ifndef __UTILS__
#define __UTILS__

#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <fstream>
#include <vector>
#include <string>

using namespace std;

class Utils {
    public:

        static void explode(string, string, vector<string> *);
        static int stringToInt(string);
        static float stringToFloat(string);
};

#endif
