/*============================================================================*/
/*                              ViSTA VR toolkit                              */
/*               CopyoRight (c) 1997-2012 RWTH Aachen University               */
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


#ifndef _VDFNBINARYOPS_H
#define _VDFNBINARYOPS_H

#include <VistaBase/VistaVectorMath.h>
#include <vector>

/**
 * These template provide some 
 */
namespace VdfnBinaryOps
{
	template<class TLeft, class TRight, class TRes>
	class BinOp
	{
	public:
		virtual ~BinOp() {}
		virtual TRes operator()( const TLeft& oLeft, const TRight& oRight ) const = 0;
	};

	template<class TLeft, class TRight, class TRes>
	class AddOp : public BinOp<TLeft, TRight, TRes>
	{
	public:
		virtual TRes operator()( const TLeft& oLeft, const TRight& oRight ) const
		{
			return TRes( oLeft + oRight );
		}
	};

	template<class TLeft, class TRight, class TRes>
	class SubOp : public BinOp<TLeft, TRight, TRes>
	{
	public:
		virtual TRes operator()( const TLeft& oLeft, const TRight& oRight ) const
		{
			return TRes( oLeft - oRight );
		}
	};

#ifdef WIN32
#pragma warning(push)
#pragma warning(disable: 4244)
#endif
	// the MultOp can contain some type conversions that may result in warnings
	// we know and accept that, so we disable the warning locally
	template<class TLeft, class TRight, class TRes>
	class MultOp : public BinOp<TLeft, TRight, TRes>
	{
		virtual TRes operator()( const TLeft& oLeft, const TRight& oRight ) const
		{
			return TRes( oLeft * oRight );
		}
	};
#ifdef WIN32
#pragma warning(pop)
#endif

	template<class TLeft, class TRight, class TRes>
	class DivOp : public BinOp<TLeft, TRight, TRes>
	{
		virtual TRes operator()( const TLeft& oLeft, const TRight& oRight ) const
		{
			return TRes( oLeft / oRight );
		}
	};

	template<class TLeft, class TRight, class TRes>
	class EqualsOp : public BinOp<TLeft, TRight, TRes>
	{
		virtual TRes operator()( const TLeft& oLeft, const TRight& oRight ) const
		{
			return TRes( oLeft == oRight );
		}
	};

	template<class TLeft, class TRight, class TRes>
	class AndOp : public BinOp<TLeft, TRight, TRes>
	{
		virtual TRes operator()( const TLeft& oLeft, const TRight& oRight ) const
		{
			return TRes( oLeft && oRight );
		}
	};

	template<class TLeft, class TRight, class TRes>
	class OrOp : public BinOp<TLeft, TRight, TRes>
	{
		virtual TRes operator()( const TLeft& oLeft, const TRight& oRight ) const
		{
			return TRes( oLeft || oRight );
		}
	};

	template<class T>
	class MaxOp : public BinOp<T, T, T>
	{
		virtual T operator()( const T& oLeft, const T& oRight ) const
		{
			return ( oLeft > oRight ? oLeft : oRight );
		}
	};

	template<class T>
	class MinOp : public BinOp<T, T, T>
	{
		virtual T operator()( const T& oLeft, const T& oRight ) const
		{
			return ( oLeft < oRight ? oLeft : oRight );
		}
	};

	template<class T>
	class LessOp : public BinOp<T, T, bool>
	{
		virtual bool operator()( const T& oLeft, const T& oRight ) const
		{
			return ( oLeft < oRight );
		}
	};

	template<class T>
	class LessEqualOp : public BinOp<T, T, bool>
	{
		virtual bool operator()( const T& oLeft, const T& oRight ) const
		{
			return ( oLeft <= oRight );
		}
	};

	template<class TLeft, class TRight, class TRes>
	class TransformOp : public BinOp<TLeft, TRight, TRes>
	{
	public:
		virtual TRes operator()( const TLeft& oLeft, const TRight& oRight ) const
		{
			// works for QUATxQUAT, MATxV3D, MATxQUAT, MATxMAT
			return TRes( oLeft * oRight );
		}
	};
	// specializations for QUATxV3D, V3DxV3D, QUATxMAT, V3DxMAT
	template<>
	class TransformOp<VistaQuaternion,VistaVector3D,VistaVector3D>
			: public BinOp<VistaQuaternion,VistaVector3D,VistaVector3D>
	{
	public:
		virtual VistaVector3D operator()(const VistaQuaternion& oLeft, const VistaVector3D& oRight) const
		{
			return ( oLeft.Rotate( oRight ) );
		}
	};
	template<>
	class TransformOp<VistaVector3D,VistaVector3D,VistaVector3D>
			: public BinOp<VistaVector3D,VistaVector3D,VistaVector3D>
	{
	public:
		virtual VistaVector3D operator()(const VistaVector3D& oLeft, const VistaVector3D& oRight) const
		{
			return ( oLeft + oRight );
		}
	};
	template<>
	class TransformOp<VistaVector3D,VistaTransformMatrix,VistaTransformMatrix>
			: public BinOp<VistaVector3D,VistaTransformMatrix,VistaTransformMatrix>
	{
	public:
		virtual VistaTransformMatrix operator()(const VistaVector3D& oLeft, const VistaTransformMatrix& oRight) const
		{
			return ( VistaTransformMatrix( oLeft ) * oRight );
		}
	};
	template<>
	class TransformOp<VistaQuaternion,VistaTransformMatrix,VistaTransformMatrix>
		: public BinOp<VistaQuaternion,VistaTransformMatrix,VistaTransformMatrix>
	{
	public:
		virtual VistaTransformMatrix operator()(const VistaQuaternion& oLeft, const VistaTransformMatrix& oRight) const
		{
			return ( VistaTransformMatrix( oLeft ) * oRight );
		}
	};
	// specialization to work on vectors of targets
	template<class TLeft, class TRight, class TRes>
	class TransformOp<TLeft, std::vector<TRight>, std::vector<TRes> >
		: public BinOp<TLeft, std::vector<TRight>, std::vector<TRes> >
	{
	public:
		std::vector<TRes> operator()( const TLeft& oLeft, const std::vector<TRight>& oRight ) const
		{
			typename std::vector<TRes> vecResult;
			vecResult.reserve( oRight.size() );
			for( typename std::vector<TRight>::const_iterator itRight = oRight.begin();
					itRight != oRight.end(); ++itRight )
			{
				vecResult.push_back( m_oOp( oLeft, (*itRight) ) );
			}
			return vecResult;
		}
	private:
		TransformOp<TLeft, TRight, TRes> m_oOp;
	};

}

#endif //_VDFNBINARYOPS_H
