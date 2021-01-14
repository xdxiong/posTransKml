
#ifndef _COMMON_H
#define _COMMON_H

#ifdef WIN32
#include <winsock2.h>
#include <windows.h>
#else
#include <pthread.h>
#include <sys/select.h>
#endif


#include<cstring>
#include<cstdio>
#include<ctime>
#include<cstdint>
#include<cstdlib>


#ifdef __cplusplus
extern "C" {
#endif

#define DTTOL       0.025               /* tolerance of time difference (s) */
#define RE_WGS84    6378137.0           /* earth semimajor axis (WGS84) (m) */
#define FE_WGS84    (1.0/298.257223563)  /* earth flattening (WGS84) */
#define MAXSBSMSG   (32)                  /* max number of SBAS msg in RTK server */
#define MAXSOLMSG   (8191)                /* max length of solution message */

#define MAXEXFILE   1024                /* max number of expanded files */
#define MAXLEAPS    64                  /* max number of leap seconds table */
#define PI          (3.1415926535897932)  /* pi */
#define D2R         (PI/180.0)          /* deg to rad */
#define R2D         (180.0/PI)          /* rad to deg */
#define CLIGHT      (299792458.0)         /* speed of light (m/s)*/
#define SOLF_LLH    0                   /* solution format: lat/lon/height */
#define SOLF_XYZ    1                   /* solution format: x/y/z-ecef */
#define SOLF_ENU    2                   /* solution format: e/n/u-baseline */
#define SOLF_NMEA   3                   /* solution format: NMEA-183 */
#define SOLF_STAT   4                   /* solution format: solution status */
#define SOLF_GSIF   5                   /* solution format: GSI F1/F2 */
#define SOLF_CUSTOM 6

#define TIMES_GPST  0                   /* time system: gps time */
#define TIMES_UTC   1                   /* time system: utc */
#define TIMES_JST   2                   /* time system: jst */

#define COMMENTH    "%"                 /* comment line indicator for solution */
#define MSG_DISCONN "$_DISCONNECT\r\n"  /* disconnect message */


typedef struct {        /* time struct */
    time_t time;        /* time (s) expressed by standard time_t */
    double sec;         /* fraction of second under 1 s */
}gtime_t;

typedef struct {        /* solution type */
    gtime_t time;       /* time (GPST) */
    double rr[6];       /* position/velocity (m|m/s) */
                        /* {x,y,z,vx,vy,vz} or {e,n,u,ve,vn,vu} */
    float  qr[6];       /* position variance/covariance (m^2) */
                        /* {c_xx,c_yy,c_zz,c_xy,c_yz,c_zx} or */
                        /* {c_ee,c_nn,c_uu,c_en,c_nu,c_ue} */
    float  qv[6];       /* velocity variance/covariance (m^2/s^2) */
    double dtr[6];      /* receiver clock bias to time systems (s) */
    uint8_t type;       /* type (0:xyz-ecef,1:enu-baseline) */
    uint8_t stat;       /* solution status (SOLQ_???) */
    uint8_t ns;         /* number of valid satellites */
    float age;          /* age of differential (s) */
    float ratio;        /* AR ratio factor for valiation */
    float thres;        /* AR ratio threshold for valiation */
} sol_t;

typedef struct {        /* solution buffer type */
    int n,nmax;         /* number of solution/max number of buffer */
    int cyclic;         /* cyclic buffer flag */
    int start,end;      /* start/end index */
    gtime_t time;       /* current solution time */
    sol_t *data;        /* solution data */
    double rb[3];       /* reference position {x,y,z} (ecef) (m) */
    uint8_t buff[MAXSOLMSG+1]; /* message buffer */
    int nb;             /* number of byte in message buffer */
} solbuf_t;


typedef struct {        /* solution options type */
    int posf;           /* solution format (SOLF_???) */
    int times;          /* time system (TIMES_???) */
    int timef;          /* time format (0:sssss.s,1:yyyy/mm/dd hh:mm:ss.s) */
    int timeu;          /* time digits under decimal point */
    int degf;           /* latitude/longitude format (0:ddd.ddd,1:ddd mm ss) */
    int outhead;        /* output header (0:no,1:yes) */
    int outopt;         /* output processing options (0:no,1:yes) */
    int outvel;         /* output velocity options (0:no,1:yes) */
    int datum;          /* datum (0:WGS84,1:Tokyo) */
    int height;         /* height (0:ellipsoidal,1:geodetic) */
    int geoid;          /* geoid model (0:EGM96,1:JGD2000) */
    int solstatic;      /* solution of static mode (0:all,1:single) */
    int sstat;          /* solution statistics level (0:off,1:states,2:residuals) */
    int trace;          /* debug trace level (0:off,1-5:debug) */
    double nmeaintv[2]; /* nmea output interval (s) (<0:no,0:all) */
                        /* nmeaintv[0]:gprmc,gpgga,nmeaintv[1]:gpgsv */
    char sep[64];       /* field separator */
    char prog[64];      /* program name */
    double maxsolstd;   /* max std-dev for solution output (m) (0:all) */
} solopt_t;

typedef struct {        /* solution status type */
    gtime_t time;       /* time (GPST) */
    uint8_t sat;        /* satellite number */
    uint8_t frq;        /* frequency (1:L1,2:L2,...) */
    float az, el;        /* azimuth/elevation angle (rad) */
    float resp;         /* pseudorange residual (m) */
    float resc;         /* carrier-phase residual (m) */
    uint8_t flag;       /* flags: (vsat<<5)+(slip<<3)+fix */
    uint16_t snr;       /* signal strength (*SNR_UNIT dBHz) */
    uint16_t lock;      /* lock counter */
    uint16_t outc;      /* outage counter */
    uint16_t slipc;     /* slip counter */
    uint16_t rejc;      /* reject counter */
} solstat_t;

typedef struct {        /* solution status buffer type */
    int n, nmax;         /* number of solution/max number of buffer */
    solstat_t *data;    /* solution status data */
} solstatbuf_t;

extern const solopt_t solopt_default;

extern gtime_t gpst2utc(gtime_t t);
extern void time2epoch(gtime_t t, double *ep);

extern gtime_t timeadd(gtime_t t, double sec);
extern double timediff(gtime_t t1, gtime_t t2);

extern gtime_t epoch2time(const double *ep);

extern void ecef2pos(const double *r, double *pos);
extern double dot(const double *a, const double *b, int n);
extern double norm(const double *a, int n);

extern void enu2ecef(const double *pos, const double *e, double *r);
extern void xyz2enu(const double *pos, double *E);
extern void matmul(const char *tr, int n, int k, int m, double alpha,
                   const double *A, const double *B, double beta, double *C);


extern int readsolt(char *files, int nfile, gtime_t ts, gtime_t te,
    double tint, int qflag, solbuf_t *solbuf);


extern double time2gpst(gtime_t t, int *week);
extern int screent(gtime_t time, gtime_t ts, gtime_t te, double tint);
extern void covecef(const double *pos, const double *Q, double *P);
extern int addsol(solbuf_t *solbuf, const sol_t *sol);
extern void initsolbuf(solbuf_t *solbuf, int cyclic, int nmax);
extern void pos2ecef(const double *pos, double *r);
extern void freesolbuf(solbuf_t *solbuf);

#ifdef __cplusplus
}
#endif

#endif
