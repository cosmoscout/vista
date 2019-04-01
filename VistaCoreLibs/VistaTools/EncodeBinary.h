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


/**
*		NOTE:	Due to the usage of sizeof(...) you might encouter problems, when using
*				these routines for communication purposes between 64-bit and 32-bit
*				architectures.
*				Encode will always yield an encoded sequence of bytes which reembles
*				the given input on that specific architecture.
*				Decode will either yield the decoded output or terminate if the types
*				size doesn't match the length of the given character string.
*/

#ifndef _VISTAENCODEBINARY_H
#define _VISTAENCODEBINARY_H

/**
*	Allocate memory and encode <in> to char* <out>
*	@param 		T		in	    template parameter for basic C++ data type
*	@param 	char*&	out		pointer to the encoded byte seuqence
*	@param		int&	size	size of encoded byte sequence
**/
template <class T>
void AllocateEncodeBinary(T in, char*& out, int& size){
	size = sizeof(T);
	out = new char[size];
	char* akt = (char*) &in;
	for(register int i=0; i<size; ++i)
		out[i] = akt[i];
}

/**
*	Encode <in> to char* <out>
*	@param  	T		in		template parameter for basic C++ data type
*	@param 	char*&	out		pointer to the encoded byte seuqence
*	@param 	int&	size	size of encoded byte sequence
**/
template<class T>
void EncodeBinary(T in, char* out, int& size){
	size = sizeof(T);
	char* akt = (char*) &in;
	for(register int i=0; i<size; ++i)
		out[i] = akt[i];
}

/**
*	Decode <in> to typed output <out>
*	@param 		char*	in		byte sequence conatining the value to be decoded
*	@param 		int		size	length of the input sequence in #bytes
*	@param 	T&		out		reference to the output
**/
template<class T>
void DecodeBinary(char* in, int size, T& out){
	if(size!=sizeof(T))
		return;
	char* akt = (char*) &out;
	for(register int i=0; i<size; ++i)
		akt[i] = in[i];
}

/**
*	Decode <in> to typed output <out>. Additionally flips byte order as to
*	convert from little endian to big endian and v.v.
*	@param		char*	in		byte sequence conatining the value to be decoded
*	@param		int		size	length of the input sequence in #bytes
*	@param		T&		out		reference to the output
**/
template<class T>
void DecodeBinaryWithFlip(char* in, int size, T& out){
	if(size!=sizeof(T))
		return;
	char* akt = (char*) &out;
	for(register int i=0; i<size; ++i)
		akt[i] = in[size - (i+1)];
}


#endif //_VISTAENCODEBINARY_H

