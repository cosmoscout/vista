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


#include "VdfnSerializer.h" 

#include <VistaBase/VistaAutoBuffer.h>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

IVistaDeSerializer &operator>>( IVistaDeSerializer &oSer, VistaVector<float,4> &v3 )
{
	oSer.ReadFloat32( v3[0] );
	oSer.ReadFloat32( v3[1] );
	oSer.ReadFloat32( v3[2] );
	oSer.ReadFloat32( v3[3] );

	bool bTransposed = false;
	oSer.ReadBool( bTransposed );
	if( bTransposed )
		v3.Transpose();

	return oSer;
}


IVistaSerializer &operator<<( IVistaSerializer &oSer, const VistaVector<float,4> &v3 )
{
	oSer.WriteFloat32( v3[0] );
	oSer.WriteFloat32( v3[1] );
	oSer.WriteFloat32( v3[2] );
	oSer.WriteFloat32( v3[3] );
	oSer.WriteBool( v3.IsTransposed() );

	return oSer;
}

IVistaSerializer &operator<<( IVistaSerializer &oSer, const VistaVector3D &v3 )
{
	oSer.WriteFloat32( v3[0] );
	oSer.WriteFloat32( v3[1] );
	oSer.WriteFloat32( v3[2] );
	oSer.WriteFloat32( v3[3] );

	return oSer;
}

IVistaDeSerializer &operator>>( IVistaDeSerializer &oSer, VistaVector3D &v3 )
{
	oSer.ReadFloat32( v3[0] );
	oSer.ReadFloat32( v3[1] );
	oSer.ReadFloat32( v3[2] );
	oSer.ReadFloat32( v3[3] );

	return oSer;
}

IVistaSerializer &operator<<( IVistaSerializer &oSer, const VistaQuaternion &v3 )
{
	oSer.WriteFloat32( v3[0] );
	oSer.WriteFloat32( v3[1] );
	oSer.WriteFloat32( v3[2] );
	oSer.WriteFloat32( v3[3] );

	return oSer;
}

IVistaDeSerializer &operator>>( IVistaDeSerializer &oSer, VistaQuaternion &v3 )
{
	oSer.ReadFloat32( v3[0] );
	oSer.ReadFloat32( v3[1] );
	oSer.ReadFloat32( v3[2] );
	oSer.ReadFloat32( v3[3] );

	return oSer;
}

IVistaDeSerializer &operator>>( IVistaDeSerializer &oSer, VistaTransformMatrix &v3 )
{
	for(int r=0; r < 4; ++r)
		for(int c=0; c < 4; ++c)
			oSer.ReadFloat32( v3[r][c] );

	return oSer;
}

IVistaSerializer &operator<<( IVistaSerializer &oSer, const VistaTransformMatrix &v3 )
{
	for(int r=0; r < 4; ++r)
		for(int c=0; c < 4; ++c)
			oSer.WriteFloat32( v3[r][c] );
	return oSer;
}

IVistaSerializer &operator<<( IVistaSerializer &oSer, const VistaAxisAndAngle &v3 )
{
	oSer << v3.m_v3Axis
		<< v3.m_fAngle;
	return oSer;
}


IVistaDeSerializer &operator>>( IVistaDeSerializer &oSer, VistaAxisAndAngle &v3 )
{
	oSer >> v3.m_v3Axis
		>> v3.m_fAngle;
	return oSer;
}

IVistaSerializer &operator<<( IVistaSerializer &oSer, const VistaBoundingBox &bb )
{
	oSer << bb.m_v3Min[0] << bb.m_v3Min[1] << bb.m_v3Min[2] 
		 << bb.m_v3Max[0] << bb.m_v3Max[1] << bb.m_v3Max[2];
	return oSer;
}


IVistaDeSerializer &operator>>( IVistaDeSerializer &oSer, VistaBoundingBox &bb )
{
	oSer >> bb.m_v3Min[0] >> bb.m_v3Min[1] >> bb.m_v3Min[2] 
		 >> bb.m_v3Max[0] >> bb.m_v3Max[1] >> bb.m_v3Max[2];

	return oSer;
}

IVistaSerializer &operator<<( IVistaSerializer &oSer, const VistaAutoBuffer &ab )
{
	VistaType::sint32 size = (VistaType::sint32)ab.size();
	oSer.WriteInt32( size );
	
	oSer.WriteRawBuffer( ab.data(), size );
	return oSer;
}


IVistaDeSerializer &operator>>( IVistaDeSerializer &oSer, VistaAutoBuffer &ab )
{
	VistaType::sint32 size;
	oSer.ReadInt32( size );
	
	VistaAutoWriteBuffer wb( size );
	oSer.ReadRawBuffer( wb.data(), size );
	ab = wb;
	return oSer;
}

IVistaDeSerializer &operator>>(IVistaDeSerializer &oDeSer,   VistaAutoBuffer *pBuffer )
{
	return oDeSer >> (*pBuffer);
}


IVistaSerializer   &operator<<(IVistaSerializer &oSer, const VistaAutoBuffer *pBuffer )
{
	return oSer << (*pBuffer);
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


