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



#include <VistaDeviceDriversBase/VistaDeviceSensor.h>
#include "VistaLeapMotionCommonShare.h"

#include <VistaAspects/VistaAspectsUtils.h>
#include <time.h>
#include <string.h>

#include <VistaDeviceDriversBase/VistaDriverPlugDev.h>
#include "VistaBase/VistaUtilityMacros.h"

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS                                  */
/*============================================================================*/

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/
namespace
{	
	class LeapMotionHandTranscoder : public IVistaMeasureTranscode
	{
		REFL_INLINEIMP( LeapMotionHandTranscoder, IVistaMeasureTranscode )
	public:
		LeapMotionHandTranscoder()
		{
		}

		static std::string GetTypeString() { return "LeapMotionHandTranscoder"; }
	};
	class LeapMotionFingersTranscoder : public IVistaMeasureTranscode
	{
		REFL_INLINEIMP( LeapMotionFingersTranscoder, IVistaMeasureTranscode )
	public:
		LeapMotionFingersTranscoder()
		{
		}

		static std::string GetTypeString() { return "LeapMotionFingersTranscoder"; }
	};
	class LeapMotionToolsTranscoder : public IVistaMeasureTranscode
	{
		REFL_INLINEIMP( LeapMotionToolsTranscoder, IVistaMeasureTranscode )
	public:
		LeapMotionToolsTranscoder()
		{
		}

		static std::string GetTypeString() { return "LeapMotionToolsTranscoder"; }
	};
	class LeapMotionGestureTranscoder : public IVistaMeasureTranscode
	{
		REFL_INLINEIMP( LeapMotionGestureTranscoder, IVistaMeasureTranscode )
	public:
		LeapMotionGestureTranscoder()
		{
		}

		static std::string GetTypeString() { return "LeapMotionGestureTranscoder"; }
	};
	class LeapMotionImageTranscoder : public IVistaMeasureTranscode
	{
		REFL_INLINEIMP( LeapMotionImageTranscoder, IVistaMeasureTranscode )
	public:
		LeapMotionImageTranscoder()
		{
		}

		//@TODO implement getters, how to acces image data?
		static std::string GetTypeString() { return "LeapMotionImageTranscoder"; }
	};

	class LeapMotionHandOrientationGet : public IVistaMeasureTranscode::TTranscodeValueGet< VistaQuaternion >
	{
	public:
		LeapMotionHandOrientationGet()
		: IVistaMeasureTranscode::TTranscodeValueGet< VistaQuaternion >( "ORIENTATION", "LeapMotionHandTranscoder", "orientation of the palm" )
		{
		}

		VistaQuaternion GetValue( const VistaSensorMeasure* pMeasure ) const
		{
			VistaQuaternion oValue;
			GetValue( pMeasure, oValue );
			return oValue;
		};

		bool GetValue( const VistaSensorMeasure * pMeasure, VistaQuaternion& oValue ) const
		{
			if( pMeasure == NULL )
				return false;

			const VistaLeapMotionMeasures::HandMeasure* pRealMeasure = pMeasure->getRead< VistaLeapMotionMeasures::HandMeasure >();
			VistaTransformMatrix matTransform( -pRealMeasure->m_v3PalmDirection.Cross( pRealMeasure->m_v3PalmNormal ),
														-pRealMeasure->m_v3PalmNormal,
														-pRealMeasure->m_v3PalmDirection );
			oValue = matTransform.GetRotationAsQuaternion();
			return true;
		}
	};
	class LeapMotionHandTransformGet : public IVistaMeasureTranscode::TTranscodeValueGet< VistaTransformMatrix >
	{
	public:
		LeapMotionHandTransformGet()
		: IVistaMeasureTranscode::TTranscodeValueGet< VistaTransformMatrix >( "TRANSFORM", "LeapMotionHandTranscoder", "transformation of the palm" )
		{
		}

		VistaTransformMatrix GetValue( const VistaSensorMeasure* pMeasure ) const
		{
			VistaTransformMatrix oValue;
			GetValue( pMeasure, oValue );
			return oValue;
		};

		bool GetValue( const VistaSensorMeasure * pMeasure, VistaTransformMatrix& oValue ) const
		{
			if( pMeasure == NULL )
				return false;

			const VistaLeapMotionMeasures::HandMeasure* pRealMeasure = pMeasure->getRead< VistaLeapMotionMeasures::HandMeasure >();
			oValue = VistaTransformMatrix();
			auto cross =  -pRealMeasure->m_v3PalmDirection.Cross( pRealMeasure->m_v3PalmNormal );
			cross[3] = 0.f;
			oValue = VistaTransformMatrix(VistaVector3D( pRealMeasure->m_v3PalmPosition ))*
					VistaTransformMatrix(cross,
										-pRealMeasure->m_v3PalmNormal,
										-pRealMeasure->m_v3PalmDirection );
			return true;
		}
	};

	template < typename TArrayType, int nArraySize, typename TMemberType, typename TGetValueType = TMemberType >
	class LeapMotionHandArraySubValuesVectorGet : public IVistaMeasureTranscode::TTranscodeValueGet< std::vector< TGetValueType > >
	{
	public:
		typedef TArrayType SubArray[nArraySize];
		typedef SubArray (VistaLeapMotionMeasures::HandMeasure::*SubList);
		typedef TMemberType (TArrayType::*MemberVariable);

		LeapMotionHandArraySubValuesVectorGet( const std::string& sPropName,
								const std::string& sTranscodeName,
								const std::string& sHelpText,
								SubList pmSubList,
								MemberVariable pmMemberVariable )
		: IVistaMeasureTranscode::TTranscodeValueGet< std::vector< TGetValueType > >( sPropName, sTranscodeName, sHelpText )
		, m_pmSubList( pmSubList )
		, m_pmMemberVariable( pmMemberVariable )
		{
		}

		std::vector< TGetValueType > GetValue( const VistaSensorMeasure* pMeasure ) const
		{
			std::vector< TGetValueType > oValue;
			GetValue( pMeasure, oValue );
			return oValue;
		};

		bool GetValue( const VistaSensorMeasure * pMeasure, std::vector< TGetValueType >& vecValues ) const
		{
			if( pMeasure == NULL )
				return false;

			const VistaLeapMotionMeasures::HandMeasure* pRealMeasure = pMeasure->getRead< VistaLeapMotionMeasures::HandMeasure >();
			vecValues.resize( nArraySize );
			for( int i = 0; i < nArraySize; ++i )
			{
				vecValues[i] = (pRealMeasure->*m_pmSubList)[i].*m_pmMemberVariable;
			}
			return true;
		}
		SubList m_pmSubList;
		MemberVariable m_pmMemberVariable;
	};
	template < typename TArrayType, int nArraySize, typename TMemberType, typename TGetValueType = TMemberType >
	class LeapMotionHandArraySubValuesIndexedGet : public IVistaMeasureTranscode::TTranscodeIndexedGet< TGetValueType >
	{
	public:
		typedef TArrayType SubArray[nArraySize];
		typedef SubArray (VistaLeapMotionMeasures::HandMeasure::*SubList);
		typedef TMemberType (TArrayType::*MemberVariable);

		LeapMotionHandArraySubValuesIndexedGet( const std::string& sPropName,
								const std::string& sTranscodeName,
								const std::string& sHelpText,
								SubList pmSubList,
								MemberVariable pmMemberVariable )
		: IVistaMeasureTranscode::TTranscodeIndexedGet< TGetValueType >( sPropName, sTranscodeName, sHelpText )
		, m_pmSubList( pmSubList )
		, m_pmMemberVariable( pmMemberVariable )
		{			
		}

		bool  GetValueIndexed(const VistaSensorMeasure* pMeasure, TGetValueType& oValue, unsigned int nIdx ) const
		{
			if( pMeasure == NULL )
				return false;

			if( nIdx < 0 || nIdx >= nArraySize )
				return false;

			const VistaLeapMotionMeasures::HandMeasure* pRealMeasure = pMeasure->getRead< VistaLeapMotionMeasures::HandMeasure >();
			oValue = (pRealMeasure->*m_pmSubList)[nIdx].*m_pmMemberVariable;
			return true;
		}

		virtual unsigned int GetNumberOfIndices() const
		{
			return nArraySize;
		}

		SubList m_pmSubList;
		MemberVariable m_pmMemberVariable;
	};
	
	IVistaPropertyGetFunctor *SaHandGetter[] =
	{
		new IVistaMeasureTranscode::TTranscodeMemberGet< VistaLeapMotionMeasures::HandMeasure, bool >( "VISIBLE", 
															LeapMotionHandTranscoder::GetTypeString(),
															"hand visibility flag", 
															&VistaLeapMotionMeasures::HandMeasure::m_bVisible ),
		new IVistaMeasureTranscode::TTranscodeMemberGet< VistaLeapMotionMeasures::HandMeasure, int >( "ID",
															LeapMotionHandTranscoder::GetTypeString(),
															"leap id of the hand", 
															&VistaLeapMotionMeasures::HandMeasure::m_nId ),
		new IVistaMeasureTranscode::TTranscodeMemberGet< VistaLeapMotionMeasures::HandMeasure, float, VistaType::microtime >( "TIME_VISIBLE", 
															LeapMotionHandTranscoder::GetTypeString(),
															"how long the hand has been visible",
															&VistaLeapMotionMeasures::HandMeasure::m_nTimeVisible ),

		new IVistaMeasureTranscode::TTranscodeMemberGet< VistaLeapMotionMeasures::HandMeasure, float >( "PALM_WIDTH", 
															LeapMotionHandTranscoder::GetTypeString(),
															"width of the hand's palm",
															&VistaLeapMotionMeasures::HandMeasure::m_nPalmWidth ),
		new IVistaMeasureTranscode::TTranscodeMemberGet< VistaLeapMotionMeasures::HandMeasure, float >( "ARM_WIDTH", 
															LeapMotionHandTranscoder::GetTypeString(),
															"avg. width of the arm",
															&VistaLeapMotionMeasures::HandMeasure::m_nPalmWidth ),
		new IVistaMeasureTranscode::TTranscodeMemberGet< VistaLeapMotionMeasures::HandMeasure, float >( "CONFIDENCE", 
															LeapMotionHandTranscoder::GetTypeString(),
															"confidence in the hand's posture",
															&VistaLeapMotionMeasures::HandMeasure::m_nConfidence ),
		new IVistaMeasureTranscode::TTranscodeMemberGet< VistaLeapMotionMeasures::HandMeasure, bool >( "IS_LEFT", 
															LeapMotionHandTranscoder::GetTypeString(),
															"returns wether the hand is a left one",
															&VistaLeapMotionMeasures::HandMeasure::m_bIsLeft ),
		new IVistaMeasureTranscode::TTranscodeMemberGet< VistaLeapMotionMeasures::HandMeasure, bool >( "IS_RIGHT", 
															LeapMotionHandTranscoder::GetTypeString(),
															"returns wether the hand is a right one",
															&VistaLeapMotionMeasures::HandMeasure::m_bIsRight ),
		new IVistaMeasureTranscode::TTranscodeMemberGet< VistaLeapMotionMeasures::HandMeasure, VistaVector3D >( "PALM_POSITION", 
															LeapMotionHandTranscoder::GetTypeString(),
															"position of the hand's palm",
															&VistaLeapMotionMeasures::HandMeasure::m_v3PalmPosition ),
		new IVistaMeasureTranscode::TTranscodeMemberGet< VistaLeapMotionMeasures::HandMeasure, VistaVector3D >( "PALM_POSITION_SMOOTH", 
															LeapMotionHandTranscoder::GetTypeString(),
															"smoothed position of the hand's palm",
															&VistaLeapMotionMeasures::HandMeasure::m_v3StabilizedPalmPosition ),
		new IVistaMeasureTranscode::TTranscodeMemberGet< VistaLeapMotionMeasures::HandMeasure, VistaVector3D >( "PALM_NORMAL", 
															LeapMotionHandTranscoder::GetTypeString(),
															"normal of the hand's palm",
															&VistaLeapMotionMeasures::HandMeasure::m_v3PalmNormal ),
		new IVistaMeasureTranscode::TTranscodeMemberGet< VistaLeapMotionMeasures::HandMeasure, VistaVector3D >( "PALM_VELOCITY", 
															LeapMotionHandTranscoder::GetTypeString(),
															"velocity of the hand's palm",
															&VistaLeapMotionMeasures::HandMeasure::m_v3PalmVelocity ),
		new IVistaMeasureTranscode::TTranscodeMemberGet< VistaLeapMotionMeasures::HandMeasure, VistaVector3D >( "PALM_DIRECTION", 
															LeapMotionHandTranscoder::GetTypeString(),
															"direction of the hand's palm",
															&VistaLeapMotionMeasures::HandMeasure::m_v3PalmDirection ),
		new IVistaMeasureTranscode::TTranscodeMemberGet< VistaLeapMotionMeasures::HandMeasure, VistaVector3D >( "ELBOW_POSITION", 
															LeapMotionHandTranscoder::GetTypeString(),
															"position of the hand's palm",
															&VistaLeapMotionMeasures::HandMeasure::m_v3ElbowPosition ),
		new IVistaMeasureTranscode::TTranscodeMemberGet< VistaLeapMotionMeasures::HandMeasure, VistaVector3D >( "WRIST_POSITION", 
															LeapMotionHandTranscoder::GetTypeString(),
															"position of the hand's palm",
															&VistaLeapMotionMeasures::HandMeasure::m_v3WristPostion ),

		new LeapMotionHandOrientationGet(),
		new LeapMotionHandTransformGet(),

		new LeapMotionHandArraySubValuesVectorGet< VistaLeapMotionMeasures::Finger, 5, bool, int >(
															"FINGERS_VISIBLE", 
															LeapMotionHandTranscoder::GetTypeString(),
															"position of the finger tips",
															&VistaLeapMotionMeasures::HandMeasure::m_a5oFingers,
															&VistaLeapMotionMeasures::Finger::m_bVisible ),
		new LeapMotionHandArraySubValuesVectorGet< VistaLeapMotionMeasures::Finger, 5, VistaVector3D >(
															"FINGERS_JOINT_0",
															LeapMotionHandTranscoder::GetTypeString(),
															"position of the finger tips",
															&VistaLeapMotionMeasures::HandMeasure::m_a5oFingers,
															&VistaLeapMotionMeasures::Finger::m_v3MetacarpalBoneStartPoint ),
		new LeapMotionHandArraySubValuesVectorGet< VistaLeapMotionMeasures::Finger, 5, VistaVector3D >(
															"FINGERS_JOINT_1",
															LeapMotionHandTranscoder::GetTypeString(),
															"position of the finger tips",
															&VistaLeapMotionMeasures::HandMeasure::m_a5oFingers,
															&VistaLeapMotionMeasures::Finger::m_v3ProximalBoneStartPoint ),
		new LeapMotionHandArraySubValuesVectorGet< VistaLeapMotionMeasures::Finger, 5, VistaVector3D >(
															"FINGERS_JOINT_2",
															LeapMotionHandTranscoder::GetTypeString(),
															"position of the finger tips",
															&VistaLeapMotionMeasures::HandMeasure::m_a5oFingers,
															&VistaLeapMotionMeasures::Finger::m_v3IntermediateBoneStartPoint ),
		new LeapMotionHandArraySubValuesVectorGet< VistaLeapMotionMeasures::Finger, 5, VistaVector3D >(
															"FINGERS_JOINT_3",
															LeapMotionHandTranscoder::GetTypeString(),
															"position of the finger tips",
															&VistaLeapMotionMeasures::HandMeasure::m_a5oFingers,
															&VistaLeapMotionMeasures::Finger::m_v3DistalBoneStartPoint ),
		new LeapMotionHandArraySubValuesVectorGet< VistaLeapMotionMeasures::Finger, 5, VistaVector3D >(
															"FINGERS_TIP",
															LeapMotionHandTranscoder::GetTypeString(),
															"position of the finger tips",
															&VistaLeapMotionMeasures::HandMeasure::m_a5oFingers,
															&VistaLeapMotionMeasures::Finger::m_v3TipPosition ),
		new LeapMotionHandArraySubValuesVectorGet< VistaLeapMotionMeasures::Finger, 5, VistaVector3D >(
															"FINGERS_SMOOTHED_TIP",
															LeapMotionHandTranscoder::GetTypeString(),
															"smoothed position of the finger tips",
															&VistaLeapMotionMeasures::HandMeasure::m_a5oFingers,
															&VistaLeapMotionMeasures::Finger::m_v3StabilizedTipPosition ),
		new LeapMotionHandArraySubValuesVectorGet< VistaLeapMotionMeasures::Finger, 5, VistaVector3D >(
															"FINGERS_DIRECTION",
															LeapMotionHandTranscoder::GetTypeString(),
															"directions of the finger tips",
															&VistaLeapMotionMeasures::HandMeasure::m_a5oFingers,
															&VistaLeapMotionMeasures::Finger::m_v3Direction ),
		new LeapMotionHandArraySubValuesVectorGet< VistaLeapMotionMeasures::Finger, 5, VistaVector3D >(
															"FINGERS_VELOCITY",
															LeapMotionHandTranscoder::GetTypeString(),
															"velocities of the finger tips",
															&VistaLeapMotionMeasures::HandMeasure::m_a5oFingers,
															&VistaLeapMotionMeasures::Finger::m_v3TipVelocity ),
		new LeapMotionHandArraySubValuesVectorGet< VistaLeapMotionMeasures::Finger, 5, float >(
															"FINGERS_LENGTH",
															LeapMotionHandTranscoder::GetTypeString(),
															"lengths of the fingers",
															&VistaLeapMotionMeasures::HandMeasure::m_a5oFingers,
															&VistaLeapMotionMeasures::Finger::m_nLength ),
		new LeapMotionHandArraySubValuesVectorGet< VistaLeapMotionMeasures::Finger, 5, float >(
															"FINGERS_WIDTH",
															LeapMotionHandTranscoder::GetTypeString(),
															"widths of the fingers",
															&VistaLeapMotionMeasures::HandMeasure::m_a5oFingers,
															&VistaLeapMotionMeasures::Finger::m_nWidth ),
		new LeapMotionHandArraySubValuesVectorGet< VistaLeapMotionMeasures::Finger, 5, int >(
															"FINGERS_ID",
															LeapMotionHandTranscoder::GetTypeString(),
															"leap ids of the fingers",
															&VistaLeapMotionMeasures::HandMeasure::m_a5oFingers,
															&VistaLeapMotionMeasures::Finger::m_nId ),
		new LeapMotionHandArraySubValuesVectorGet< VistaLeapMotionMeasures::Finger, 5, float >(
															"FINGERS_TOUCHDISTANCE",
															LeapMotionHandTranscoder::GetTypeString(),
															"distance of finger tips from virtual leap touch plane",
															&VistaLeapMotionMeasures::HandMeasure::m_a5oFingers,
															&VistaLeapMotionMeasures::Finger::m_nTouchDistance ),
		new LeapMotionHandArraySubValuesVectorGet< VistaLeapMotionMeasures::Finger, 5, bool, int >(
															"FINGERS_EXTENDED",
															LeapMotionHandTranscoder::GetTypeString(),
															"state of extension of the fingers",
															&VistaLeapMotionMeasures::HandMeasure::m_a5oFingers,
															&VistaLeapMotionMeasures::Finger::m_bExtended ),
		new LeapMotionHandArraySubValuesVectorGet< VistaLeapMotionMeasures::Finger, 5, int >(
															"FINGERS_TYPE",
															LeapMotionHandTranscoder::GetTypeString(),
															"type of finger (0: thumb, ..., 4: pinky finger",
															&VistaLeapMotionMeasures::HandMeasure::m_a5oFingers,
															&VistaLeapMotionMeasures::Finger::m_nFingerType ),

		new LeapMotionHandArraySubValuesIndexedGet< VistaLeapMotionMeasures::Finger, 5, bool, int >(
															"FINGER_VISIBLE", 
															LeapMotionHandTranscoder::GetTypeString(),
															"position of the finger tips",
															&VistaLeapMotionMeasures::HandMeasure::m_a5oFingers,
															&VistaLeapMotionMeasures::Finger::m_bVisible ),
		new LeapMotionHandArraySubValuesIndexedGet< VistaLeapMotionMeasures::Finger, 5, VistaVector3D >(
															"FINGER_JOINT_0",
															LeapMotionHandTranscoder::GetTypeString(),
															"position of the finger tips",
															&VistaLeapMotionMeasures::HandMeasure::m_a5oFingers,
															&VistaLeapMotionMeasures::Finger::m_v3MetacarpalBoneStartPoint ),
		new LeapMotionHandArraySubValuesIndexedGet< VistaLeapMotionMeasures::Finger, 5, VistaVector3D >(
															"FINGER_JOINT_1",
															LeapMotionHandTranscoder::GetTypeString(),
															"position of the finger tips",
															&VistaLeapMotionMeasures::HandMeasure::m_a5oFingers,
															&VistaLeapMotionMeasures::Finger::m_v3ProximalBoneStartPoint ),
		new LeapMotionHandArraySubValuesIndexedGet< VistaLeapMotionMeasures::Finger, 5, VistaVector3D >(
															"FINGER_JOINT_2",
															LeapMotionHandTranscoder::GetTypeString(),
															"position of the finger tips",
															&VistaLeapMotionMeasures::HandMeasure::m_a5oFingers,
															&VistaLeapMotionMeasures::Finger::m_v3IntermediateBoneStartPoint ),
		new LeapMotionHandArraySubValuesIndexedGet< VistaLeapMotionMeasures::Finger, 5, VistaVector3D >(
															"FINGER_JOINT_3",
															LeapMotionHandTranscoder::GetTypeString(),
															"position of the finger tips",
															&VistaLeapMotionMeasures::HandMeasure::m_a5oFingers,
															&VistaLeapMotionMeasures::Finger::m_v3DistalBoneStartPoint ),
		new LeapMotionHandArraySubValuesIndexedGet< VistaLeapMotionMeasures::Finger, 5, VistaVector3D >(
															"FINGER_TIP",
															LeapMotionHandTranscoder::GetTypeString(),
															"position of the finger tips",
															&VistaLeapMotionMeasures::HandMeasure::m_a5oFingers,
															&VistaLeapMotionMeasures::Finger::m_v3TipPosition ),
		new LeapMotionHandArraySubValuesIndexedGet< VistaLeapMotionMeasures::Finger, 5, VistaVector3D >(
															"FINGER_SMOOTHED_TIP",
															LeapMotionHandTranscoder::GetTypeString(),
															"smoothed position of the finger tips",
															&VistaLeapMotionMeasures::HandMeasure::m_a5oFingers,
															&VistaLeapMotionMeasures::Finger::m_v3StabilizedTipPosition ),
		new LeapMotionHandArraySubValuesIndexedGet< VistaLeapMotionMeasures::Finger, 5, VistaVector3D >(
															"FINGER_DIRECTION",
															LeapMotionHandTranscoder::GetTypeString(),
															"directions of the finger tips",
															&VistaLeapMotionMeasures::HandMeasure::m_a5oFingers,
															&VistaLeapMotionMeasures::Finger::m_v3Direction ),
		new LeapMotionHandArraySubValuesIndexedGet< VistaLeapMotionMeasures::Finger, 5, VistaVector3D >(
															"FINGER_VELOCITY",
															LeapMotionHandTranscoder::GetTypeString(),
															"velocities of the finger tips",
															&VistaLeapMotionMeasures::HandMeasure::m_a5oFingers,
															&VistaLeapMotionMeasures::Finger::m_v3TipVelocity ),
		new LeapMotionHandArraySubValuesIndexedGet< VistaLeapMotionMeasures::Finger, 5, float >(
															"FINGER_LENGTH",
															LeapMotionHandTranscoder::GetTypeString(),
															"lengths of the fingers",
															&VistaLeapMotionMeasures::HandMeasure::m_a5oFingers,
															&VistaLeapMotionMeasures::Finger::m_nLength ),
		new LeapMotionHandArraySubValuesIndexedGet< VistaLeapMotionMeasures::Finger, 5, float >(
															"FINGER_WIDTH",
															LeapMotionHandTranscoder::GetTypeString(),
															"widths of the fingers",
															&VistaLeapMotionMeasures::HandMeasure::m_a5oFingers,
															&VistaLeapMotionMeasures::Finger::m_nWidth ),
		new LeapMotionHandArraySubValuesIndexedGet< VistaLeapMotionMeasures::Finger, 5, int >(
															"FINGER_ID",
															LeapMotionHandTranscoder::GetTypeString(),
															"leap ids of the fingers",
															&VistaLeapMotionMeasures::HandMeasure::m_a5oFingers,
															&VistaLeapMotionMeasures::Finger::m_nId ),
		new LeapMotionHandArraySubValuesIndexedGet< VistaLeapMotionMeasures::Finger, 5, float >(
															"FINGER_TOUCHDISTANCE",
															LeapMotionHandTranscoder::GetTypeString(),
															"distance of finger tips from virtual leap touch plane",
															&VistaLeapMotionMeasures::HandMeasure::m_a5oFingers,
															&VistaLeapMotionMeasures::Finger::m_nTouchDistance ),
		new LeapMotionHandArraySubValuesIndexedGet< VistaLeapMotionMeasures::Finger, 5, bool, int >(
															"FINGER_EXTENDED",
															LeapMotionHandTranscoder::GetTypeString(),
															"state of extension of the fingers",
															&VistaLeapMotionMeasures::HandMeasure::m_a5oFingers,
															&VistaLeapMotionMeasures::Finger::m_bExtended ),
		new LeapMotionHandArraySubValuesIndexedGet< VistaLeapMotionMeasures::Finger, 5, int >(
															"FINGER_TYPE",
															LeapMotionHandTranscoder::GetTypeString(),
															"type of finger (0: thumb, ..., 4: pinky finger",
															&VistaLeapMotionMeasures::HandMeasure::m_a5oFingers,
															&VistaLeapMotionMeasures::Finger::m_nFingerType ),

		new LeapMotionHandArraySubValuesVectorGet< VistaLeapMotionMeasures::Tool, 2, bool, int >(
															"TOOLS_VISIBLE", 
															LeapMotionHandTranscoder::GetTypeString(),
															"position of the tool tips",
															&VistaLeapMotionMeasures::HandMeasure::m_a2oTools,
															&VistaLeapMotionMeasures::Tool::m_bVisible ),
		new LeapMotionHandArraySubValuesVectorGet< VistaLeapMotionMeasures::Tool, 2, VistaVector3D >(
															"TOOLS_TIP",
															LeapMotionHandTranscoder::GetTypeString(),
															"position of the tool tips",
															&VistaLeapMotionMeasures::HandMeasure::m_a2oTools,
															&VistaLeapMotionMeasures::Tool::m_v3TipPosition ),
		new LeapMotionHandArraySubValuesVectorGet< VistaLeapMotionMeasures::Tool, 2, VistaVector3D >(
															"TOOLS_SMOOTHED_TIP",
															LeapMotionHandTranscoder::GetTypeString(),
															"smoothed position of the tool tips",
															&VistaLeapMotionMeasures::HandMeasure::m_a2oTools,
															&VistaLeapMotionMeasures::Tool::m_v3StabilizedTipPosition ),
		new LeapMotionHandArraySubValuesVectorGet< VistaLeapMotionMeasures::Tool, 2, VistaVector3D >(
															"TOOLS_DIRECTION",
															LeapMotionHandTranscoder::GetTypeString(),
															"directions of the tool tips",
															&VistaLeapMotionMeasures::HandMeasure::m_a2oTools,
															&VistaLeapMotionMeasures::Tool::m_v3Direction ),
		new LeapMotionHandArraySubValuesVectorGet< VistaLeapMotionMeasures::Tool, 2, VistaVector3D >(
															"TOOLS_VELOCITY",
															LeapMotionHandTranscoder::GetTypeString(),
															"velocities of the tool tips",
															&VistaLeapMotionMeasures::HandMeasure::m_a2oTools,
															&VistaLeapMotionMeasures::Tool::m_v3TipVelocity ),
		new LeapMotionHandArraySubValuesVectorGet< VistaLeapMotionMeasures::Tool, 2, float >(
															"TOOLS_LENGTH",
															LeapMotionHandTranscoder::GetTypeString(),
															"lengths of the tools",
															&VistaLeapMotionMeasures::HandMeasure::m_a2oTools,
															&VistaLeapMotionMeasures::Tool::m_nLength ),
		new LeapMotionHandArraySubValuesVectorGet< VistaLeapMotionMeasures::Tool, 2, float >(
															"TOOLS_WIDTH",
															LeapMotionHandTranscoder::GetTypeString(),
															"widths of the tools",
															&VistaLeapMotionMeasures::HandMeasure::m_a2oTools,
															&VistaLeapMotionMeasures::Tool::m_nWidth ),
		new LeapMotionHandArraySubValuesVectorGet< VistaLeapMotionMeasures::Tool, 2, int >(
															"TOOLS_ID",
															LeapMotionHandTranscoder::GetTypeString(),
															"leap ids of the tools",
															&VistaLeapMotionMeasures::HandMeasure::m_a2oTools,
															&VistaLeapMotionMeasures::Tool::m_nId ),
		new LeapMotionHandArraySubValuesVectorGet< VistaLeapMotionMeasures::Tool, 2, float >(
															"TOOLS_TOUCHDISTANCE",
															LeapMotionHandTranscoder::GetTypeString(),
															"distance of tool tips from virtual leap touch plane",
															&VistaLeapMotionMeasures::HandMeasure::m_a2oTools,
															&VistaLeapMotionMeasures::Tool::m_nTouchDistance ),

		new LeapMotionHandArraySubValuesIndexedGet< VistaLeapMotionMeasures::Tool, 2, bool, int >(
															"TOOLS_VISIBLE", 
															LeapMotionHandTranscoder::GetTypeString(),
															"position of the tool tips",
															&VistaLeapMotionMeasures::HandMeasure::m_a2oTools,
															&VistaLeapMotionMeasures::Tool::m_bVisible ),
		new LeapMotionHandArraySubValuesIndexedGet< VistaLeapMotionMeasures::Tool, 2, VistaVector3D >(
															"TOOLS_TIP",
															LeapMotionHandTranscoder::GetTypeString(),
															"position of the tool tips",
															&VistaLeapMotionMeasures::HandMeasure::m_a2oTools,
															&VistaLeapMotionMeasures::Tool::m_v3TipPosition ),
		new LeapMotionHandArraySubValuesIndexedGet< VistaLeapMotionMeasures::Tool, 2, VistaVector3D >(
															"TOOLS_SMOOTHED_TIP",
															LeapMotionHandTranscoder::GetTypeString(),
															"smoothed position of the tool tips",
															&VistaLeapMotionMeasures::HandMeasure::m_a2oTools,
															&VistaLeapMotionMeasures::Tool::m_v3StabilizedTipPosition ),
		new LeapMotionHandArraySubValuesIndexedGet< VistaLeapMotionMeasures::Tool, 2, VistaVector3D >(
															"TOOLS_DIRECTION",
															LeapMotionHandTranscoder::GetTypeString(),
															"directions of the tool tips",
															&VistaLeapMotionMeasures::HandMeasure::m_a2oTools,
															&VistaLeapMotionMeasures::Tool::m_v3Direction ),
		new LeapMotionHandArraySubValuesIndexedGet< VistaLeapMotionMeasures::Tool, 2, VistaVector3D >(
															"TOOLS_VELOCITY",
															LeapMotionHandTranscoder::GetTypeString(),
															"velocities of the tool tips",
															&VistaLeapMotionMeasures::HandMeasure::m_a2oTools,
															&VistaLeapMotionMeasures::Tool::m_v3TipVelocity ),
		new LeapMotionHandArraySubValuesIndexedGet< VistaLeapMotionMeasures::Tool, 2, float >(
															"TOOLS_LENGTH",
															LeapMotionHandTranscoder::GetTypeString(),
															"lengths of the tools",
															&VistaLeapMotionMeasures::HandMeasure::m_a2oTools,
															&VistaLeapMotionMeasures::Tool::m_nLength ),
		new LeapMotionHandArraySubValuesIndexedGet< VistaLeapMotionMeasures::Tool, 2, float >(
															"TOOLS_WIDTH",
															LeapMotionHandTranscoder::GetTypeString(),
															"widths of the tools",
															&VistaLeapMotionMeasures::HandMeasure::m_a2oTools,
															&VistaLeapMotionMeasures::Tool::m_nWidth ),
		new LeapMotionHandArraySubValuesIndexedGet< VistaLeapMotionMeasures::Tool, 2, int >(
															"TOOLS_ID",
															LeapMotionHandTranscoder::GetTypeString(),
															"leap ids of the tools",
															&VistaLeapMotionMeasures::HandMeasure::m_a2oTools,
															&VistaLeapMotionMeasures::Tool::m_nId ),
		new LeapMotionHandArraySubValuesIndexedGet< VistaLeapMotionMeasures::Tool, 2, float >(
															"TOOLS_TOUCHDISTANCE",
															LeapMotionHandTranscoder::GetTypeString(),
															"distance of tool tips from virtual leap touch plane",
															&VistaLeapMotionMeasures::HandMeasure::m_a2oTools,
															&VistaLeapMotionMeasures::Tool::m_nTouchDistance ),

		NULL
	};

	class LeapMotionHandGestureTypeStringGet : public IVistaMeasureTranscode::TTranscodeValueGet< std::string >
	{
	public:
		LeapMotionHandGestureTypeStringGet()
		: IVistaMeasureTranscode::TTranscodeValueGet< std::string >( "TYPE_STRING", "LeapMotionGestureTranscoder", "String describing the gesture type" )
		{
		}

		std::string GetValue( const VistaSensorMeasure* pMeasure ) const
		{
			std::string oValue;
			GetValue( pMeasure, oValue );
			return oValue;
		};

		bool GetValue( const VistaSensorMeasure* pMeasure, std::string& oValue ) const
		{
			if( pMeasure == NULL )
				return false;

			const VistaLeapMotionMeasures::GestureMeasure* pRealMeasure = pMeasure->getRead< VistaLeapMotionMeasures::GestureMeasure >();
			switch( pRealMeasure->m_eType )
			{
				case VistaLeapMotionMeasures::GestureMeasure::TP_CIRCLE:
					oValue = "CIRCLE";
					break;
				case VistaLeapMotionMeasures::GestureMeasure::TP_KEY_TAP:
					oValue = "KEY_TAP";
					break;
				case VistaLeapMotionMeasures::GestureMeasure::TP_SCREEN_TAP:
					oValue = "SCREEN_TAP";
					break;
				case VistaLeapMotionMeasures::GestureMeasure::TP_SWIPE:
					oValue = "SWIPE";
					break;
				default:
					VISTA_THROW( "Invalid gesture type", -1 );
			}
			return true;
		}
	};
	IVistaPropertyGetFunctor *SaGestureGetter[] =
	{
		new IVistaMeasureTranscode::TTranscodeMemberGet< VistaLeapMotionMeasures::GestureMeasure, int >( "ID", 
															LeapMotionGestureTranscoder::GetTypeString(),
															"leap id of the gesture", 
															&VistaLeapMotionMeasures::GestureMeasure::m_nId ),
		new IVistaMeasureTranscode::TTranscodeMemberGet< VistaLeapMotionMeasures::GestureMeasure, VistaLeapMotionMeasures::GestureMeasure::Type, int >( "TYPE", 
															LeapMotionGestureTranscoder::GetTypeString(),
															"type of the gesture ( 0 = SWIPE, SCREEN_TAP, KEY_TAP, CIRCLE = 3 )", 
															&VistaLeapMotionMeasures::GestureMeasure::m_eType ),
		new IVistaMeasureTranscode::TTranscodeMemberGet< VistaLeapMotionMeasures::GestureMeasure, VistaType::microtime >( "DURATION", 
															LeapMotionGestureTranscoder::GetTypeString(),
															"duration of the gesture in seconds", 
															&VistaLeapMotionMeasures::GestureMeasure::m_nDuration ),
		new LeapMotionHandGestureTypeStringGet(),
		new IVistaMeasureTranscode::TTranscodeMemberGet< VistaLeapMotionMeasures::GestureMeasure, float >( "LENGTH", 
															LeapMotionGestureTranscoder::GetTypeString(),
															"length of the gesture (radius or swipe length)", 
															&VistaLeapMotionMeasures::GestureMeasure::m_nLength ),
		new IVistaMeasureTranscode::TTranscodeMemberGet< VistaLeapMotionMeasures::GestureMeasure, int >( "POINTABLE_ID", 
															LeapMotionGestureTranscoder::GetTypeString(),
															"leap id of the pointable performing this gesture", 
															&VistaLeapMotionMeasures::GestureMeasure::m_nPerformingPointableId ),
		new IVistaMeasureTranscode::TTranscodeMemberGet< VistaLeapMotionMeasures::GestureMeasure, VistaVector3D >( "POSITION", 
															LeapMotionGestureTranscoder::GetTypeString(),
															"position of the gesture", 
															&VistaLeapMotionMeasures::GestureMeasure::m_v3Position ),
		new IVistaMeasureTranscode::TTranscodeMemberGet< VistaLeapMotionMeasures::GestureMeasure, VistaVector3D >( "DIRECTION", 
															LeapMotionGestureTranscoder::GetTypeString(),
															"direction of the gesture", 
															&VistaLeapMotionMeasures::GestureMeasure::m_v3Direction	),
	};



	class VistaLeapMotionDriverTranscoderFactoryFactory : public IVistaTranscoderFactoryFactory
	{
	public:
		typedef std::map<std::string,ICreateTranscoder*> CreatorsMap;

		VistaLeapMotionDriverTranscoderFactoryFactory()
		{
			CreateCreators( m_mapCreators );
		}

		~VistaLeapMotionDriverTranscoderFactoryFactory()
		{
			CleanupCreators( m_mapCreators );
		}		
		

		static void CreateCreators( CreatorsMap& m_mapCreators )
		{
			m_mapCreators[ "HAND" ]  = new TCreateTranscoder< LeapMotionHandTranscoder >();
			m_mapCreators[ "FINGERS" ]  = new TCreateTranscoder< LeapMotionFingersTranscoder >();
			m_mapCreators[ "TOOLS" ]  = new TCreateTranscoder< LeapMotionToolsTranscoder >();
			m_mapCreators[ "GESTURE" ]  = new TCreateTranscoder< LeapMotionGestureTranscoder >();
			m_mapCreators[ "IMAGE" ]  = new TCreateTranscoder< LeapMotionImageTranscoder >();
		}

		static void CleanupCreators( CreatorsMap& m_mapCreators )
		{
			for( CreatorsMap::iterator it = m_mapCreators.begin(); it != m_mapCreators.end(); ++it )
				delete (*it).second;

			m_mapCreators.clear();
		}

		virtual IVistaMeasureTranscoderFactory *CreateFactoryForType( const std::string& strTypeName )
		{
			CreatorsMap::const_iterator it = m_mapCreators.find( strTypeName );
			if( it == m_mapCreators.end() )
				return NULL;
			return (*it).second->Create();
		}


		virtual void DestroyTranscoderFactory( IVistaMeasureTranscoderFactory *fac )
		{
			delete fac;
		}

		static void OnUnload()
		{
			CreatorsMap mapCreators;
			CreateCreators( mapCreators );
			for( CreatorsMap::iterator itCreator = mapCreators.begin(); itCreator != mapCreators.end(); ++itCreator )
				(*itCreator).second->OnUnload();
			CleanupCreators( mapCreators );
		}
	private:
		CreatorsMap m_mapCreators;

	};

//	VistaVRPNDriverTranscoderFactoryFactory* SpFactory = NULL;
} // namespace


/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

#if VISTALEAPMOTIONTRANSCODER_EXPORTS
	DEFTRANSCODERPLUG_FUNC_EXPORTS( VistaLeapMotionDriverTranscoderFactoryFactory )
#else
	DEFTRANSCODERPLUG_FUNC_IMPORTS( VistaLeapMotionDriverTranscoderFactoryFactory )
#endif

DEFTRANSCODERPLUG_CLEANUP;
IMPTRANSCODERPLUG_CLEANUP( VistaLeapMotionDriverTranscoderFactoryFactory )

