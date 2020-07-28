#ifndef DNest5_Skilling_hpp
#define DNest5_Skilling_hpp

#include <string.h>
#include <float.h>
#include <time.h>
#include <math.h>
#include <assert.h>

namespace DNest5
{

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Filename:  random64.h
// 
// Purpose:   Header for random64.c
// 
// History:   John Skilling
//            18 Feb 2012 - 24 Oct 2012, update to 64-bit-integer operation.
//            Keith Richardson, Microsoft kludges (edited JS)
//            JS Aug 2014  NumOnes added
//            JS Feb 2016  ProbCauchy2 and subsidiary routines added
//            JS Nov 2018  Knuth's subtractive 32-bit generator included
//            JS Dec 2018  Mersenne-twister generator included
//.............................................................................
//                          ==============
//                           CHANGE NOTES
//                          ==============
//     ***********
//     *  SETUP  *
//     ***********
// Generator:                Was Rand_t   = unsigned int[63].
//                           Now Rand64_t = unsigned long[4].
//
// Initialiser:              Was int RanInit(Rand_t,     int).
//                           Now int Ran64Init(Rand64_t, int).
//
// Multiple initialiser:       -
//                           Now int Ran64InitN(Rand64_t*, int, int).
//
//     *****************
//     * DISTRIBUTIONS *
//     *****************
// Uniform 64bit integer:      -
//                           Now unsigned long RanLong(Rand64_t).     [0,2^64)
//
// Uniform 32bit integer:    Was int      Ranint(Rand_t).
//                           Now unsigned RanInt(Rand64_t).           [0,2^32)
//                           Also Knuth's Knuth(Knuth_t)              [0,2^32)
//                            and Python's Mersenne(Mersenne_t).      [0,2^64)
//
// Uniform double precision: Was double Randouble(Rand_t).
//                           Now double RanDouble(Rand64_t).          (0.,1.)
//
// Uniform signed double:      -
//                           Now double RanSigned(Rand64_t).          (-1.,1.)
//
// Uniform single precision: Was float Ranfloat(Rand_t).
//                           Now float RanFloat(Rand64_t).            (0.f,1.f)
//
// Uniform bounded integer:  Was unsigned Rangrid(Rand_t,   unsigned).
//                           Now unsigned RanGrid(Rand64_t, unsigned)
//
// Exponential distribution:   -
//                           Now double RanExp(Rand64_t).
//
// Cauchy distribution:      Was double Rancauchy(Rand_t).
//                           Now double RanCauchy(Rand64_t).
//
// Cauchy2 X from V:         Now double RanCauchyXfromV(Rand64_t, double).
//
// Cauchy2 V from X:         Now double RanCauchyVfromX(Rand64_t, double).
//
// Normal distribution:      Was double Rangauss(Rand_t).
//                           Now double RanGauss(Rand64_t).
//
// Gamma distribution:       Was double Rangamma(Rand_t,   double).
//                           Now double RanGamma(Rand64_t, double).
//
// Beta distribution:        Was double Ranbeta(Rand_t,   int    r, int    n).
//                           Now double RanBeta(Rand64_t, double a, double b),
//                              where r = a/(a+b), n = a+b.                 !!!
//
// Dirichlet distribution      -
//  (as multiple Gammas)     Now double RanGamLogs(Rand64_t, int, double*,
//                                                                double*);
//
// Poisson distribution:     Was unsigned Ranpoiss(Rand_t, double).
//                           Now int      RanPoiss(Rand_t, double).
//
// Binomial distribution:    Was unsigned Ranbinom(Rand_t, unsigned, double).
//                           Now int      RanBinom(Rand_t, int,      double).
//
// Permutation:              Was void Ranperm(Rand_t,   int, int*).
//                           Now void RanPerm(Rand64_t, int, int*).
//
// Random direction:         Was void Ransphere(Rand_t,   int, double*).
//                           Now void RanSphere(Rand64_t, int, double*).
//
// Random direction:         Was void Ransimplex(Rand_t,   double,int,double*).
//                           Now void RanSimplex(Rand64_t, double,int,double*).
//
//     *************
//     * FUNCTIONS *
//     *************
// Cauchy joint prob:        Now double ProbCauchy2(double, double).
//
// Gamma (as log):           Was double logGamma(double), renamed loggamma. !!!
//                           Now double logGamma(double).
//
// Factorial (as log):       Was double logfactorial(unsigned).
//                           Now double logFactorial(unsigned).
//
// # of '1' bits in integer    -
//                           Now int NumOnes32(unsigned),      for 32-bit,
//                           and int NumOnes64(unsigned long), for 64-bit.
//
// Calendar date (as string):  -
//                           Now void RanDate(char* date, int seed).
//-----------------------------------------------------------------------------
#define slong long
#define ulong unsigned long

/* Microsoft kludges
#define slong __int64             (long long)
#define ulong unsigned __int64    (unsigned long long)
*/

typedef ulong    Rand64_t[4];
typedef unsigned Knuth_t[58];
typedef ulong    Mersenne_t[314];

extern double ulong_to_double(ulong u);

extern int Ran64Init(      //   O  +ve seed value used, from input or time.
Rand64_t   Rand,           //   O  Random generator state              ulong[4]
int        seed);          // I    Seed: +ve = value, -ve = time seed

extern int Ran64InitN(     //   O  +ve seed value used, from input or time.
ulong**    pRand,          //   O  &(Random generator states)               [N]
int        seed,           // I    Seed: +ve = value, -ve = time seed
int        N);             // I    # random generators

extern int KnuthInit(      //   O  +ve seed value used, from input or time.
Knuth_t    Rand,           //   O  Random generator state          unsigned[58]
int        seed);          // I    Seed: +ve = value, -ve = time seed

extern unsigned Knuth(     //   O  32-bit integer
Knuth_t   Rand);           // I O  Knuth's random generator state  unsigned[58]

extern int MersenneInit(   //   O  +ve seed value used, from input or time.
Mersenne_t Rand,           //   O  Random generator state            ulong[314]
int        seed);          // I    Seed: +ve = value, -ve = time seed

extern ulong Mersenne(     //   O  64-bit integer
Mersenne_t   Rand);          // I O  Mersenne twister                ulong[314]

extern ulong RanLong(      //   O  Random sample from Uniform[0, 2^64)
Rand64_t   Rand);          // I O  Random generator state

extern unsigned RanInt(    //   O  Random sample from Uniform[0, 2^32)
Rand64_t   Rand);          // I O  Random generator state

extern float RanFloat(     //   O  Random odd multiple of 2^-24 in (0,1)
Rand64_t   Rand);          // I O  Random generator state

extern double RanDouble(   //   O  Random odd multiple of 2^-53 in (0,1)
Rand64_t   Rand);          // I O  Random generator state

extern double RanSigned(   //   O  Random odd multiple of 2^-53 in (-1,1)
Rand64_t   Rand);          // I O  Random generator state

extern unsigned RanGrid(   //   O  Random sample from Uniform[0, bound)
Rand64_t   Rand,           // I O  Random generator state
unsigned   bound);         // I    Supremum (0 yields 0)

extern double RanExp(      //   O  Random sample from exponential
Rand64_t   Rand);          // I O  Random generator state

extern double RanCauchy(   //   O  Random sample from Cauchy
Rand64_t   Rand);          // I O  Random generator state

extern double RanCauchyXfromV( //  (see ProbCauchy2)
                           //   O  Conditional Cauchy sample of location
Rand64_t   Rand,           // I O  Random generator state
double     v);             // I    Velocity

extern double RanCauchyVfromX( //  (see ProbCauchy2)
                           //   O  Conditional Cauchy sample of velocity
Rand64_t   Rand,           // I O  Random generator state
double     x);             // I    Location

extern double RanGauss(    //   O  Random sample from normal
Rand64_t   Rand);          // I O  Random generator state

extern double RanGamma(    //   O  Random sample from gamma
Rand64_t   Rand,           // I O  Random generator state
double     c);             // I    Exponent > 0.0

extern double RanBeta(     //   O  Random sample from beta
Rand64_t   Rand,           // I O  Random generator state
double     a,              // I    Exponent > 0.0
double     b);             // I    Exponent > 0.0

extern double RanGamLogs(  //   O  log(SUM samples)
Rand64_t   Rand,           // I O  Random generator state
int        n,              // I    dimension
double*    logx,           //   O  log(samples) from gammas                 [n]
double*    c);             // I    exponents > 0.0                          [n]

extern int RanPoiss(       //   O  Random sample from Poisson
Rand64_t   Rand,           // I O  Random generator state
double     c);             // I    Mean > 0

extern int RanBinom(       //   O  Random sample from binomial
Rand64_t   Rand,           // I O  Random generator state
int        n,              // I    Range
double     p);             // I    Mean/Range

extern void RanPerm( 
Rand64_t   Rand,           // I O  Random generator state
int        n,              // I    Dimension
int*       perm);          //   O  Output permutation

extern void RanSphere(
Rand64_t   Rand,           // I O  random generator
int        n,              // I    dimension
double*    v);             //   O  random vector, unit length v'.v = 1      [n]

extern void RanSimplex(
Rand64_t   Rand,           // I O  random generator
double     s,              // I    SUM(v)
int        n,              // I    dimension
double*    v);             //   O  vector being coloured                    [n]

extern double ProbCauchy2( //   O  Joint probability
double     x,              // I    First  unit-Cauchy variable (equal  weights)
double     y);             // I    Second unit-Cauchy variable (linear weights)

extern double logGamma(    //   O  log of Gamma function
double     x);             // I    Argument

extern double logFactorial(//   O  log of factorial
unsigned   k);             // I    Argument

extern int NumOnes32(      //   O  number of '1' bits set
unsigned   k);             // I    32-bit integer

extern int NumOnes64(      //   O  number of '1' bits set
ulong      k);             // I    64-bit integer

extern void RanDate(
char*      date,           //   O  user's 25-char string, incl. NULL terminator
int        seed);          // I    time-seed as actually used
//.............................................................................


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Filename:  random64.c
// 
// Purpose:   Random utility procedures.
// 
// History:   From random.c,  17 Nov 1994 - 19 Dec 2018.
//
// Acknowledgments:
//   "Numerical Recipes", Press et al, (third edition) for ideas.
//   "Handbook of Mathematical Functions", Abramowitz and Stegun, for formulas.
//-----------------------------------------------------------------------------
/*
    Copyright (c) 1994-2019 Maximum Entropy Data Consultants Ltd,
                            115c Milton Road, Cambridge CB4 1XE, England

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

#include "license.txt"
*/


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Functions: Ran64Init
//            Ran64InitN
//
// Purpose:   Initialise unsigned long[4] generator (or multiple generators)
//            in preparation for other Random procedures.
//
// Format:    Rand[0] = 64bit call counter
//            Rand[1] = state for method A2, period 2^64 - 1
//            Rand[2] = state for method B2, period 4294963023 * 2^31 - 1
//            Rand[3] = state for method C3',period 2^64
//
// Method:    Initialise Rand[1,2,3] with successive random numbers from
//            method B3, guaranteed to be within the cycles of A2,B2,C3',
//            and continue until all generators are differently initialised.
//
// Note:      Requires standard 64-bit integers to be available.
//
// Ref:       Ideas from Numerical Recipes, 3rd edition, section 7.1.
//
// History:   John Skilling  18 Feb 2012
//-----------------------------------------------------------------------------
int Ran64Init(        //   O  +ve seed value used, either from input or time.
Rand64_t   Rand,      //   O  Random generator state                   ulong[4]
int        seed)      // I    Seed: +ve = value, -ve = time seed
{
    ulong* pRand = Rand;   // explicit pointer to Rand
    return Ran64InitN(&pRand, seed, 1);
}
int Ran64InitN(       //   O  +ve seed value used, either from input or time.
ulong**    pRand,     //   O  &(Random generator states)                 [N][4]
int        seed,      // I    Seed: +ve = value, -ve = time seed
int        N)         // I    # random generators
{
    ulong r;
    int   n;
// Check 64bit integers and IEEE floating point (else fail)
    double x = (double)0x0001020304050607;
    ulong* u = (ulong*)&x;
    assert( (ulong)(-1) >> 63 == 1 );
    assert( *u == 0x42f0203040506070 );

// User- or time-seed
    if( seed < 0 )                           // time(NULL) hits 2^31 in 2038AD
        seed = (int)time(NULL) & 0x7fffffff; // time(NULL) hits 2^32 in 2106AD
// Random generators
    r = (unsigned)(~seed);                   // guaranteed initialiser of B3
    for( n = 0; n < N; n++ )
    {
        pRand[n][0] = 0;
        pRand[n][1] = r = 0xf82179f3 * (r & 0xffffffff) + (r >> 32);    // B3
        pRand[n][2] = r = 0xf82179f3 * (r & 0xffffffff) + (r >> 32);    // B3
        pRand[n][3] = r = 0xf82179f3 * (r & 0xffffffff) + (r >> 32);    // B3
        RanLong(pRand[n]);                   // burn-in ...
        RanLong(pRand[n]);                   //   ... the generator to ...
        RanLong(pRand[n]);                   //            ... satisfy paranoia
        pRand[n][0] = 0;                     // zero the call counter
    }
    return seed;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Functions: KnuthInit
//
// Purpose:   Initialise unsigned[57] generator
//            in preparation for Knuth's subtractive procedure.
//
// Ref:       Ideas from Numerical Recipes, 2nd edition, section 7.1.
//
// History:   John Skilling  18 Feb 2012
//-----------------------------------------------------------------------------
int  KnuthInit(       //   O  +ve seed value used, either from input or time.
Knuth_t   Rand,       //   O  Random generator state               unsigned[58]
int       seed)       // I    Non-negative seed
{
	unsigned* R = Rand + 1;
    unsigned  i, j, k, m;
    Rand[0] = 0;
// User- or time-seed
    if( seed < 0 )                           // time(NULL) hits 2^31 in 2038AD
        seed = (int)time(NULL) & 0x7fffffff; // time(NULL) hits 2^32 in 2106AD
    m = (unsigned)(161803398 - seed);
    R[54] = m;
    k = 1;
    for( i = 1; i < 55; ++i )
    {
        j = (21 * i - 1) % 55;
        R[j] = k;
        k = m - k;
        m = R[j];
    }
    for( k = 0; k < 4; ++k )
        for( i = 0; i < 55; ++i )
            R[i] -= R[(i + 31) % 55];
    R[55] = 54;
    R[56] = 30;
    return seed;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  Knuth
//
// Purpose:   Random unsigned sample, in [0,2^32).
//
// Method:    Knuth's subtractive method ran3 in "Numerical Recipes" 2nd ed.,
//            re-coded for speed.  Period estimated as at least 10^480.
//            
// History:   John Skilling  6 May 1995, 12 Nov 2006
//-----------------------------------------------------------------------------
unsigned Knuth(       //   O  32-bit integer
Knuth_t  Rand)        // I O  Random generator state               unsigned[58]
{
static const int Roll[55] = {    1, 2, 3, 4, 5, 6, 7, 8, 9,
                             10,11,12,13,14,15,16,17,18,19,
                             20,21,22,23,24,25,26,27,28,29,
                             30,31,32,33,34,35,36,37,38,39,
                             40,41,42,43,44,45,46,47,48,49,
                             50,51,52,53,54, 0};
	unsigned* R = Rand + 1;
	Rand[0]++;
    R[55] = Roll[R[55]];
    R[56] = Roll[R[56]];
    return (R[R[55]] -= R[R[56]]);
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Functions: MersenneInit
//
// Purpose:   Initialise Mersenne generator.
//
// History:   John Skilling   2 Dec 2018
//-----------------------------------------------------------------------------
int  MersenneInit(    //   O  +ve seed value used, either from input or time.
Mersenne_t Rand,      //   O  Random generator state                 ulong[314]
int        seed)      // I    Non-negative seed
{
	int   i;
// User- or time-seed
    if( seed < 0 )                           // time(NULL) hits 2^31 in 2038AD
        seed = (int)time(NULL) & 0x7fffffff; // time(NULL) hits 2^32 in 2106AD
    Rand[0] = 0;                             // call counter
	Rand[1] = seed;                          // start initialise . . .
	for( i = 1; i < 312; i++ )               // . . . the generator state
		Rand[i+1] = 0x5851f42d4c957f2d * (Rand[i] ^ (Rand[i] >> 62)) + i;
	Rand[313] = 0;                           // pointer looping from 0 to 311
	return seed;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  Mersenne
//
// Purpose:   Random unsigned sample, in [0,2^64).  The Establishment standard.
//
// Method:    Mersenne twister.  Period = 2^19937 - 1.
//            For i=1,2,...,312 and repeat:
//               update Rand[i], "twisting" with Rand[i+1] and Rand[i+156],
//               then output "tempered" form of Rand[i].
//            
// History:   John Skilling   2 Dec 2018
//-----------------------------------------------------------------------------
ulong Mersenne(       //   O  32-bit integer
Mersenne_t Rand)      //   O  Random generator state                 ulong[314]
{
static const int Roll[468] = {
      0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12,
     13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
     26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38,
     39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51,
     52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64,
     65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77,
     78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90,
     91, 92, 93, 94, 95, 96, 97, 98, 99,100,101,102,103,
    104,105,106,107,108,109,110,111,112,113,114,115,116,
    117,118,119,120,121,122,123,124,125,126,127,128,129,
    130,131,132,133,134,135,136,137,138,139,140,141,142,
    143,144,145,146,147,148,149,150,151,152,153,154,155,
    156,157,158,159,160,161,162,163,164,165,166,167,168,
    169,170,171,172,173,174,175,176,177,178,179,180,181,
    182,183,184,185,186,187,188,189,190,191,192,193,194,
    195,196,197,198,199,200,201,202,203,204,205,206,207,
    208,209,210,211,212,213,214,215,216,217,218,219,220,
    221,222,223,224,225,226,227,228,229,230,231,232,233,
    234,235,236,237,238,239,240,241,242,243,244,245,246,
    247,248,249,250,251,252,253,254,255,256,257,258,259,
    260,261,262,263,264,265,266,267,268,269,270,271,272,
    273,274,275,276,277,278,279,280,281,282,283,284,285,
    286,287,288,289,290,291,292,293,294,295,296,297,298,
    299,300,301,302,303,304,305,306,307,308,309,310,311,
      0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12,
     13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
     26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38,
     39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51,
     52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64,
     65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77,
     78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90,
     91, 92, 93, 94, 95, 96, 97, 98, 99,100,101,102,103,
    104,105,106,107,108,109,110,111,112,113,114,115,116,
    117,118,119,120,121,122,123,124,125,126,127,128,129,
    130,131,132,133,134,135,136,137,138,139,140,141,142,
    143,144,145,146,147,148,149,150,151,152,153,154,155 };
static const int* Roll1   = Roll + 1;
static const int* Roll156 = Roll + 156;
	ulong* R = Rand + 1;
	ulong  t;
	int    i, j, k;
	Rand[0]++;       // call counter
// addressing (mod 312)
	i = R[312];      // current pointer, incremented each call
	j = Roll1[i];    // adjacent (i+1) mod 312
	k = Roll156[i];  // opposite (i+156) mod 312
	R[312] = j;      // incremented pointer
// twisting of R[i] by R[j] and R[k] --- the possibly weak single engine
	t = (R[i] & 0xffffffff80000000) + (R[j] & 0x000000007fffffff);
	R[i] = (t >> 1) ^ R[k];
	if( t & 1 )
		R[i] ^= 0xb5026f5aa96619e9;
// tempering of twisted R[i] --- an attempt to hide imperfections
	t = R[i];
	t ^= (t >> 29) & 0x0000000555555555;
	t ^= (t << 17) & 0x71d67fffeda60000;
	t ^= (t << 37) & 0xfff7eee000000000;
	t ^= (t >> 43);
	return t;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  RanLong
//
// Purpose:   Random integer sample, from Uniform[0,2^64).
//
// Method:    Ideas from Numerical Recipes, 3rd edition, section 7.1.
//            Methods A1 and A3 are replaced by A3 and A2, because it
//            might be better to keep the bit-shift counts close to 64.
//            Method B1 is replaced by B2, which has slightly longer period.
//            Method C3 is adjusted because "c=a" is allowed,
//            and increment ++ might be faster than addition +.
//
// Notes: (1) All random calls are directed through this procedure.
//        (2) Period is 3x10^57, almost 2^191, exactly
//            3138547745195092223933386588320749626261648328864846315520.
//        (3) Period exhaustion only begins to be detectable at P^(2/3) calls.
//
// History:   John Skilling  17 Feb 2012
//            JS              2 Dec 2018  Call counter increment set to 1
//-----------------------------------------------------------------------------
ulong RanLong(           //   O  Random sample from Uniform[0, 2^64)
Rand64_t   Rand)         // I O  Random generator state
{
    ulong R;
    Rand[0] ++;
    Rand[1] ^= Rand[1] >> 20;
    Rand[1] ^= Rand[1] << 41;
    Rand[1] ^= Rand[1] >> 5;                                            // A2
    Rand[2] = 0xffffef4f * (Rand[2] & 0xffffffff) + (Rand[2] >> 32);    // B2
    Rand[3] ++;
    Rand[3] = Rand[3] * 0x27bb2ee687b0b0fd;                             // C3'
    R = Rand[3] ^ (Rand[3] << 17);
    R ^= R >> 31;
    return  (Rand[1] + (R ^ R<<8)) ^ Rand[2];          // (A2 + A3(C3')) ^ B2
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  RanInt
//
// Purpose:   Random integer sample, from Uniform[0,2^32).
//
// Method:    Top 32 bits of RanLong.
//
// Note:      Result is unsigned --- legacy Ranint was signed.
//
// History:   John Skilling  17 Feb 2012
//-----------------------------------------------------------------------------
unsigned RanInt(         //   O  Random sample from Uniform[0, 2^32)
Rand64_t   Rand)         // I O  Random generator state
{
    return (unsigned)(RanLong(Rand) >> 32);
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  RanFloat
//
// Purpose:   Random single-precision floating-point sample from Uniform(0,1).
//            The 2^23 allowed values are odd multiples of 2^-24,
//            symmetrically placed in strict interior.
//
// Notes:     Assumes IEEE standard 23-bit mantissa.
//
// History:   John Skilling   17 Feb 2012
//            Richard Denny    4 May 2012  bugfix
//-----------------------------------------------------------------------------
float RanFloat(          //   O  Random odd multiple of 2^-24 in (0,1)
Rand64_t   Rand)         // I O  Random generator state
{
    static const float scale = 1.0 / (float)0x1000000;
    unsigned  u = (0xfffffe & RanInt(Rand)) | 1;
    return (float)u * scale;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  RanDouble
//
// Purpose:   Random double-precision floating-point sample from Uniform(0,1).
//            The 2^52 allowed values are odd multiples of 2^-53,
//            symmetrically placed in strict interior.
//
// Notes:     Assumes IEEE standard 52-bit mantissa.
//
// History:   John Skilling   17 Feb 2012
//            Richard Denny    4 May 2012  bugfix
//-----------------------------------------------------------------------------
double RanDouble(      //   O  Random odd multiple of 2^-53 in (0,1)
Rand64_t Rand)         // I O  Random generator state
{
    static const double scale = 1.0 / (double)0x20000000000000;
    ulong  u = (RanLong(Rand) >> 11) | 1;
    slong  v = (slong)u;
    return (double)v * scale;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  RanSigned
//
// Purpose:   Random double-precision floating-point sample from Uniform(-1,1).
//            The 2^53 allowed values are odd multiples of 2^-53, of
//            either sign, symmetrically placed in strict interior.
//
// Notes:     Assumes IEEE standard 52-bit mantissa.
//
// History:   John Skilling   17 Feb 2012
//-----------------------------------------------------------------------------
double RanSigned(      //   O  Random odd multiple of 2^-53 in (-1,1)
Rand64_t Rand)         // I O  Random generator state
{
    static const double scale = 1.0 / (double)0x20000000000000;
    slong  u = ((slong)RanLong(Rand) >> 10) | 1;
    return (double)u * scale;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  RanGrid
//
// Purpose:   Random 32-bit integer from [0,bound).
//
// History:   John Skilling   17 Feb 2012
//                             9 Mar 2012  bound=0 returns 0 (change of spec.)
//-----------------------------------------------------------------------------
unsigned RanGrid(         //   O  Random sample from Uniform[0, bound)
Rand64_t   Rand,          // I O  Random generator state
unsigned   bound)         // I    Supremum
{
    unsigned   i, M, N;
    if( bound == 0 )
        return 0;
    N = (unsigned)(-1) / bound;
    M = N * bound;
    do  i = RanInt(Rand);
    while( i >= M );
    return i / N;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  RanExp
// 
// Purpose:   Sample from
//                prob(x)  =  exp(-x)
//
// History:   John Skilling  29 Feb 2012
//-----------------------------------------------------------------------------
double RanExp(           //   O  Random sample from exponential distribution
Rand64_t   Rand)         // I O  Random generator state
{
    return  -log(RanDouble(Rand));
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  RanCauchy
// 
// Purpose:   Sample from
//                prob(x)  =  (1/pi) / (1 + x^2)
//
// Method:    Ratio-of-uniforms.
//                x = v/u  where u and v are uniform over unit disc.
// 
// Note:      Range is finite:  |x| < 2^63
// 
// History:   John Skilling  17 Feb 2012
//-----------------------------------------------------------------------------
double RanCauchy(        //   O  Random sample from Cauchy distribution
Rand64_t   Rand)         // I O  Random generator state
{
    double u;  // Uniform(-1,1), not 0. (*2^63)
    double v;  // Uniform[-1,1)         (*2^63)
    do
    {
        u = (double)((slong)(RanLong(Rand) | 1));
        v = (double)((slong)RanLong(Rand));
    } while( u * u + v * v >= 8.5070591730234616e37 );
    return  v / u;            //    (2^63)^2     //
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  RanGauss
// 
// Purpose:   Sample from Gaussian N(0,1)
//
// Method:    Ratio-of-uniforms.
//
// Note:      Range is finite: |x| < 13.153 standard deviations, x=0 excluded.
// 
// History:   John Skilling  17 Feb 2012
//            JS             19 Dec 2018 x=0 excluded
//-----------------------------------------------------------------------------
double RanGauss(         //   O  Random sample from normal distribution
Rand64_t   Rand)         // I O  Random generator state
{
               // do{ u = Uniform(-0.8578,0.8578) avoids certain rejection
    double v;  //     v = Uniform(0,1), not 0
    double x;  //     normal sample u/v (when accepted)
               //   } until v is accepted
    do
    {
        v = 10.8420217248550443401e-20*(double)((slong)((RanLong(Rand)>>1)|1));
        x = 9.30028623558065703491e-20*(double)((slong)(RanLong(Rand)|1)) / v;
    } while( log(v) >= - 0.25 * x * x );
    return  x;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  RanGamma
// 
// Purpose:   Draw sample from
//                             -1+c  -x
//                prob(x)  =  x     e  / Gamma(c)
// 
// Note:      x can (arithmetically correctly) underflow to 0 if c is small.
//            Use RanGamLogs directly to circumvent this.
//
// Method:    Marsaglia & Tang (see Numerical Recipes, 3rd ed, sec. 7.3).
// 
// History:   John Skilling  17 Feb 2012
//            JS             15 Sep 2014  Code transferred to RanGamLogs
//-----------------------------------------------------------------------------
double RanGamma(         //   O  Random sample from gamma distribution
Rand64_t   Rand,         // I O  Random generator state
double     c)            // I    Exponent > 0.0
{
    double y;
    return exp(RanGamLogs(Rand, 1, &y, &c));
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  RanBeta
// 
// Purpose:   Draw sample from
//                             Gamma(a+b)      a-1     b-1
//                prob(x)  = ---------------  x   (1-x)  
//                           Gamma(a)Gamma(b) 
// 
// Note:      x can (arithmetically correctly) underflow to 0 if a or b small.
//            Use RanGamLogs to circumvent this.
// 
// History:   John Skilling  17 Feb 2012
//            JS             15 Sep 2014  Code transferred to RanGamLogs
//-----------------------------------------------------------------------------
double RanBeta(          //   O  Random sample from beta distribution
Rand64_t   Rand,         // I O  Random generator state
double     a,            // I    Exponent >= 0.0
double     b)            // I    Exponent >= 0.0
{
    double y[2], c[2], logZ;
    c[0] = a;
    c[1] = b;
    logZ = RanGamLogs(Rand, 2, y, c);
    return  exp(y[0] - logZ);
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  RanGamLogs      (for Gamma, Beta, Dirichlet)
// 
// Purpose:   Draw n samples x[0]...x[n-1] from
//                                 -1+c[k]   -x[k]
//                prob(x[k]) =  x[k]        e      / Gamma(c[k])
//
//            Samples are returned as logs to avoid underflow to 0 if c small.
//            Return value of function is
//                logZ = log(SUM x[k]).
//
// Usage:     ..,exp(logx[k] - logZ),.. is sample from Dirichlet(..,c[k],..),
//                               available as logarithms to avoid underflow.
//            For n=2, this duplicates RanBeta(c[0],c[1]) without underflow.
//            For n=1, logZ duplicates RanGamma(c[0])     without underflow.
//
// Method:    Marsaglia & Tang (see Numerical Recipes, 3rd ed, sec. 7.3).
// 
// History:   John Skilling  17 Feb 2012  RanGamma and RanBeta
//            JS             15 Sep 2014  Dirichlet generalisation
//            JS             16 Sep 2019  c[k] = 0 allowed
//-----------------------------------------------------------------------------
double RanGamLogs(       //   O  log(SUM samples)
Rand64_t   Rand,         // I O  Random generator state
int        n,            // I    dimension
double*    logx,         //   O  log(samples) from gammas
double*    c)            // I    exponents > 0.0
{
    double a, b, r, u = 0.0, top = -DBL_MAX;
    int    k;
    for( k = 0; k < n; k++ )
    {
// Sample non-singular distribution with exponent >= 1.
        a = c[k] - 1.0/3.0;
        if( c[k] < 1.0 )
            a += 1.0;
        b = 1.0 / sqrt(9.0 * a);
        do
        {
            do
            {
                r = RanGauss(Rand);
                u = 1.0 + b * r;
            } while( u <= 0.0 );
            u *= u * u;
        } while( log(RanDouble(Rand)) >= 0.5 * r * r + a * (1. - u + log(u)) );
        u = log(a * u);
// Tidy up if exponent was < 1.
        if( c[k] < 1.0 )
            u = (c[k] > 0.0) ? u + log(RanDouble(Rand)) / c[k] : -DBL_MAX;
        logx[k] = u;
        if( top < u )
            top = u;
    }
    r = 0.0;
    for( k = 0; k < n; k++ )
        r += exp(logx[k] - top);
    return log(r) + top;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  RanPoiss
// 
// Purpose:   Draw sample from Poisson
//                           -c  k
//                prob(k) = e   c / k! 
//
// Method:    Numerical Recipes, 3rd ed, sec. 7.3.
// 
// History:   John Skilling  17 Feb 2012
//-----------------------------------------------------------------------------
int   RanPoiss(        //   O  Random sample from Poisson distribution
Rand64_t   Rand,       // I O  Random generator state
double     c)          // I    Mean > 0
{
    if( c < 6.0 )
    {
// Product of uniform deviates
        double y;      // termination
        double t;      // accumulated product
        int    k;      // counter
        y = exp(-c);
        t = 1.0;
        for( k = -1; t >= y; k++ )
            t *= RanDouble(Rand);
        return k;
    }
    else if( c < 67108864 )  // 2^26
    {
// Ratio of normals
        double centre; // mean + 0.5
        double width;  // standard deviation
        double b;      // width exp(-c)    (as log)
        double u;      // do{    Uniform(-.6257,.6257)     x = u/v 
        double v;      //        Uniform(0,.6337)          v^2 <= prob(x)
        int    k;      //        centre + width * x, as integer >= 0
                       //   } until k is in range and v^2 is accepted
        centre = c + 0.27;
        width = sqrt(c);
        b = log(width) - c;
        do
        {
            u = 6.80e-20 * (double)((slong)RanLong(Rand));            // signed
            v = 6.88e-20 * (double)((slong)((RanLong(Rand)>>1) | 1)); // +ve
            k = (int)(centre + width * u / v + 1.0) - 1;   // keep k(-ve) < 0
        } while( k < 0 || 2.0 * log(v) >= b + k * log(c) - logFactorial(k) );
        return k;
    }
    else
// Normal limit
        return (int)(c + sqrt(c) * RanGauss(Rand));
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  RanBinom
// 
// Purpose:   Draw sample from binomial
//
//                             n!     k      n-k
//                prob(k) = -------- p  (1-p)              0 <= k <= n
//                          k!(n-k)!
// 
// History:   John Skilling  17 Feb 2012
//-----------------------------------------------------------------------------
int  RanBinom(         //   O  Random sample from binomial distribution
Rand64_t   Rand,       // I O  Random generator state
int        n,          // I    Range
double     p)          // I    Mean/Range
{
    double q;          // complement of p
    double width;      // standard deviation
    if( p <= 0.0 )
        return 0;      // forced leftward result
    if( p >= 1.0 )
        return n;      // forced rightward result
    q = 1.0 - p;
    width = sqrt(n * p * q);
    if( width <= 2.0 )
// Tabulate the cumulant distribution function and slice randomly into it.
    {
        double r;      // odds, diminishing rightward (or leftward)
        double a;      // term k is n-k  (in floating point)
        double b;      // term k is k    (in floating point)
        double t;      // term k is r^k n!/(n-k)!
        double s;      // term k is cumulant of terms up to k
        int    k;      // counter of terms
        double cdf[32];// store cumulants s (32 always adequate here)
        int    j;      // lower bound in binary chop
        int    i;      // index in binary chop j <= i < k = result
        r = (p < q) ? p / q : q / p;
        a = n;
        b = 0;
        t = 1.0;
        s = 0.0;
        for( k = 0; t > s * DBL_EPSILON; k++ )
        {
            cdf[k] = s += t;
            t *= r * a-- / ++b;
        }                       // cumulant is almost or exactly q^r (or p^r)
        s *= RanDouble(Rand);   // safely guaranteed to be < computed cumulant
        j = -1;
        do                      // binary chop
        {
            i = (j + k) / 2;
            if( s <= cdf[i] )  k = i;
            else               j = i;
        } while( k - j > 1 );
        return (p < q) ? k : n - k;
    }
    else if( n < 134217728 )    // 2^26 * 2, beyond which subtraction 
    {                           // of logs would lose too much precision
// Ratio of normals
        double centre; // mean + 0.5
        double b;      // width n!    (as log)
        double u;      // do{    Uniform(-.693,.693)       x = u/v 
        double v;      //        Uniform(0,.634)           v^2 <= prob(x)
        int    k;      //        centre + width * x, as 0 <= integer <= n
                       //   } until k is in range and v^2 is accepted
        centre = n * p + 0.5;
        b = log(width) + logFactorial(n);
        do
        {
            u = 7.51e-20 * (double)((slong)RanLong(Rand));            // signed
            v = 6.88e-20 * (double)((slong)((RanLong(Rand)>>1) | 1)); // +ve
            k = (int)(centre + width * u / v + 1.0) - 1;    // keep k(-ve) < 0
        } while( k < 0 || k > n
                 || 2.0 * log(v) >= b + k * log(p) + (n - k) * log(q)
                                   - logFactorial(k) - logFactorial(n-k) );
        return k;
    }
// Asymptotic forms
    else if( n * p < 33554432 )   // 2^26 / 2
        return RanPoiss(Rand, n * p);
    else if( n * q < 33554432 )   // 2^26 / 2
        return n - RanPoiss(Rand, n * q);
    else
        return (int)(0.5 + n * p + width * RanGauss(Rand));
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  RanPerm
//
// Purpose:   Random permutation of {0,1,2,...,n-1}.
//
// History:   John Skilling  17 Feb 2012
//-----------------------------------------------------------------------------
void RanPerm( 
Rand64_t Rand,        // I O  Random generator state
int      n,           // I    Dimension
int*     perm)        //   O  Output permutation
{
    int    i, m;
    for( m = 0; m < n; ++m )
    {
        i = RanGrid(Rand, m + 1);
        if( i < m )
            perm[m] = perm[i];
        perm[i] = m;
    }
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  RanSphere
// 
// Purpose:   Random direction
//
// History:   John Skilling  17 Feb 2012
//-----------------------------------------------------------------------------
void RanSphere(
Rand64_t Rand,   // I O  random generator
int      n,      // I    dimension
double*  v)      //   O  random vector, unit length v'.v = 1     [n]
{
    double t = 0.0;
    int    i;
    for( i = 0; i < n; i++ )
    {
        v[i] = RanGauss(Rand);
        t += v[i] * v[i];      // cannot be 0
    }
    t = 1.0 / sqrt(t);
    for( i = 0; i < n; i++ )
        v[i] *= t;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  RanSimplex
//
// Purpose:   Fill vector with uniformly random "colour" over simplex SUM=s.
//
// History:   John Skilling  17 Feb 2012
//-----------------------------------------------------------------------------
void RanSimplex(
Rand64_t Rand,        // I O  random generator
double   s,           // I    SUM(v)
int      n,           // I    dimension
double*  v)           //   O  vector being coloured      [n]
{
    for( --n; n; n-- )
        s -= v[n] = s * (1.0 - pow(RanDouble(Rand), 1.0 / n));
    v[0] = s;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  PrCauchy2
// 
// Purpose:   Joint probability Prob(x,v) of unit-Cauchy variables
//                             2   n                            1   n
//               location x = --- SUM k C[k]  ,   velocity v = --- SUM C[k]
//                            n^2 k=1                           n  k=1
//            (n --> infinity) where C[k] is the k'th draw from unit Cauchy.
//
// Method:    Formula is
//                           1   (    4            1      v-x+s )
//               Pr(x,v) = ----- ( ------- + Imag --- log ----- )
//                         2PI^2 ( |s^2|^2        s^3     v-x-s )
//            where
//                     s = sqrt(1 + 2i(v-x) + v^2)
//
// Note:      Formula is derived from characteristic function (= Fourier(Prob))
//
//                                   { - (1 - w/2) |q|              if w <= 1
//               <exp(ipx+iqv)> = exp{
//                                   { - (1/w + w/2 - 1) |q|        if w >= 1
//            where w = -2p/q.
//
// Result (scaled):         v
//                           | 0 0 0 0 0 0 0 0 1 1 1 1 1
//                           | 0 0 0 0 0 0 0 1 1 1 2 2 2
//                         1 | 0 0 0 0 0 0 1 1 2 2 3 2 2
//                           | 0 0 0 0 0 1 1 2 4 4 4 2 1
//                           | 0 0 0 0 1 1 3 5 6 5 3 1 1
//                           | 0 0 0 1 1 3 6 8 6 3 1 1 0
//                         0 | 0 0 1 1 3 6 9 6 3 1 1 0 0
//                           | 0 1 1 3 6 8 6 3 1 1 0 0 0
//                           | 1 1 3 5 6 5 3 1 1 0 0 0 0
//                           | 1 2 4 4 4 2 1 1 0 0 0 0 0
//                        -1 | 2 2 3 2 2 1 1 0 0 0 0 0 0
//                           | 2 2 2 1 1 1 0 0 0 0 0 0 0
//                           | 1 1 1 1 1 0 0 0 0 0 0 0 0
//                            --------------------------- x
//                                -1       0       1
// 
// History:   John Skilling   4 Feb 2016
//-----------------------------------------------------------------------------
double PrCauchy2(   //   O  Joint probability
double  x,          // I    Location unit-Cauchy variable (from linear weights)
double  v)          // I    Velocity unit-Cauchy variable (from equal  weights)
{
    double  c1, s1, z1;   // Re
    double  c2, s2, z2;   // Im
    double  t, dd, ss, zz;
    double  d = v - x;    // Pre-compute to force correct signs in complex LOG
    dd = d * d;

// z = 1 + 2id + v*v,    s = SQRT(z)
    z1 = 1.0 + v * v;                   z2 = 2.0 * d;   zz = z1 * z1 + z2 * z2;
    s1 = sqrt(0.5 * (sqrt(zz) + z1));   s2 = d / s1;    ss = s1 * s1 + s2 * s2;

// c = LOG((v-x+s)/(v-x-s)) with 0 < Imag(log) <= PI forced by construction
    t = z2 * s1 / (dd + ss);
    c1 = 0.5 * log((1.0 + t) / (1.0 - t));        c2 = atan2(z2 * s2, dd - ss);

// c = 4/|z|^2 + (c/s)/z
    c2 = (4 + ((c2 * s1 - c1 * s2) * z1 - (c1 * s1 + c2 * s2) * z2) / ss) / zz;
// Imag(c) / 2*PI*PI
    return (c2 > 0.0) ? c2 / 19.7392088021787172 : 0.0;   // (paranoia)
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  RanCauchyXfromV
// 
// Purpose:   Sample from conditional probability Pr(x|v) = Pr(x,v)/Pr(v).
//
// Method:    Rejection of samples from
//                                    h/q^4   |    h   |    h/q^4
//                  upper bound =   ----------|--------|-----------
//                                           q=-1     q=1
//            where q = (x-v)/c with experimental width w
//            and   h = Pr(v,v) analytic.
// 
// History:   John Skilling   4 Feb 2016
//-----------------------------------------------------------------------------
double RanCauchyXfromV(     //   O  Conditional Cauchy sample of location
Rand64_t   Rand,            // I O  Random generator state
double     v)               // I    Velocity
{
    double PI = 3.14159265358979324;
    double a, b, h, q, w, x, upper, value;

    h = 1 + v*v;
    h = (2.0 / h + 0.5*PI / sqrt(h)) / (PI*PI*h);   // height of upper bound
    w = sqrt(0.335 + v*v);                          // width of upper bound
    do
    {
        a = RanSigned(Rand);     // Uniform(-1,1)
        b = RanDouble(Rand);     // Uniform(0,1)
        q = a / pow(b, 1./3.);   // sample from {... 1/q^4 ... 1 ... 1/q^4 ...}
        upper = (fabs(q) < 1) ? h : h / (q*q*q*q);  // upper bound
        x = v + w * q;                              // trial location
        value = PrCauchy2(x, v);                    // required value
    } while( value < upper * RanDouble(Rand) );     // rejection
    return x;                                       // accepted location
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  RanCauchyVfromX
// 
// Purpose:   Sample from conditional probability Pr(v|x) = Pr(x,v)/Pr(x).
//
// Method:    Rejection of samples from (for positive x, else reverse signs)
//            IF( x < 1 )
//                                    h/q^3   |    h   |    h/q^3
//                  upper bound =   ----------|--------|-----------
//                                           q=-1     q=1
//                  with experimental parameters h,c,w, where q = (v-c)/w;
//            ELSE (x > 1)
//                                       h/q^3     |     2h/q^3
//                  upper bound =   ---------------|---------------
//                                   v -ve       v=x/2       v +ve
//                  with experimental height h, where q = 1 + |2v/x - 1|.
// 
// History:   John Skilling   4 Feb 2016
//-----------------------------------------------------------------------------
double RanCauchyVfromX(     //   O  Conditional Cauchy sample of velocity
Rand64_t   Rand,            // I O  Random generator state
double     x)               // I    Location
{
    double a, b, c, h, q, v, w, upper, value;

    if( fabs(x) <= 1 )                    // extract small x
    {
        h = 0.377 / sqrt(1.0 + 4.0*x*x);  // experimental height of upper bound
        c = 1.72 * x;                     // experimental centre of upper bound
        w = 0.65 * sqrt(1.0 + 1.85*x*x);  // experimental width  of upper bound
        do
        {
            a = RanSigned(Rand);          // Uniform(-1,1)
            b = RanDouble(Rand);          // Uniform(0,1)
            q = a / sqrt(b);     // sample from {... 1/q^3 ... 1 ... 1/q^3 ...}
            upper = (fabs(q) < 1) ? h : h / fabs(q*q*q);   // upper bound
            v = c + w * q;                                 // trial velocity
            value = PrCauchy2(x, v);                       // required value
        } while( value < upper * RanDouble(Rand) );        // rejection
    }
    else                                  // remaining larger x
    {
        h = 0.8 / fabs(x*x*x);            // experimental height of upper bound
        do
        {
            q = 1.0 / sqrt(RanDouble(Rand));        // sample from 1/q^3 in q>1
            if( RanGrid(Rand, 3) > 0 )              // weight doubled on right
            {                                       // v > x/2 on right
                v = q * x/2;                        // trial velocity
                upper = 2.0 * h / (q*q*q);          // upper bound
            }
            else
            {                                       // v < x/2 on left
                v = x - q * x/2;                    // trial velocity
                upper = h / (q*q*q);                // upper bound
            }
            value = PrCauchy2(x, v);                // required value
        } while( value < upper * RanDouble(Rand) ); // rejection
    }
    return v;                                       // accepted velocity
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  logGamma
// 
// Purpose:   log( Gamma(x) )
//
// Method:    Stirling formula.
//
// History:   John Skilling  17 Feb 2012
//-----------------------------------------------------------------------------
double logGamma(            //   O  log of Gamma function
double x)                   // I    Argument
{
    double s, t;
// Ensure x > 16, otherwise obtain correction.
    if( x < 16.0 )
    {
        for( t = 1.0; x < 16.0; t *= x++ ) ;
        s = -log(t);
    }
    else
        s = 0.0;
// Asymptotic series
    t = 1.0 / (x * x);
    return s + 0.918938533204672742 - x + (x - 0.5) * log(x)
             + (420.0 - (14.0 - (4.0 - 3.0 * t) * t) * t) / (5040.0 * x);
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  logFactorial
// 
// Purpose:   log( k ! )
//
// Method:    Lookup table up to 1023, then asymptotic logGamma.
//
// History:   John Skilling  17 Feb 2012
//-----------------------------------------------------------------------------
double logFactorial(     //   O  Value = log(k!)
unsigned  k)             // I    Argument
{
/*         // CODE TO GENERATE LOOKUP TABLE //
    int i, j, n;
    table[0] = 0.0;
    for( i = 1; i < 1024; i++ )
        table[i] = log(i);
    for( n = 1; n < 1024; n <<= 1 )  // Binary add preserves full precision
        for( i = n; i < 1024; i += n+n )
            for( j = 0; j < n; j++ )
                table[i+j] += table[i-1];
    printf("    static const double table[1024] = {0.0, 0.0,\n");
    for( i = 2; i < 3; i++ )  printf("%19.17f,", table[i]);
    for( ; i < 8; i++ )       printf("%18.16f,", table[i]);
    for( ; i < 38; i++ )      printf("%18.15f,", table[i]);
    for( ; i < 226; i++ )     printf("%18.14f,", table[i]);
    for( ; i < 1024; i++ )    printf("%18.13f,", table[i]);
    printf("};\n");
*/
    static const double table[1024] = {0.0, 0.0,
.69314718055994531,1.7917594692280550,3.1780538303479453,4.7874917427820458,
6.5792512120101012,8.5251613610654129,10.604602902745249,12.801827480081467,
15.104412573075514,17.502307845873887,19.987214495661885,22.552163853123425,
25.191221182738680,27.899271383840890,30.671860106080672,33.505073450136891,
36.395445208033053,39.339884187199488,42.335616460753485,45.380138898476901,
48.471181351835220,51.606675567764370,54.784729398112319,58.003605222980518,
61.261701761001994,64.557538627006323,67.889743137181540,71.257038967168000,
74.658236348830158,78.092223553315307,81.557959456115029,85.054467017581516,
88.580827542197682,92.136175603687093,95.719694542143202,99.330612454787428,
102.96819861451381,106.63176026064346,110.32063971475739,114.03421178146169,
117.77188139974507,121.53308151543862,125.31727114935688,129.12393363912722,
132.95257503561629,136.80272263732638,140.67392364823425,144.56574394634487,
148.47776695177305,152.40959258449737,156.36083630307877,160.33112821663090,
164.32011226319520,168.32744544842765,172.35279713916282,176.39584840699735,
180.45629141754375,184.53382886144948,188.62817342367160,192.73904728784490,
196.86618167288998,201.00931639928154,205.16819948264123,209.34258675253685,
213.53224149456327,217.73693411395425,221.95644181913036,226.19054832372763,
230.43904356577696,234.70172344281829,238.97838956183435,243.26884900298273,
247.57291409618691,251.89040220972322,256.22113555000954,260.56494097186322,
264.92164979855283,269.29109765101987,273.67312428569369,278.06757344036617,
282.47429268763040,286.89313329542699,291.32395009427034,295.76660135076065,
300.22094864701415,304.68685676566872,309.16419358014696,313.65282994987911,
318.15263962020936,322.66349912672621,327.18528770377520,331.71788719692847,
336.26118197919851,340.81505887079902,345.37940706226686,349.95411804077025,
354.53908551944085,359.13420536957540,363.73937555556347,368.35449607240474,
372.97946888568902,377.61419787391867,382.25858877306001,386.91254912321756,
391.57598821732961,396.24881705179155,400.93094827891576,405.62229616114490,
410.32277652693733,415.03230672824964,419.75080559954472,424.47819341825709,
429.21439186665157,433.95932399501481,438.71291418612117,443.47508812091894,
448.24577274538461,453.02489623849613,457.81238798127816,462.60817852687489,
467.41219957160814,472.22438392698058,477.04466549258564,481.87297922988790,
486.70926113683942,491.55344822329801,496.40547848721764,501.26529089157930,
506.13282534203489,511.00802266523601,515.89082458782241,520.78117371604412,
525.67901351599505,530.58428829443346,535.49694318016952,540.41692410599762,
545.34417779115483,550.27865172428551,555.22029414689484,560.16905403727310,
565.12488109487435,570.08772572513419,575.05753902471020,580.03427276713080,
585.01787938883911,590.00831197561786,595.00552424938201,600.00947055532743,
605.02010584942366,610.03738568623862,615.06126620708483,620.09170412847732,
625.12865673089095,630.17208184781020,635.22193785505976,640.27818366040810,
645.34077869343503,650.40968289565524,655.48485671088906,660.56626107587351,
665.65385741110595,670.74760761191271,675.84747403973688,680.95341951363741,
686.06540730199401,691.18340111441080,696.30736509381404,701.43726380873704,
706.57306224578736,711.71472580228999,716.86222027910344,722.01551187360121,
727.17456717281584,732.33935314673931,737.50983714177744,742.68598687435133,
747.86777042464337,753.05515623048404,758.24811308137430,763.44661011264020,
768.65061679971700,773.86010295255835,779.07503871016729,784.29539453524569,
789.52114120895885,794.75224982581346,799.98869178864334,805.23043880370301,
810.47746287586347,815.72973630391016,820.98723167593789,826.24992186484280,
831.51778002390620,836.79077958246990,842.06889424170049,847.35209797043842,
852.64036500113298,857.93366982585746,863.23198719240554,868.53529210046452,
873.84355979786574,879.15676577690760,884.47488577075183,889.79789574989013,
895.12577191867967,900.45849071194516,905.79602879164645,911.13836304361121,
916.48547057432870,921.83732870780477,927.19391498247683,932.55520714818624,
937.92118316320807,943.29182119133566,948.66709959901982,954.04699695256033,
959.43149201534948,964.82056374516594,970.21419129151832,975.61235399303609,
981.01503137490840,986.42220314636847,991.83384919822356,997.24994960042795,
1002.6704845997002,1008.0954346171817,1013.5247802461361,1018.9585022496902,
1024.3965815586134,1029.8389992691355,1035.2857366408016,1040.7367750943672,
1046.1920962097252,1051.6516817238692,1057.1155135288948,1062.5835736700301,
1068.0558443437012,1073.5323078956330,1079.0129468189748,1084.4977437524656,
1089.9866814786224,1095.4797429219630,1100.9769111472560,1106.4781693578007,
1111.9835008937330,1117.4928892303610,1123.0063179765261,1128.5237708729906,
1134.0452317908530,1139.5706847299848,1145.1001138174961,1150.6335033062237,
1156.1708375732424,1161.7121011184008,1167.2572785628804,1172.8063546477756,
1178.3593142326972,1183.9161422943966,1189.4768239254122,1195.0413443327350,
1200.6096888364962,1206.1818428686738,1211.7577919718201,1217.3375217978064,
1222.9210181065880,1228.5082667649883,1234.0992537454993,1239.6939651251009,
1245.2923870840993,1250.8945059049790,1256.5003079712751,1262.1097797664600,
1267.7229078728481,1273.3396789705148,1278.9600798362319,1284.5840973424192,
1290.2117184561098,1295.8429302379311,1301.4777198411005,1307.1160745104341,
1312.7579815813724,1318.4034284790155,1324.0524027171768,1329.7048918974453,
1335.3608837082652,1341.0203659240249,1346.6833264041609,1352.3497530922732,
1358.0196340152538,1363.6929572824251,1369.3697110846936,1375.0498836937106,
1380.7334634610493,1386.4204388173891,1392.1107982717131,1397.8045304105158,
1403.5016238970213,1409.2020674704120,1414.9058499450682,1420.6129602098172,
1426.3233872271919,1432.0371200327013,1437.7541477341074,1443.4744595107150,
1449.1980446126672,1454.9248923602545,1460.6549921432281,1466.3883334201259,
1472.1249057176051,1477.8646986297842,1483.6077018175938,1489.3539050081340,
1495.1032979940421,1500.8558706328677,1506.6116128464546,1512.3705146203320,
1518.1325660031121,1523.8977571058970,1529.6660781016908,1535.4375192248208,
1541.2120707703652,1546.9897230935878,1552.7704666093800,1558.5542917917098,
1564.3411891730766,1570.1311493439739,1575.9241629523581,1581.7202207031232,
1587.5193133575840,1593.3214317329609,1599.1265667018774,1604.9347091918580,
1610.7458501848346,1616.5599807166595,1622.3770918766229,1628.1971748069752,
1634.0202207024581,1639.8462208098385,1645.6751664274489,1651.5070489047325,
1657.3418596417951,1663.1795900889610,1669.0202317463343,1674.8637761633659,
1680.7102149384234,1686.5595397183702,1692.4117421981448,1698.2668141203471,
1704.1247472748307,1709.9855334982965,1715.8491646738946,1721.7156327308280,
1727.5849296439617,1733.4570474334371,1739.3319781642890,1745.2097139460689,
1751.0902469324694,1756.9735693209577,1762.8596733524080,1768.7485513107408,
1774.6401955225665,1780.5345983568313,1786.4317522244683,1792.3316495780507,
1798.2342829114521,1804.1396447595066,1810.0477276976756,1815.9585243417159,
1821.8720273473543,1827.7882294099618,1833.7071232642350,1839.6287016838787,
1845.5529574812931,1851.4798835072636,1857.4094726506537,1863.3417178381014,
1869.2766120337210,1875.2141482388035,1881.1543194915239,1887.0971188666508,
1893.0425394752574,1898.9905744644379,1904.9412170170258,1910.8944603513135,
1916.8502977207781,1922.8087224138080,1928.7697277534312,1934.7333070970499,
1940.6994538361735,1946.6681613961589,1952.6394232359494,1958.6132328478186,
1964.5895837571165,1970.5684695220175,1976.5498837332721,1982.5338200139593,
1988.5202720192437,1994.5092334361336,2000.5006979832415,2006.4946594105481,
2012.4911114991671,2018.4900480611138,2024.4914629390751,2030.4953500061815,
2036.5017031657833,2042.5105163512258,2048.5217835256299,2054.5354986816728,
2060.5516558413710,2066.5702490558674,2072.5912724052168,2078.6147199981779,
2084.6405859720035,2090.6688644922342,2096.6995497524954,2102.7326359742938,
2108.7681174068189,2114.8059883267410,2120.8462430380182,2126.8888758717007,
2132.9338811857369,2138.9812533647828,2145.0309868200147,2151.0830759889395,
2157.1375153352087,2163.1942993484372,2169.2534225440195,2175.3148794629469,
2181.3786646716349,2187.4447727617385,2193.5131983499828,2199.5839360779851,
2205.6569806120856,2211.7323266431740,2217.8099688865232,2223.8899020816189,
2229.9721209919953,2236.0566204050706,2242.1433951319827,2248.2324400074294,
2254.3237498895073,2260.4173196595525,2266.5131442219845,2272.6112185041511,
2278.7115374561708,2284.8140960507844,2290.9188892831994,2297.0259121709419,
2303.1351597537059,2309.2466270932091,2315.3603092730409,2321.4762013985242,
2327.5942985965653,2333.7145960155167,2339.8370888250311,2345.9617722159251,
2352.0886414000393,2358.2176916100998,2364.3489180995830,2370.4823161425793,
2376.6178810336614,2382.7556080877475,2388.8954926399738,2395.0375300455612,
2401.1817156796869,2407.3280449373556,2413.4765132332732,2419.6271160017195,
2425.7798486964239,2431.9347067904400,2438.0916857760258,2444.2507811645173,
2450.4119884862125,2456.5753032902476,2462.7407211444788,2468.9082376353672,
2475.0778483678587,2481.2495489652692,2487.4233350691711,2493.5992023392773,
2499.7771464533280,2505.9571631069803,2512.1392480136969,2518.3233969046341,
2524.5096055285348,2530.6978696516176,2536.8881850574708,2543.0805475469456,
2549.2749529380503,2555.4713970658449,2561.6698757823369,2567.8703849563799,
2574.0729204735676,2580.2774782361362,2586.4840541628610,2592.6926441889577,
2598.9032442659827,2605.1158503617339,2611.3304584601565,2617.5470645612413,
2623.7656646809328,2629.9862548510328,2636.2088311191037,2642.4333895483792,
2648.6599262176669,2654.8884372212578,2661.1189186688362,2667.3513666853869,
2673.5857774111055,2679.8221470013091,2686.0604716263488,2692.3007474715196,
2698.5429707369744,2704.7871376376384,2711.0332444031196,2717.2812872776281,
2723.5312625198876,2729.7831664030537,2736.0369952146293,2742.2927452563827,
2748.5504128442649,2754.8099943083298,2761.0714859926511,2767.3348842552427,
2773.6001854679803,2779.8673860165218,2786.1364823002282,2792.4074707320865,
2798.6803477386325,2804.9551097598746,2811.2317532492161,2817.5102746733819,
2823.7906705123423,2830.0729372592382,2836.3570714203088,2842.6430695148179,
2848.9309280749794,2855.2206436458887,2861.5122127854470,2867.8056320642931,
2874.1008980657330,2880.3980073856669,2886.6969566325229,2892.9977424271860,
2899.3003614029312,2905.6048102053528,2911.9110854923010,2918.2191839338107,
2924.5291022120373,2930.8408370211901,2937.1543850674670,2943.4697430689894,
2949.7869077557366,2956.1058758694830,2962.4266441637337,2968.7492094036611,
2975.0735683660423,2981.3997178391974,2987.7276546229268,2994.0573755284495,
3000.3888773783428,3006.7221570064826,3013.0572112579807,3019.3940369891270,
3025.7326310673302,3032.0729903710580,3038.4151117897791,3044.7589922239058,
3051.1046285847342,3057.4520177943905,3063.8011567857702,3070.1520425024846,
3076.5046718988042,3082.8590419396014,3089.2151496002975,3095.5729918668058,
3101.9325657354780,3108.2938682130512,3114.6568963165914,3121.0216470734435,
3127.3881175211750,3133.7563047075255,3140.1262056903533,3146.4978175375854,
3152.8711373271626,3159.2461621469906,3165.6228890948892,3172.0013152785405,
3178.3814378154402,3184.7632538328467,3191.1467604677305,3197.5319548667285,
3203.9188341860909,3210.3073955916366,3216.6976362587020,3223.0895533720945,
3229.4831441260449,3235.8784057241605,3242.2753353793769,3248.6739303139120,
3255.0741877592209,3261.4761049559479,3267.8796791538825,3274.2849076119137,
3280.6917875979830,3287.1003163890423,3293.5104912710085,3299.9223095387188,
3306.3357684958860,3312.7508654550575,3319.1675977375698,3325.5859626735059,
3332.0059576016529,3338.4275798694598,3344.8508268329933,3351.2756958568984,
3357.7021843143561,3364.1302895870408,3370.5600090650801,3376.9913401470135,
3383.4242802397525,3389.8588267585401,3396.2949771269095,3402.7327287766457,
3409.1720791477460,3415.6130256883789,3422.0555658548474,3428.4996971115474,
3434.9454169309329,3441.3927227934746,3447.8416121876212,3454.2920826097652,
3460.7441315642027,3467.1977565630950,3473.6529551264357,3480.1097247820076,
3486.5680630653524,3493.0279675197298,3499.4894356960835,3505.9524651530046,
3512.4170534566942,3518.8831981809317,3525.3508969070363,3531.8201472238320,
3538.2909467276145,3544.7632930221152,3551.2371837184678,3557.7126164351721,
3564.1895887980618,3570.6680984402701,3577.1481430021968,3583.6297201314733,
3590.1128274829307,3596.5974627185656,3603.0836235075099,3609.5713075259946,
3616.0605124573194,3622.5512359918221,3629.0434758268430,3635.5372296666947,
3642.0324952226315,3648.5292702128172,3655.0275523622936,3661.5273394029496,
3668.0286290734898,3674.5314191194057,3681.0357072929419,3687.5414913530703,
3694.0487690654554,3700.5575382024272,3707.0677965429504,3713.5795418725947,
3720.0927719835072,3726.6074846743795,3733.1236777504228,3739.6413490233354,
3746.1604963112754,3752.6811174388340,3759.2032102370044,3765.7267725431539,
3772.2518022009972,3778.7782970605681,3785.3062549781907,3791.8356738164530,
3798.3665514441786,3804.8988857364011,3811.4326745743347,3817.9679158453482,
3824.5046074429392,3831.0427472667070,3837.5823332223245,3844.1233632215144,
3850.6658351820215,3857.2097470275862,3863.7550966879207,3870.3018820986813,
3876.8501012014435,3883.3997519436771,3889.9508322787206,3896.5033401657556,
3903.0572735697810,3909.6126304615918,3916.1694088177501,3922.7276066205623,
3929.2872218580551,3935.8482525239519,3942.4106966176460,3948.9745521441778,
3955.5398171142133,3962.1064895440163,3968.6745674554286,3975.2440488758430,
3981.8149318381820,3988.3872143808762,3994.9608945478367,4001.5359703884365,
4008.1124399574846,4014.6903013152059,4021.2695525272156,4027.8501916645009,
4034.4322168033937,4041.0156260255526,4047.6004174179379,4054.1865890727927,
4060.7741390876176,4067.3630655651514,4073.9533666133480,4080.5450403453565,
4087.1380848794988,4093.7324983392487,4100.3282788532106,4106.9254245550965,
4113.5239335837114,4120.1238040829239,4126.7250342016523,4133.3276220938424,
4139.9315659184431,4146.5368638393911,4153.1435140255890,4159.7515146508849,
4166.3608638940532,4172.9715599387710,4179.5836009736031,4186.1969851919830,
4192.8117107921862,4199.4277759773195,4206.0451789552935,4212.6639179388112,
4219.2839911453411,4225.9053967971058,4232.5281331210554,4239.1521983488556,
4245.7775907168634,4252.4043084661125,4259.0323498422922,4265.6617130957293,
4272.2923964813717,4278.9243982587677,4285.5577166920484,4292.1923500499097,
4298.8282966055958,4305.4655546368813,4312.1041224260471,4318.7439982598735,
4325.3851804296137,4332.0276672309810,4338.6714569641290,4345.3165479336349,
4351.9629384484824,4358.6106268220456,4365.2596113720701,4371.9098904206585,
4378.5614622942476,4385.2143253236009,4391.8684778437846,4398.5239181941524,
4405.1806447183299,4411.8386557642007,4418.4979496838851,4425.1585248337242,
4431.8203795742702,4438.4835122702607,4445.1479212906106,4451.8136050083931,
4458.4805618008231,4465.1487900492402,4471.8182881390976,4478.4890544599439,
4485.1610874054049,4491.8343853731731,4498.5089467649868,4505.1847699866212,
4511.8618534478692,4518.5401955625230,4525.2197947483673,4531.9006494271580,
4538.5827580246078,4545.2661189703740,4551.9507306980413,4558.6365916451105,
4565.3237002529768,4572.0120549669236,4578.7016542361025,4585.3924965135211,
4592.0845802560270,4598.7779039242978,4605.4724659828189,4612.1682648998776,
4618.8652991475437,4625.5635672016588,4632.2630675418204,4638.9637986513681,
4645.6657590173709,4652.3689471306116,4659.0733614855762,4665.7790005804363,
4672.4858629170385,4679.1939470008920,4685.9032513411503,4692.6137744506032,
4699.3255148456592,4706.0384710463359,4712.7526415762459,4719.4680249625799,
4726.1846197361010,4732.9024244311240,4739.6214375855097,4746.3416577406451,
4753.0630834414360,4759.7857132362915,4766.5095456771123,4773.2345793192799,
4779.9608127216379,4786.6882444464891,4793.4168730595738,4800.1466971300633,
4806.8777152305447,4813.6099259370130,4820.3433278288503,4827.0779194888228,
4833.8136995030654,4840.5506664610675,4847.2888189556634,4854.0281555830206,
4860.7686749426266,4867.5103756372782,4874.2532562730703,4880.9973154593808,
4887.7425518088658,4894.4889639374396,4901.2365504642694,4907.9853100117598,
4914.7352412055488,4921.4863426744851,4928.2386130506275,4934.9920509692256,
4941.7466550687132,4948.5024239906979,4955.2593563799455,4962.0174508843720,
4968.7767061550367,4975.5371208461202,4982.2986936149246,4989.0614231218551,
4995.8253080304185,5002.5903470071989,5009.3565387218587,5016.1238818471247,
5022.8923750587728,5029.6620170356255,5036.4328064595338,5043.2047420153740,
5049.9778223910289,5056.7520462773873,5063.5274123683230,5070.3039193606965,
5077.0815659543314,5083.8603508520155,5090.6402727594887,5097.4213303854249,
5104.2035224414321,5110.9868476420361,5117.7713047046727,5124.5568923496812,
5131.3436093002856,5138.1314542825949,5144.9204260255883,5151.7105232611011,
5158.5017447238279,5165.2940891512990,5172.0875552838788,5178.8821418647549,
5185.6778476399286,5192.4746713582035,5199.2726117711791,5206.0716676332377,
5212.8718377015393,5219.6731207360117,5226.4755154993354,5233.2790207569433,
5240.0836352770057,5246.8893578304233,5253.6961871908152,5260.5041221345155,
5267.3131614405584,5274.1233038906739,5280.9345482692752,5287.7468933634518,
5294.5603379629629,5301.3748808602231,5308.1905208502976,5315.0072567308925,
5321.8250873023462,5328.6440113676217,5335.4640277322960,5342.2851352045527,
5349.1073325951729,5355.9306187175289,5362.7549923875722,5369.5804524238274,
5376.4069976473838,5383.2346268818874,5390.0633389535287,5396.8931326910406,
5403.7240069256877,5410.5559604912523,5417.3889922240396,5424.2231009628522,
5431.0582855490002,5437.8945448262766,5444.7318776409629,5451.5702828418107,
5458.4097592800390,5465.2503058093280,5472.0919212858053,5478.9346045680431,
5485.7783545170496,5492.6231699962582,5499.4690498715227,5506.3159930111078,
5513.1639982856841,5520.0130645683175,5526.8631907344625,5533.7143756619571,
5540.5666182310088,5547.4199173241941,5554.2742718264499,5561.1296806250593,
5567.9861426096541,5574.8436566721994,5581.7022217069916,5588.5618366106446,
5595.4225002820931,5602.2842116225747,5609.1469695356263,5616.0107729270785,
5622.8756207050501,5629.7415117799328,5636.6084450643948,5643.4764194733652,
5650.3454339240307,5657.2154873358286,5664.0865786304394,5670.9587067317789,
5677.8318705659913,5684.7060690614444,5691.5813011487207,5698.4575657606110,
5705.3348618321088,5712.2131883004004,5719.0925441048603,5725.9729281870468,
5732.8543394906892,5739.7367769616867,5746.6202395480996,5753.5047262001426,
5760.3902358701780,5767.2767675127088,5774.1643200843737,5781.0528925439385,
5787.9424838522928,5794.8330929724398,5801.7247188694928,5808.6173605106651,
5815.5110168652664,5822.4056869046999,5829.3013696024482,5836.1980639340709,
5843.0957688771996,5849.9944834115304,5856.8942065188148,5863.7949371828599,
5870.6966743895164,5877.5994171266748,5884.5031643842594,5891.4079151542210,
5898.3136684305327,5905.2204232091808,5912.1281784881630,5919.0369332674782,
5925.9466865491231,5932.8574373370848,5939.7691846373364,5946.6819274578302,
5953.5956648084903,5960.5103957012088,5967.4261191498399,5974.3428341701929,
5981.2605397800289,5988.1792349990492,5995.0989188488966,6002.0195903531458,
6008.9412485372959,6015.8638924287725,6022.7875210569109,6029.7121334529593,
6036.6377286500701,6043.5643056832923,6050.4918635895701,6057.4204014077350,
6064.3499181784991,6071.2804129444503,};
    return (k < 1024) ? table[k] : logGamma(k + 1.0);
}

/******************************************************************************
static double TABLElogA[32768];   // quadratic interpolant of Log
static double TABLElogB[32768];   // linear    interpolant of Log
static double TABLElogC[32768];   // constant  interpolant of Log
    int    i;
    double u, v, w;
// lookup tables for fast Log
    for( i = 0; i < 32768; i++ )
    {
        u = log(1.0 + (i+0.0) / 32768.) - ((i+0.0) / 32768.) * log(2.0);
        v = log(1.0 + (i+0.5) / 32768.) - ((i+0.5) / 32768.) * log(2.0);
        w = log(1.0 + (i+1.0) / 32768.) - ((i+1.0) / 32768.) * log(2.0);
        TABLElogC[i] = u;
        TABLElogB[i] = (4. * v - 3. * u - w)
                      / 137438953472.;
        TABLElogA[i] = (2. * w - 4. * v + 2. * u)
                      / (137438953472. * 137438953472.);
    }

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  Log
// 
// Purpose:   Fast calculation of log( x ), saving about 50%.
//
// Method:    Assume the "double" input x is in IEEE format, essentially
//                   2^exponent * (1 + mantissa).
//            where the exponent is an integer and 0 <= mantissa < 1.
//            We aim to squeeze the logarithm
//                   log(x) = exponent * log(2) + log(1 + mantissa)
//            by correcting the initial approximation
//                   estimate  = (exponent + mantissa) * log(2)
//            which comes from reading the IEEE format as a 64bit integer.
//            The correction is
//                   misfit = log(1 + mantissa) - mantissa * log(2)
//            The mantissa range [0,1) is broken into 32768 intervals, within
//            each of which lookup quadratic interpolation is used:
//                   misfit = C[i] + (B[i] + A[i] * f) * f
//            where  f = mantissa * 2^37 = fractional interval.
//
// History:   John Skilling  17 Feb 2012
//-----------------------------------------------------------------------------
static double Log(     //   O  logarithm, computed fast
double  x)             // I    Argument
{
    slong  IEEE = *(slong*)&x;                // exponent+1023; 52bit mantissa.
    int    j = (IEEE >> 37) & 32767;          // mantissa index
    double f = (double)(IEEE & 0x1fffffffff); // mantissa fraction
    if( IEEE >= 0x0010000000000000 && IEEE < 0x7ff0000000000000 )
        return (slong)(IEEE - 0x3ff0000000000000) * 1.53909591862332397618e-16
                       + TABLElogC[j] + (TABLElogB[j] + TABLElogA[j] * f) * f;
    else
        return log(x);  // (unnormalised or out of range)
}
******************************************************************************/

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  NumOnes32 and NumOnes64
//
// Purpose:   Number of '1' bits in integer.
//
// Method:    Pairwise evaluation followed by progressive summation,
//            ref: "Hacker's Delight".
//
// History:   John Skilling   18 August 2014
//-----------------------------------------------------------------------------
int NumOnes32(    //   O  number of '1' bits set
unsigned k)       // I    32-bit integer
{
    k -= (k>>1) & 0x55555555;          // each 2-bit pair becomes # set bits
                                       // 00 -> 0, 01 -> 1, 10 -> 1, 11 -> 2

    k = (k & 0x33333333)               // each 4-bit digit becomes # set bits,
       + ((k>>2) & 0x33333333);        // calculated as evenpair + oddpair

    k = (k + (k>>4))                   // each 8-bit char becomes #set bits
       & 0x0f0f0f0f;                   // but kill odd digits before next step

    k *= 0x01010101;                   // put sum of 8-bit totals into top char
    return k >> 24;                    // return top char
}
//-----------------------------------------------------------------------------
int NumOnes64(    //   O  number of '1' bits set
ulong  k)         // I    64-bit integer
{
    k -= (k>>1) & 0x5555555555555555;  // each 2-bit pair becomes # set bits
                                       // 00 -> 0, 01 -> 1, 10 -> 1, 11 -> 2

    k = (k & 0x3333333333333333)       // each 4-bit digit becomes # set bits,
       + ((k>>2) & 0x3333333333333333);// calculated as evenpair + oddpair

    k = (k + (k>>4))                   // each 8-bit char becomes # set bits
       & 0x0f0f0f0f0f0f0f0f;           // but kill odd digits before next step

    k *= 0x0101010101010101;           // put sum of 8-bit totals into top char
    return k >> 56;                    // return top char
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  RanDate
// 
// Purpose:   Translate time-seed to printable date and time, such as
//               "Wed Feb 29 14:45:13 2012"    from   seed=1330526713.
// 
// History:   John Skilling  29 Feb 2012
//-----------------------------------------------------------------------------
void RanDate(
char*    date,    //   O  user's 25-character string, including NULL terminator
int      seed)    // I    time-seed as actually used
{
    time_t  t;
    t =  (time_t)((time(NULL) & 0xffffffff80000000) | seed);
                                                    // still OK after 2038 AD
    strncpy(date, ctime(&t), 24);                   // ignore ctime's newline
    date[24] = 0;                                   // terminate output string
}


} // namespace

#endif

