/*------------------------------------------------------------------------------
* solution.c : solution functions
*
*          Copyright (C) 2007-2020 by T.TAKASU, All rights reserved.
*
* references :
*     [1] National Marine Electronic Association and International Marine
*         Electronics Association, NMEA 0183 version 4.10, August 1, 2012
*     [2] NMEA 0183 Talker Identifier Mnemonics, March 3, 2019
*         (https://www.nmea.org/content/STANDARDS/NMEA_0183_Standard)
*
* version : $Revision:$ $Date:$
* history : 2007/11/03  1.0 new
*           2009/01/05  1.1  add function outsols(), outsolheads(),
*                            setsolformat(), outsolexs, outsolex
*           2009/04/02  1.2  add dummy fields in NMEA mesassage
*                            fix bug to format lat/lon as deg-min-sec
*           2009/04/14  1.3  add age and ratio field to solution
*           2009/11/25  1.4  add function readsolstat()
*           2010/02/14  1.5  fix bug on output of gpstime at week boundary
*           2010/07/05  1.6  added api:
*                                initsolbuf(),freesolbuf(),addsol(),getsol(),
*                                inputsol(),outprcopts(),outprcopt()
*                            modified api:
*                                readsol(),readsolt(),readsolstat(),
*                                readsolstatt(),outsolheads(),outsols(),
*                                outsolexs(),outsolhead(),outsol(),outsolex(),
*                                outnmea_rmc(),outnmea_gga(),outnmea_gsa(),
*                                outnmea_gsv()
*                            deleted api:
*                                setsolopt(),setsolformat()
*           2010/08/14  1.7  fix bug on initialize solution buffer (2.4.0_p2)
*                            suppress enu-solution if base pos not available
*                            (2.4.0_p3)
*           2010/08/16  1.8  suppress null record if solution is not available
*                            (2.4.0_p4)
*           2011/01/23  1.9  fix bug on reading nmea solution data
*                            add api freesolstatbuf()
*           2012/02/05  1.10 fix bug on output nmea gpgsv
*           2013/02/18  1.11 support nmea GLGSA,GAGSA,GLCSV,GACSV sentence
*           2013/09/01  1.12 fix bug on presentation of nmea time tag
*           2015/02/11  1.13 fix bug on checksum of $GLGSA and $GAGSA
*                            fix bug on satellite id of $GAGSA
*           2016/01/17  1.14 support reading NMEA GxZDA
*                            ignore NMEA talker ID
*           2016/07/30  1.15 suppress output if std is over opt->maxsolstd
*           2017/06/13  1.16 support output/input of velocity solution
*           2018/10/10  1.17 support reading solution status file
*           2020/11/30  1.18 add NMEA talker ID GQ and GI (NMEA 0183 4.11)
*                            add NMEA GQ/GB/GI-GSA/GSV sentences
*                            change talker ID GP to GN for NMEA RMC/GGA
*                            change newline to "\r\n" in SOLF_LLH,XYZ,ENU
*                            add reading age information in NMEA GGA
*                            use integer types in stdint.h
*                            suppress warnings
*-----------------------------------------------------------------------------*/
#include <ctype.h>
#include "../include/common.h"
#include<math.h>

/* constants and macros ------------------------------------------------------*/

#define SQR(x)     ((x)<0.0?-(x)*(x):(x)*(x))
#define SQRT(x)    ((x)<0.0||(x)!=(x)?0.0:sqrt(x))

#define NMEA_TID   "GN"         /* NMEA talker ID for RMC and GGA sentences */
#define MAXFIELD   64           /* max number of fields in a record */
#define MAXNMEA    256          /* max length of nmea sentence */

#define KNOT2M     0.514444444  /* m/knot */
#define MAXFIELD   64           /* max number of fields in a record */


/* decode lat/lon/height -----------------------------------------------------*/
static int decode_custom(char *buff, const solopt_t *opt, sol_t *sol)
{
    double pos[3] = { 0 }, val[MAXFIELD] = {0},std[3] = { 0 };
    int flag = 0;
    double dop=0.0;
    double utctime = 0.0;
    double t = 0.0;

    sscanf(buff, "%lf  %lf  %lf  %lf  %lf  %lf  %lf   %d %lf", &utctime, 
        val, val + 1, val + 2, val+3, val + 4, val + 5, &flag, &dop);

    pos[0] = val[0] * D2R; /* lat/lon/hgt (ddd.ddd) */
    pos[1] = val[1] * D2R;
    pos[2] = val[2];

    sol->time.time = (__int64)(utctime);
    sol->time.sec =  utctime-sol->time.time;
    pos2ecef(pos, sol->rr);

    sol->stat = (uint8_t)val[6];

    return 1;
}



/* decode solution position --------------------------------------------------*/
static int decode_solpos(char *buff, const solopt_t *opt, sol_t *sol)
{
    sol_t sol0 = { { 0 } };
    char *p = buff;


    //trace(4, "decode_solpos: buff=%s\n", buff);

    *sol = sol0;

    /* decode solution time */
  /*  if (!(p = decode_soltime(p, opt, &sol->time))) {
        return 0;
    }*/
    /* decode solution position */
  
    return decode_custom(p, opt, sol);  //by xdx 2021/1/14
    
   
}

/* decode solution -----------------------------------------------------------*/
static int decode_sol(char *buff, const solopt_t *opt, sol_t *sol, double *rb)
{

#if 0
    char *p;

    //trace(4, "decode_sol: buff=%s\n", buff);

    //if (test_nmea(buff)) { /* decode nmea */
    //    return decode_nmea(buff, sol);
    //}
    //else if (test_solstat(buff)) { /* decode solution status */
    //    return decode_solsss(buff, sol);
    //}
    //if (!strncmp(buff, COMMENTH, 1)) { /* reference position */
    //    if (!strstr(buff, "ref pos") && !strstr(buff, "slave pos")) return 0;
    //    if (!(p = strchr(buff, ':'))) return 0;
    //    decode_refpos(p + 1, opt, rb);
    //    return 0;
    //}
    /* decode position record */
    return decode_solpos(buff, opt, sol);
#endif


    double pos[3] = { 0 }, val[MAXFIELD] = { 0 }, std[3] = { 0 };
    int flag = 0;
    double dop = 0.0;
    double utctime = 0.0;
    double t = 0.0;

    sscanf(buff, "%lf  %lf  %lf  %lf  %lf  %lf  %lf   %d %lf", &utctime,
        val, val + 1, val + 2, val + 3, val + 4, val + 5, &flag, &dop);

    pos[0] = val[0] * D2R; /* lat/lon/hgt (ddd.ddd) */
    pos[1] = val[1] * D2R;
    pos[2] = val[2];

    sol->time.time = (__int64)(utctime);
    sol->time.sec = utctime - sol->time.time;
    pos2ecef(pos, sol->rr);

    sol->stat = (uint8_t)val[6];

    return 1;
}
/* decode solution options ---------------------------------------------------*/
static void decode_solopt(char *buff, solopt_t *opt)
{
    char *p;

    //trace(4, "decode_solhead: buff=%s\n", buff);

    if (strncmp(buff, COMMENTH, 1) && strncmp(buff, "+", 1)) return;

    if (strstr(buff, "GPST")) opt->times = TIMES_GPST;
    else if (strstr(buff, "UTC")) opt->times = TIMES_UTC;
    else if (strstr(buff, "JST")) opt->times = TIMES_JST;

    if ((p = strstr(buff, "x-ecef(m)"))) {
        opt->posf = SOLF_XYZ;
        opt->degf = 0;
        strncpy(opt->sep, p + 9, 1);
        opt->sep[1] = '\0';
    }
    else if ((p = strstr(buff, "latitude(d'\")"))) {
        opt->posf = SOLF_LLH;
        opt->degf = 1;
        strncpy(opt->sep, p + 14, 1);
        opt->sep[1] = '\0';
    }
    else if ((p = strstr(buff, "latitude(deg)"))) {
        opt->posf = SOLF_LLH;
        opt->degf = 0;
        strncpy(opt->sep, p + 13, 1);
        opt->sep[1] = '\0';
    }
    else if ((p = strstr(buff, "e-baseline(m)"))) {
        opt->posf = SOLF_ENU;
        opt->degf = 0;
        strncpy(opt->sep, p + 13, 1);
        opt->sep[1] = '\0';
    }
    else if ((p = strstr(buff, "+SITE/INF"))) { /* gsi f2/f3 solution */
        opt->times = TIMES_GPST;
        opt->posf = SOLF_GSIF;
        opt->degf = 0;
        strcpy(opt->sep, " ");
    }
}
/* read solution option ------------------------------------------------------*/
static void readsolopt(FILE *fp, solopt_t *opt)
{
    char buff[MAXSOLMSG + 1];
    int i;

    for (i = 0;fgets(buff, sizeof(buff), fp) && i<100;i++) { /* only 100 lines */
        decode_solopt(buff, opt);
    }
}

/* add solution data to solution buffer ----------------------------------------
* add solution data to solution buffer
* args   : solbuf_t *solbuf IO solution buffer
*          sol_t  *sol      I  solution data
* return : status (1:ok,0:error)
*-----------------------------------------------------------------------------*/
extern int addsol(solbuf_t *solbuf, const sol_t *sol)
{
    sol_t *solbuf_data;

    if (solbuf->cyclic) { /* ring buffer */
        if (solbuf->nmax <= 1) return 0;
        solbuf->data[solbuf->end] = *sol;
        if (++solbuf->end >= solbuf->nmax) solbuf->end = 0;
        if (solbuf->start == solbuf->end) {
            if (++solbuf->start >= solbuf->nmax) solbuf->start = 0;
        }
        else solbuf->n++;

        return 1;
    }
    if (solbuf->n >= solbuf->nmax) {
        solbuf->nmax = solbuf->nmax == 0 ? 8192 : solbuf->nmax * 2;
        if (!(solbuf_data = (sol_t *)realloc(solbuf->data, sizeof(sol_t)*solbuf->nmax))) {
            free(solbuf->data); solbuf->data = NULL; solbuf->n = solbuf->nmax = 0;
            return 0;
        }
        solbuf->data = solbuf_data;
    }
    solbuf->data[solbuf->n++] = *sol;
    return 1;
}

/* input solution data from stream ---------------------------------------------
* input solution data from stream
* args   : uint8_t data     I stream data
*          gtime_t ts       I  start time (ts.time==0: from start)
*          gtime_t te       I  end time   (te.time==0: to end)
*          double tint      I  time interval (0: all)
*          int    qflag     I  quality flag  (0: all)
*          solbuf_t *solbuf IO solution buffer
* return : status (1:solution received,0:no solution,-1:disconnect received)
*-----------------------------------------------------------------------------*/
extern int inputsol(uint8_t data, gtime_t ts, gtime_t te, double tint,
    int qflag, const solopt_t *opt, solbuf_t *solbuf)
{
    sol_t sol = { { 0 } };
    int stat;

    sol.time = solbuf->time;

    if (data == '$' || (!isprint(data) && data != '\r'&&data != '\n')) { /* sync header */
        solbuf->nb = 0;
    }
    if (data != '\r'&&data != '\n') {
        solbuf->buff[solbuf->nb++] = data;
        printf("%c\n",data);
    }
    if (data != '\n'&&solbuf->nb<MAXSOLMSG) return 0; /* sync trailer */

    solbuf->buff[solbuf->nb] = '\0';
    solbuf->nb = 0;

    /* check disconnect message */
    if (!strncmp((char *)solbuf->buff, MSG_DISCONN, strlen(MSG_DISCONN) - 2)) {
      //  trace(3, "disconnect received\n");
        return -1;
    }
    /* decode solution */
    sol.time = solbuf->time;
    if ((stat = decode_sol((char *)solbuf->buff, opt, &sol, solbuf->rb))>0) {
        if (stat) solbuf->time = sol.time; /* update current time */
        if (stat != 1) return 0;
    }
    if (stat != 1 || !screent(sol.time, ts, te, tint) || (qflag&&sol.stat != qflag)) {
        return 0;
    }
    /* add solution to solution buffer */
    return addsol(solbuf, &sol);
}
/* read solution data --------------------------------------------------------*/
static int readsoldata(FILE *fp, gtime_t ts, gtime_t te, double tint, int qflag,
    const solopt_t *opt, solbuf_t *solbuf)
{
    int c;

    while ((c = fgetc(fp)) != EOF) {
        /* input solution */
        inputsol((uint8_t)c, ts, te, tint, qflag, opt, solbuf);
    }
    return solbuf->n>0;
}
/* compare solution data -----------------------------------------------------*/
static int cmpsol(const void *p1, const void *p2)
{
    sol_t *q1 = (sol_t *)p1, *q2 = (sol_t *)p2;
    double tt = timediff(q1->time, q2->time);
    return tt<-0.0 ? -1 : (tt>0.0 ? 1 : 0);
}
/* sort solution data --------------------------------------------------------*/
static int sort_solbuf(solbuf_t *solbuf)
{
    sol_t *solbuf_data;

    //trace(4, "sort_solbuf: n=%d\n", solbuf->n);

    if (solbuf->n <= 0) return 0;

    if (!(solbuf_data = (sol_t *)realloc(solbuf->data, sizeof(sol_t)*solbuf->n))) {
       // trace(1, "sort_solbuf: memory allocation error\n");
        free(solbuf->data); solbuf->data = NULL; solbuf->n = solbuf->nmax = 0;
        return 0;
    }
    solbuf->data = solbuf_data;
    qsort(solbuf->data, solbuf->n, sizeof(sol_t), cmpsol);
    solbuf->nmax = solbuf->n;
    solbuf->start = 0;
    solbuf->end = solbuf->n - 1;
    return 1;
}

/* initialize solution buffer --------------------------------------------------
* initialize position solutions
* args   : solbuf_t *solbuf I  solution buffer
*          int    cyclic    I  solution data buffer type (0:linear,1:cyclic)
*          int    nmax      I  initial number of solution data
* return : status (1:ok,0:error)
*-----------------------------------------------------------------------------*/
extern void initsolbuf(solbuf_t *solbuf, int cyclic, int nmax)
{
#if 0
    gtime_t time0 = { 0 };
#endif
    int i;

   // trace(3, "initsolbuf: cyclic=%d nmax=%d\n", cyclic, nmax);

    solbuf->n = solbuf->nmax = solbuf->start = solbuf->end = solbuf->nb = 0;
    solbuf->cyclic = cyclic;
#if 0
    solbuf->time = time0;
#endif
    solbuf->data = NULL;
    for (i = 0;i<3;i++) {
        solbuf->rb[i] = 0.0;
    }
    if (cyclic) {
        if (nmax <= 2) nmax = 2;
        if (!(solbuf->data =(sol_t*) malloc(sizeof(sol_t)*nmax))) {
           // trace(1, "initsolbuf: memory allocation error\n");
            return;
        }
        solbuf->nmax = nmax;
    }
}


/* free solution ---------------------------------------------------------------
* free memory for solution buffer
* args   : solbuf_t *solbuf I  solution buffer
* return : none
*-----------------------------------------------------------------------------*/
extern void freesolbuf(solbuf_t *solbuf)
{
    int i;
    free(solbuf->data);
    solbuf->n = solbuf->nmax = solbuf->start = solbuf->end = solbuf->nb = 0;
    solbuf->data = NULL;
    for (i = 0;i<3;i++) {
        solbuf->rb[i] = 0.0;
    }
}

/* read solutions data from solution files -------------------------------------
* read solution data from soluiton files
* args   : char   *files[]  I  solution files
*          int    nfile     I  number of files
*         (gtime_t ts)      I  start time (ts.time==0: from start)
*         (gtime_t te)      I  end time   (te.time==0: to end)
*         (double tint)     I  time interval (0: all)
*         (int    qflag)    I  quality flag  (0: all)
*          solbuf_t *solbuf O  solution buffer
* return : status (1:ok,0:no data or error)
*-----------------------------------------------------------------------------*/
extern int readsolt(char *files, int nfile, gtime_t ts, gtime_t te,
    double tint, int qflag, solbuf_t *solbuf)
{
    FILE *fp;
    solopt_t opt = solopt_default;
    int i;

    //trace(3, "readsolt: nfile=%d\n", nfile);

    initsolbuf(solbuf, 0, 0);

    for (i = 0;i<nfile;i++) {
        if (!(fp = fopen(files, "rb"))) {
           // trace(2, "readsolt: file open error %s\n", files[i]);
            continue;
        }
        /* read solution options in header */
       // readsolopt(fp, &opt);
        rewind(fp);

        /* read solution data */
        if (!readsoldata(fp, ts, te, tint, qflag, &opt, solbuf)) {
          //  trace(2, "readsolt: no solution in %s\n", files[i]);
        }
        fclose(fp);
    }
    return sort_solbuf(solbuf);
}
//extern int readsol(char *files[], int nfile, solbuf_t *sol)
//{
//    gtime_t time = { 0 };
//
//    //trace(3, "readsol: nfile=%d\n", nfile);
//
//    return readsolt(files, nfile, time, time, 0.0, 0, sol);
//}

/* get solution data from solution buffer --------------------------------------
* get solution data by index from solution buffer
* args   : solbuf_t *solbuf I  solution buffer
*          int    index     I  index of solution (0...)
* return : solution data pointer (NULL: no solution, out of range)
*-----------------------------------------------------------------------------*/
extern sol_t *getsol(solbuf_t *solbuf, int index)
{
    //trace(4, "getsol: index=%d\n", index);

    if (index<0 || solbuf->n <= index) return NULL;
    if ((index = solbuf->start + index) >= solbuf->nmax) {
        index -= solbuf->nmax;
    }
    return solbuf->data + index;
}

