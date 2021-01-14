

#include"./include/convKml.h"
#include<iostream>

int main(int argc,char *argv[]) {

    gtime_t ts = { 0 }, te = { 0 };
    double tint = 0.0;
    int qflg = 0;
    double offset[3] = { 0 };
    int tcolor = 4;
    int pcolor = 5;
    int outalt = 0;
    int outtime = 1;
    int nfile = 0;

    char  *infile[1024];

    for (int i = 0;i<MAXEXFILE;i++) {
        if (!(infile[i] = (char *)malloc(1024))) {
            for (i--;i >= 0;i--) free(infile[i]);
            return -4;
        }
    }

    std::cout <<0<< argv[0] << std::endl;
    std::cout <<1<< argv[1] << std::endl;
    std::cout <<2<< argv[2] << std::endl;
    std::cout << argc << std::endl;
    for (int i = 1;i < argc;i++) {
        strcpy(infile[i-1], argv[i]);
        std::cout << infile[i-1] << std::endl;
    }


    std::cout << argc - 1 << std::endl;
    nfile = argc - 1;
    system("pause");

   /* nfile = argc-1;
    std::cout << nfile << std::endl;
    system("pause");*/
  /*  char *infile[] = {
        {"G:\\googleEarth\\myRTK.pos"},
        {"G:\\googleEarth\\myRTK - ¸±±¾.pos"},
    };*/

    /*char *outfile[] = {
        
        {""},
        {""},
    };*/

    int stat= convkml(infile, NULL, ts,
        te, 2,tint, qflg, offset,
        tcolor,  pcolor,  outalt, outtime);

    for (int i = 0;i<MAXEXFILE;i++) {
        free(infile[i]);
    }

    system("pause");

    return 0;
}