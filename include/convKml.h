
#ifndef  _CONVKML_H
#define  _CONVKML_H

#include"common.h"



extern int convkml(char *infile[], char *outfile[], gtime_t ts,
    gtime_t te, int nfile, double tint, int qflg, double *offset,
    int tcolor, int pcolor, int outalt, int outtime);



#endif