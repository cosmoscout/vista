/*============================================================================*/
/*                              ViSTA VR toolkit                              */
/*               Copyright (c) 1997-2016 RWTH Aachen University               */
/*============================================================================*/
/*                                  License                                   */
/*                                                                            */
/*  This program is free software: you can redistribute it and/or modify      */
/*  it under the terms of the GNU Lesser General Public License as published  */
/*  by the Free Software Foundation, either version 3 of the License, or      */
/*  (at your option) any later version.                                       */
/*                                                                            */
/*  This program is distributed in the hope that it will be useful,           */
/*  but WITHOUT ANY WARRANTY; without even the implied warranty of            */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             */
/*  GNU Lesser General Public License for more details.                       */
/*                                                                            */
/*  You should have received a copy of the GNU Lesser General Public License  */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.     */
/*============================================================================*/
/*                                Contributors                                */
/*                                                                            */
/*============================================================================*/

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "VistaRandomNumberGenerator.h"

#include <cmath>
#include <cstddef>
#include <cstring>

/*============================================================================*/
/*  MAKROS AND DEFINES                                                        */
/*============================================================================*/
//#define M 397
//#define MATRIX_A 0x9908b0df   /* constant vector a */
//#define UPPER_MASK 0x80000000 /* most significant w-r bits */
//#define LOWER_MASK 0x7fffffff /* least significant r bits */

static const int          N          = 624;
static const int          M          = 397;
static const unsigned int MATRIX_A   = 0x9908b0df;
static const unsigned int UPPER_MASK = 0x80000000;
static const unsigned int LOWER_MASK = 0x7fffffff;
/*============================================================================*/
/*  IMPLEMENTATION                                                            */
/*============================================================================*/

//
VistaRandomNumberGenerator* S_pStandardRNG = NULL;

// thread-local storage
#ifdef WIN32
__declspec(thread) VistaRandomNumberGenerator* S_pThreadInstance = NULL;
#else
__thread VistaRandomNumberGenerator* S_pThreadInstance = NULL;
#endif

VistaRandomNumberGenerator::VistaRandomNumberGenerator()
    : m_mt(new unsigned int[N])
    , m_mti(N + 1) {
  memset(m_mt, 0, N * sizeof(unsigned int));
}

VistaRandomNumberGenerator::VistaRandomNumberGenerator(const VistaRandomNumberGenerator& oCopy)
    : m_mt(new unsigned int[N])
    , m_mti(oCopy.m_mti) {
  memcpy(m_mt, oCopy.m_mt, N * sizeof(unsigned int));
}

VistaRandomNumberGenerator::~VistaRandomNumberGenerator() {
  delete[] m_mt;
}

VistaRandomNumberGenerator* VistaRandomNumberGenerator::GetStandardRNG() {
  if (S_pStandardRNG == NULL) {
    S_pStandardRNG = new VistaRandomNumberGenerator;
  }

  return S_pStandardRNG;
}

VistaRandomNumberGenerator* VistaRandomNumberGenerator::GetThreadLocalRNG() {
  if (S_pThreadInstance == NULL) {
    S_pThreadInstance = new VistaRandomNumberGenerator;
  }

  return S_pThreadInstance;
}

void VistaRandomNumberGenerator::SetSeed(unsigned int s) {
  m_mt[0] = s & 0xffffffff;
  for (m_mti = 1; m_mti < (unsigned int)N; m_mti++) {
    m_mt[m_mti] = (1812433253 * (m_mt[m_mti - 1] ^ (m_mt[m_mti - 1] >> 30)) + m_mti);
    /* See Knuth TAOCP Vol2. 3rd Ed. P.106 for mtiplier. */
    /* In the previous versions, MSBs of the seed affect   */
    /* only MSBs of the array mt[].                        */
    /* 2002/01/09 modified by Makoto Matsumoto             */
    m_mt[m_mti] &= 0xffffffff;
    /* for >32 bit machines */
  }
}

void VistaRandomNumberGenerator::SetSeedByArray(unsigned int init_key[], int key_length) {
  int i, j, k;
  SetSeed(19650218);
  i = 1;
  j = 0;
  k = (N > key_length ? N : key_length);
  for (; k; k--) {
    m_mt[i] = (m_mt[i] ^ ((m_mt[i - 1] ^ (m_mt[i - 1] >> 30)) * 1664525)) + init_key[j] +
              j;           /* non linear */
    m_mt[i] &= 0xffffffff; /* for WORDSIZE > 32 machines */
    i++;
    j++;
    if (i >= N) {
      m_mt[0] = m_mt[N - 1];
      i       = 1;
    }
    if (j >= key_length)
      j = 0;
  }
  for (k = N - 1; k; k--) {
    m_mt[i] = (m_mt[i] ^ ((m_mt[i - 1] ^ (m_mt[i - 1] >> 30)) * 1566083941)) - i; /* non linear */
    m_mt[i] &= 0xffffffff; /* for WORDSIZE > 32 machines */
    i++;
    if (i >= N) {
      m_mt[0] = m_mt[N - 1];
      i       = 1;
    }
  }

  m_mt[0] = 0x80000000; /* MSB is 1; assuring non-zero initial array */
}

unsigned int VistaRandomNumberGenerator::GenerateInt32() {
  unsigned int        y;
  static unsigned int mag01[2] = {0x0, MATRIX_A};
  /* mag01[x] = x * MATRIX_A  for x=0,1 */

  if (m_mti >= (unsigned int)N) { /* generate N words at one time */
    int kk;

    if (m_mti == N + 1) /* if init_genrand() has not been called, */
      SetSeed(5489);    /* a defat initial seed is used */

    for (kk = 0; kk < N - M; kk++) {
      y        = (m_mt[kk] & UPPER_MASK) | (m_mt[kk + 1] & LOWER_MASK);
      m_mt[kk] = m_mt[kk + M] ^ (y >> 1) ^ mag01[y & 0x1];
    }
    for (; kk < N - 1; kk++) {
      y        = (m_mt[kk] & UPPER_MASK) | (m_mt[kk + 1] & LOWER_MASK);
      m_mt[kk] = m_mt[kk + (M - N)] ^ (y >> 1) ^ mag01[y & 0x1];
    }
    y           = (m_mt[N - 1] & UPPER_MASK) | (m_mt[0] & LOWER_MASK);
    m_mt[N - 1] = m_mt[M - 1] ^ (y >> 1) ^ mag01[y & 0x1];

    m_mti = 0;
  }

  y = m_mt[m_mti++];

  /* Tempering */
  y ^= (y >> 11);
  y ^= (y << 7) & 0x9d2c5680;
  y ^= (y << 15) & 0xefc60000;
  y ^= (y >> 18);

  return y;
}

unsigned int VistaRandomNumberGenerator::GenerateInt32(
    unsigned int iLowerBound, unsigned int iUpperBound) {
  return (iLowerBound + (int)((float)(iUpperBound - iLowerBound + 1) * GenerateDouble1()));
}

int VistaRandomNumberGenerator::GenerateInt31() {
  return (int)(GenerateInt32() >> 1);
}

double VistaRandomNumberGenerator::GenerateDouble1() {
  return GenerateInt32() * (1.0 / 4294967296.0);
  /* divided by 2^32 */
}

double VistaRandomNumberGenerator::GenerateDouble2() {
  return GenerateInt32() * (1.0 / 4294967295.0);
  /* divided by 2^32-1 */
}

double VistaRandomNumberGenerator::GenerateDouble3() {
  return (((double)GenerateInt32()) + 0.5) * (1.0 / 4294967296.0);
  /* divided by 2^32 */
}

double VistaRandomNumberGenerator::GenerateDouble53() {
  unsigned int a = GenerateInt32() >> 5, b = GenerateInt32() >> 6;
  return (a * 67108864.0 + b) * (1.0 / 9007199254740992.0);
}

float VistaRandomNumberGenerator::GenerateFloat1() {
  return (GenerateInt32() >> 9) * (1.0f / 8388608.0f);
  /* divided by 2^23 */
  /* note: you can't just cast from double here, as the cast to float
   * rounds exactly; therefore value just under 1 will become 1.
   * Instead, we generate a 23bit random integer number (IEEE 754 specifies
   * floats to have 23 digits) and divide it by 2^23-1 */
}

float VistaRandomNumberGenerator::GenerateFloat2() {
  return (GenerateInt32() >> 9) * (1.0f / 8388607.0f);
  /* divided by 2^23-1 */
}

float VistaRandomNumberGenerator::GenerateFloat3() {
  return ((GenerateInt32() >> 9) + 1) * (1.0f / 8388609.0f);
  /* divide by 2^23+1 */
}

double VistaRandomNumberGenerator::GenerateGaussian() {
  double x1, x2, w;

  do {
    x1 = 2.0 * GenerateDouble2() - 1.0;
    x2 = 2.0 * GenerateDouble2() - 1.0;
    w  = x1 * x1 + x2 * x2;
  } while (w >= 1.0);

  w = sqrt((-2.0 * log(w)) / w);

  return x1 * w;
}

double VistaRandomNumberGenerator::GenerateGaussian(double dMean, double dStdDev) {
  return dMean + dStdDev * GenerateGaussian();
}

double VistaRandomNumberGenerator::GenerateDouble(double nMin, double nMax) {
  return (nMin + (nMax - nMin) * GenerateDouble2());
}

float VistaRandomNumberGenerator::GenerateFloat(float fMin, float fMax) {
  return (fMin + (fMax - fMin) * GenerateFloat2());
}

/*============================================================================*/
/*  LOCAL VARS / FUNCTIONS                                                    */
/*============================================================================*/
