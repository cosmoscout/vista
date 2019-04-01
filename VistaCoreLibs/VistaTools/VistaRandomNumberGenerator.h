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


#ifndef _VISTARANDOMNUMBERGENERATOR_H
#define _VISTARANDOMNUMBERGENERATOR_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "VistaToolsConfig.h"
#include <algorithm>
#include <utility>
#include <vector>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/
//#define N 624

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
class VISTATOOLSAPI VistaRandomNumberGenerator
{
public:
	VistaRandomNumberGenerator();
	VistaRandomNumberGenerator( const VistaRandomNumberGenerator& oCopy );
	~VistaRandomNumberGenerator();

	/**
	 * returns standard RNG - only use from the main thread.
	 * The standard RNG is created on first call, and is initialized
	 * with the default seed. However, the VistaSystem will set a random
	 * seed (same on all nodes in a cluster) during its initialization.
	 */
	static VistaRandomNumberGenerator* GetStandardRNG();
	/**
	 * returns a specific RNG for the calling thread.
	 * Will be created on first request with the default seed.
	 */
	static VistaRandomNumberGenerator* GetThreadLocalRNG();

	void          SetSeed(unsigned int);
	void          SetSeedByArray(unsigned int init_key[], int key_length);

	/**
	 * unsigned int, 32bit resolution
	 * [0, 4294967296) <-> [0, 2^32)
	 */
	unsigned int GenerateInt32() ;
	/**
	 * signed int, 31bit resolution
	 * [0, 2147483648) <-> [0, 2^31)
	 */
	int           GenerateInt31() ;

	/**
	 * [iLowerBound, iUpperBound]
	 */
	unsigned int GenerateInt32(unsigned int iLowerBound, unsigned int iUpperBound);


	/**
	 * [0,1)
	 */
	double        GenerateDouble1() ;
	/**
	 * [0,1]
	 */
	double        GenerateDouble2() ;

	/**
	 * (0,1)
	 */
	double        GenerateDouble3() ;

	/**
	 * [min,max]
	 */
	double        GenerateDouble( double nMin, double nMax ) ;


	/**
	 * [0,1), 53bit resolution
	 */
	double        GenerateDouble53() ;



	/**
	 * [0,1)
	 */
	float         GenerateFloat1() ;
	/**
	 * [0,1]
	 */
	float         GenerateFloat2() ;

	/**
	 * (0,1)
	 */
	float         GenerateFloat3() ;

	/**
	 * [min,max]
	 */
	float         GenerateFloat( float fMin, float fMax ) ;


	/** 
	 * Creates a random number according to a gaussian distribution
	 * with mean at 0 and Std of 1
	 * This uses the Box-muller-transform to generate a pseudo-gaussian distribution
	 */
	double GenerateGaussian();

	/** 
	 * Creates a random number according to a gaussian distribution
	 * with specified mean and Std Deviation
	 * This uses the Box-muller-transform to generate a pseudo-gaussian distribution
	 */
	double GenerateGaussian( double dMean, double dStdDev );


	/**
	 * Shuffles the given std::vector using synchronous random calls.
	 * Requires vecVector to have no more than MAX(unsigned int) entries.
	*/
	template<typename T>
	void ShuffleVector(std::vector<T>& vecVector)
	{
		for (std::size_t i = vecVector.size() - 1; i > 0; --i)
		{
			std::size_t nSwapIndex = static_cast<std::size_t>(this->GenerateInt32(0, static_cast<unsigned int>(i)));
			std::swap(vecVector[i], vecVector[nSwapIndex]);
		}
	}


protected:
private:


	//MEMBERS
	//unsigned long m_mt[N]; /* the array for the state vector  */
	unsigned int *m_mt;
	unsigned int m_mti; //=N+1; /* mti==N+1 means mt[N] is not initialized */
};


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTARANDOMNUMBERGENERATOR_H

