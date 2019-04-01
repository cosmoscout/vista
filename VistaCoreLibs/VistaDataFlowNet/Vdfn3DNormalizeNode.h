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


#ifndef _VDFN3DNORMALIZENODE_H
#define _VDFN3DNORMALIZENODE_H


/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "VdfnConfig.h"
#include "VdfnNode.h"
#include "VdfnPort.h"
#include "VdfnSerializer.h"
#include "VdfnNodeFactory.h"

#include <VistaAspects/VistaPropertyAwareable.h>
#include <VistaBase/VistaExceptionBase.h>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * Normalizes a 3D coordinate from [0;source_max] to [min_x;min_x+target_w]
 * in each dimension. This node is useful if, for example you need a matrix
 * that will compute a normalized coordinate, for example a pixel point relative
 * to a window. As the source width and height may change, they are in-ports.
 * To cut a long story short: the output is a matrix that you can use in the
 * usual way to 'normalize' a 3D vector v: transform * v = v'
 *
 * @ingroup VdfnNodes
 * @inport{min_x,float,optional,the minimum x value to normalize to}
 * @inport{min_y,float,optional,the minimum y value to normalize to}
 * @inport{min_z,float,optional,the minimum z value to normalize to}
 * @inport{target_z,float}
 * @inport{target_y,float}
 * @inport{target_x,float}
 * @inport{source_w,float}
 * @inport{source_d,float}
 * @inport{source_h,float}
 * @outport{transform,VistaTransformMatrix}
 */
class VISTADFNAPI Vdfn3DNormalizeNode : public IVdfnNode
{
public:
	Vdfn3DNormalizeNode();

	bool GetIsValid() const;
	bool PrepareEvaluationRun();

	void SetMinX( float f ) { m_minx = f; };
	void SetMinY( float f ) { m_miny = f; };
	void SetMinZ( float f ) { m_minz = f; };

	void SetFlipX( bool b ) { m_flipx = b; };
	void SetFlipY( bool b ) { m_flipy = b; };
	void SetFlipZ( bool b ) { m_flipz = b; };

	void SetTargetW( float f ) { m_targetw = f; };
	void SetTargetH( float f ) { m_targeth = f; };
	void SetTargetD( float f ) { m_targetd = f; };

protected:
	bool DoEvalNode();

	TVdfnPort<float> *m_pMinX,
					 *m_pMinY,
					 *m_pMinZ,
					 *m_pTgW,
					 *m_pTgH,
					 *m_pTgD,
					 *m_pSrW,
					 *m_pSrH,
					 *m_pSrD;

	TVdfnPort<bool> *m_pFlipX,
					*m_pFlipY,
					*m_pFlipZ;

	float m_minx;
	float m_miny;
	float m_minz;

	float m_targetw;
	float m_targeth;
	float m_targetd;

	bool m_flipx;
	bool m_flipy;
	bool m_flipz;

	TVdfnPort<VistaTransformMatrix> *m_pOut;

};

class VISTADFNAPI Vista3DNormalizeNodeCreate : public VdfnNodeFactory::IVdfnNodeCreator
{
public:
	Vista3DNormalizeNodeCreate()
	{

	}

	virtual IVdfnNode *CreateNode( const VistaPropertyList &oParams ) const
	{
		Vdfn3DNormalizeNode *pNode = new Vdfn3DNormalizeNode;
        try {
            const VistaPropertyList &prams = oParams.GetPropertyConstRef("param").GetPropertyListConstRef();
			float fValue;
			bool bFlipState = false;
            if( prams.GetValue( "min_x", fValue ) ) {
                pNode->SetMinX( fValue );
            }
			if( prams.GetValue( "min_y", fValue ) ) {
                pNode->SetMinY( fValue );
            }
			if( prams.GetValue( "min_z", fValue ) ) {
                pNode->SetMinZ( fValue );
            }
			if( prams.GetValue( "target_w", fValue ) ) {
                pNode->SetTargetW( fValue );
            }
			if( prams.GetValue( "target_h", fValue ) ) {
                pNode->SetTargetH( fValue );
            }
			if( prams.GetValue( "target_d", fValue ) ) {
                pNode->SetTargetD( fValue );
            }
			if( prams.GetValue( "flip_x", bFlipState ) ) {
                pNode->SetFlipX( bFlipState );
            }
			if( prams.GetValue( "flip_y", bFlipState ) ) {
                pNode->SetFlipY( bFlipState );
            }
			if( prams.GetValue( "flip_z", bFlipState ) ) {
                pNode->SetFlipZ( bFlipState );
            }

        } catch (VistaExceptionBase &x) {
            x.PrintException();
        }
        return pNode;
	}
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VDFN3DNORMALIZENODE_H

