

#include"./include/convKml.h"

int main() {

    char *infile = {
    "G:\\googleEarth\\myRTK.pos"
    };

    char *outfile = {"G:\\googleEarth\\myRTK.kml"};
    gtime_t ts = { 0 }, te = {0};
    double tint = 0.0;
    int qflg = 0;
    double offset[3] = { 0 };
    int tcolor = 4;
    int pcolor = 5;
    int outalt = 0;
    int outtime = 1;

    int stat= convkml(infile, outfile, ts,
        te, 1,tint, qflg, offset,
        tcolor,  pcolor,  outalt, outtime);

    system("pause");

    return 0;
}