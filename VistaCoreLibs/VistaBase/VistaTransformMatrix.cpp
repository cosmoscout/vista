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
/* Matrix decomposition code taken from Graphics Gems IV,                     */
/* http://tog.acm.org/resources/GraphicsGems/gemsiv/polar_decomp/             */
/* Copyright (C) 1993 Ken Shoemake <shoemake@graphics.cis.upenn.edu>          */
/*                                                                            */
/* original EULA:                                                             */
/* The Graphics Gems code is copyright-protected. In other words, you cannot  */
/* claim the text of the code as your own and resell it. Using the code is    */
/* permitted in any program, product, or library, non-commercial or           */
/* commercial.                                                                */
/* Giving credit is not required, though is a nice gesture. The code comes    */
/* as-is, and if there are any flaws or problems with any Gems code, nobody   */
/* involved with Gems - authors, editors, publishers, or webmasters - are to  */
/* be held responsible. Basically, don't be a jerk, and remember that         */
/* anything free comes with no guarantee.                                     */
/*============================================================================*/


#include "VistaTransformMatrix.h" 

#include <cmath>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

namespace
{
	/**** Decompose.h - Basic declarations ****/
	struct Quat {float x, y, z, w; float& operator[](const size_t &i){ return((float*)(&x))[i];}}; /* Quaternion */
	//enum QuatPart {X, Y, Z, W}; // already defined in VistaMath
	typedef Quat HVect; /* Homogeneous 3D vector */
	typedef float HMatrix[4][4]; /* Right-handed, for column vectors */
	struct AffineParts {
		HVect t;	/* Translation components */
		Quat  q;	/* Essential rotation	  */
		Quat  u;	/* Stretch rotation	  */
		HVect k;	/* Stretch factors	  */
		float f;	/* Sign of determinant	  */
	};
	float polar_decomp(HMatrix M, HMatrix Q, HMatrix S);
	HVect spect_decomp(HMatrix S, HMatrix U);
	Quat snuggle(Quat q, HVect *k);
	void decomp_affine(HMatrix A, AffineParts *parts);
		
	/**** Decompose.c ****/
	/* Ken Shoemake, 1993 */

	/******* Matrix Preliminaries *******/

	/** Fill out 3x3 matrix to 4x4 **/
	#define mat_pad(A) (A[Vista::W][Vista::X]=A[Vista::X][Vista::W]=A[Vista::W][Vista::Y]=A[Vista::Y][Vista::W]=A[Vista::W][Vista::Z]=A[Vista::Z][Vista::W]=0,A[Vista::W][Vista::W]=1)

	/** Copy nxn matrix A to C using "gets" for assignment **/
	#define mat_copy(C,gets,A,n) {int i,j; for(i=0;i<n;i++) for(j=0;j<n;j++)\
		C[i][j] gets (A[i][j]);}

	/** Copy transpose of nxn matrix A to C using "gets" for assignment **/
	#define mat_tpose(AT,gets,A,n) {int i,j; for(i=0;i<n;i++) for(j=0;j<n;j++)\
		AT[i][j] gets (A[j][i]);}

	/** Assign nxn matrix C the element-wise combination of A and B using "op" **/
	#define mat_binop(C,gets,A,op,B,n) {int i,j; for(i=0;i<n;i++) for(j=0;j<n;j++)\
		C[i][j] gets (A[i][j]) op (B[i][j]);}

	/** Multiply the upper left 3x3 parts of A and B to get AB **/
	void mat_mult(HMatrix A, HMatrix B, HMatrix AB)
	{
		int i, j;
		for (i=0; i<3; i++) for (j=0; j<3; j++)
		AB[i][j] = A[i][0]*B[0][j] + A[i][1]*B[1][j] + A[i][2]*B[2][j];
	}

	/** Return dot product of length 3 vectors va and vb **/
	float vdot(float *va, float *vb)
	{
		return (va[0]*vb[0] + va[1]*vb[1] + va[2]*vb[2]);
	}

	/** Set v to cross product of length 3 vectors va and vb **/
	void vcross(float *va, float *vb, float *v)
	{
		v[0] = va[1]*vb[2] - va[2]*vb[1];
		v[1] = va[2]*vb[0] - va[0]*vb[2];
		v[2] = va[0]*vb[1] - va[1]*vb[0];
	}

	/** Set MadjT to transpose of inverse of M times determinant of M **/
	void adjoint_transpose(HMatrix M, HMatrix MadjT)
	{
		vcross(M[1], M[2], MadjT[0]);
		vcross(M[2], M[0], MadjT[1]);
		vcross(M[0], M[1], MadjT[2]);
	}

	/******* Quaternion Preliminaries *******/

	/* Construct a (possibly non-unit) quaternion from real components. */
	Quat Qt_(float x, float y, float z, float w)
	{
		Quat qq;
		qq.x = x; qq.y = y; qq.z = z; qq.w = w;
		return (qq);
	}

	/* Return conjugate of quaternion. */
	Quat Qt_Conj(Quat q)
	{
		Quat qq;
		qq.x = -q.x; qq.y = -q.y; qq.z = -q.z; qq.w = q.w;
		return (qq);
	}

	/* Return quaternion product qL * qR.  Note: order is important!
	 * To combine rotations, use the product Mul(qSecond, qFirst),
	 * which gives the effect of rotating by qFirst then qSecond. */
	Quat Qt_Mul(Quat qL, Quat qR)
	{
		Quat qq;
		qq.w = qL.w*qR.w - qL.x*qR.x - qL.y*qR.y - qL.z*qR.z;
		qq.x = qL.w*qR.x + qL.x*qR.w + qL.y*qR.z - qL.z*qR.y;
		qq.y = qL.w*qR.y + qL.y*qR.w + qL.z*qR.x - qL.x*qR.z;
		qq.z = qL.w*qR.z + qL.z*qR.w + qL.x*qR.y - qL.y*qR.x;
		return (qq);
	}

	/* Return product of quaternion q by scalar w. */
	Quat Qt_Scale(Quat q, float w)
	{
		Quat qq;
		qq.w = q.w*w; qq.x = q.x*w; qq.y = q.y*w; qq.z = q.z*w;
		return (qq);
	}

	/* Construct a unit quaternion from rotation matrix.  Assumes matrix is
	 * used to multiply column vector on the left: vnew = mat vold.	 Works
	 * correctly for right-handed coordinate system and right-handed rotations.
	 * Translation and perspective components ignored. */
	Quat Qt_FromMatrix(HMatrix mat)
	{
		/* This algorithm avoids near-zero divides by looking for a large component
		* - first w, then x, y, or z.  When the trace is greater than zero,
		* |w| is greater than 1/2, which is as small as a largest component can be.
		* Otherwise, the largest diagonal entry corresponds to the largest of |x|,
		* |y|, or |z|, one of which must be larger than |w|, and at least 1/2. */
		Quat qu;
		double tr, s;

		tr = mat[Vista::X][Vista::X] + mat[Vista::Y][Vista::Y]+ mat[Vista::Z][Vista::Z];
		if (tr >= 0.0) {
			s = std::sqrt(tr + mat[Vista::W][Vista::W]);
			qu.w = (float)s*0.5f ;
			s = 0.5 / s;
			qu.x = (float)( (mat[Vista::Z][Vista::Y] - mat[Vista::Y][Vista::Z]) * s );
			qu.y = (float)( (mat[Vista::X][Vista::Z] - mat[Vista::Z][Vista::X]) * s );
			qu.z = (float)( (mat[Vista::Y][Vista::X] - mat[Vista::X][Vista::Y]) * s );
		} else {
			int h = Vista::X;
			if (mat[Vista::Y][Vista::Y] > mat[Vista::X][Vista::X]) h = Vista::Y;
			if (mat[Vista::Z][Vista::Z] > mat[h][h]) h = Vista::Z;
			switch (h) {
	#define caseMacro(i,j,k,I,J,K) \
			case I:\
			s = std::sqrt( (mat[I][I] - (mat[J][J]+mat[K][K])) + mat[Vista::W][Vista::W] );\
			qu.i = (float)s*0.5f ;\
			s = 0.5 / s;\
			qu.j = (float)( (mat[I][J] + mat[J][I]) * s );\
			qu.k = (float)( (mat[K][I] + mat[I][K]) * s );\
			qu.w = (float)( (mat[K][J] - mat[J][K]) * s );\
			break
				caseMacro(x,y,z,Vista::X,Vista::Y,Vista::Z);
				caseMacro(y,z,x,Vista::Y,Vista::Z,Vista::X);
				caseMacro(z,x,y,Vista::Z,Vista::X,Vista::Y);
			default:
				qu.x = 0; 
				qu.y = 0;
				qu.z = 0; 
				qu.w = 1;
				break;
			}
		}
		if (mat[Vista::W][Vista::W] != 1.0) 
			qu = Qt_Scale(qu, 1/std::sqrt(mat[Vista::W][Vista::W]));
		return (qu);
	}
	/******* Decomp Auxiliaries *******/

	static HMatrix mat_id = {{1,0,0,0},{0,1,0,0},{0,0,1,0},{0,0,0,1}};

	/** Compute either the 1 or infinity norm of M, depending on tpose **/
	float mat_norm(HMatrix M, int tpose)
	{
		int i;
		float sum, max;
		max = 0.0;
		for (i=0; i<3; i++) {
			if (tpose) sum = std::abs(M[0][i])+std::abs(M[1][i])+std::abs(M[2][i]);
			else	   sum = std::abs(M[i][0])+std::abs(M[i][1])+std::abs(M[i][2]);
			if (max<sum) max = sum;
		}
		return max;
	}

	float norm_inf(HMatrix M) {return mat_norm(M, 0);}
	float norm_one(HMatrix M) {return mat_norm(M, 1);}

	/** Return index of column of M containing maximum abs entry, or -1 if M=0 **/
	int find_max_col(HMatrix M)
	{
		float abs, max;
		int i, j, col;
		max = 0.0; col = -1;
		for (i=0; i<3; i++) for (j=0; j<3; j++) {
			abs = M[i][j]; if (abs<0.0) abs = -abs;
			if (abs>max) {max = abs; col = j;}
		}
		return col;
	}

	/** Setup u for Household reflection to zero all v components but first **/
	void make_reflector(float *v, float *u)
	{
		float s = std::sqrt(vdot(v, v));
		u[0] = v[0]; u[1] = v[1];
		u[2] = v[2] + ((v[2]<0.0) ? -s : s);
		s = std::sqrt(2.0f/vdot(u, u));
		u[0] = u[0]*s; u[1] = u[1]*s; u[2] = u[2]*s;
	}

	/** Apply Householder reflection represented by u to column vectors of M **/
	void reflect_cols(HMatrix M, float *u)
	{
		int i, j;
		for (i=0; i<3; i++) {
			float s = u[0]*M[0][i] + u[1]*M[1][i] + u[2]*M[2][i];
			for (j=0; j<3; j++)
				M[j][i] -= u[j]*s;
		}
	}
	/** Apply Householder reflection represented by u to row vectors of M **/
	void reflect_rows(HMatrix M, float *u)
	{
		int i, j;
		for (i=0; i<3; i++) {
			float s = vdot(u, M[i]);
			for (j=0; j<3; j++)
				M[i][j] -= u[j]*s;
		}
	}

	/** Find orthogonal factor Q of rank 1 (or less) M **/
	void do_rank1(HMatrix M, HMatrix Q)
	{
		float v1[3], v2[3], s;
		int col;
		mat_copy(Q,=,mat_id,4);
		/* If rank(M) is 1, we should find a non-zero column in M */
		col = find_max_col(M);
		if (col<0) return; /* Rank is 0 */
		v1[0] = M[0][col]; v1[1] = M[1][col]; v1[2] = M[2][col];
		make_reflector(v1, v1); reflect_cols(M, v1);
		v2[0] = M[2][0]; v2[1] = M[2][1]; v2[2] = M[2][2];
		make_reflector(v2, v2); reflect_rows(M, v2);
		s = M[2][2];
		if (s<0.0) Q[2][2] = -1.0;
		reflect_cols(Q, v1); reflect_rows(Q, v2);
	}

	/** Find orthogonal factor Q of rank 2 (or less) M using adjoint transpose **/
	void do_rank2(HMatrix M, HMatrix MadjT, HMatrix Q)
	{
		float v1[3], v2[3];
		float w, x, y, z, c, s, d;
		int col;
		/* If rank(M) is 2, we should find a non-zero column in MadjT */
		col = find_max_col(MadjT);
		if (col<0) {do_rank1(M, Q); return;} /* Rank<2 */
		v1[0] = MadjT[0][col]; v1[1] = MadjT[1][col]; v1[2] = MadjT[2][col];
		make_reflector(v1, v1); reflect_cols(M, v1);
		vcross(M[0], M[1], v2);
		make_reflector(v2, v2); reflect_rows(M, v2);
		w = M[0][0]; x = M[0][1]; y = M[1][0]; z = M[1][1];
		if (w*z>x*y) {
			c = z+w; s = y-x; d = std::sqrt(c*c+s*s); c = c/d; s = s/d;
			Q[0][0] = Q[1][1] = c; Q[0][1] = -(Q[1][0] = s);
		} else {
			c = z-w; s = y+x; d = std::sqrt(c*c+s*s); c = c/d; s = s/d;
			Q[0][0] = -(Q[1][1] = c); Q[0][1] = Q[1][0] = s;
		}
		Q[0][2] = Q[2][0] = Q[1][2] = Q[2][1] = 0.0; Q[2][2] = 1.0;
		reflect_cols(Q, v1); reflect_rows(Q, v2);
	}


	/******* Polar Decomposition *******/

	/* Polar Decomposition of 3x3 matrix in 4x4,
	 * M = QS.  See Nicholas Higham and Robert S. Schreiber,
	 * Fast Polar Decomposition of An Arbitrary Matrix,
	 * Technical Report 88-942, October 1988,
	 * Department of Computer Science, Cornell University.
	 */
	float polar_decomp(HMatrix M, HMatrix Q, HMatrix S)
	{
	#define TOL 1.0e-6
		HMatrix Mk, MadjTk, Ek;
		float det, M_one, M_inf, MadjT_one, MadjT_inf, E_one, gamma, g1, g2;
		int i, j;
		mat_tpose(Mk,=,M,3);
		M_one = norm_one(Mk);  M_inf = norm_inf(Mk);
		do {
			adjoint_transpose(Mk, MadjTk);
			det = vdot(Mk[0], MadjTk[0]);
			if (det==0.0) {do_rank2(Mk, MadjTk, Mk); break;}
			MadjT_one = norm_one(MadjTk); MadjT_inf = norm_inf(MadjTk);
			gamma = std::sqrt(std::sqrt((MadjT_one*MadjT_inf)/(M_one*M_inf))/std::abs(det));
			g1 = gamma*0.5f ;
			g2 = 0.5f /(gamma*det);
			mat_copy(Ek,=,Mk,3);
			mat_binop(Mk,=,g1*Mk,+,g2*MadjTk,3);
			mat_copy(Ek,-=,Mk,3);
			E_one = norm_one(Ek);
			M_one = norm_one(Mk);  M_inf = norm_inf(Mk);
		} while (E_one>(M_one*TOL));
		mat_tpose(Q,=,Mk,3); mat_pad(Q);
		mat_mult(Mk, M, S);	 mat_pad(S);
		for (i=0; i<3; i++) for (j=i; j<3; j++)
			S[i][j] = S[j][i] = 0.5f *(S[i][j]+S[j][i]);
		return (det);
	}


	/******* Spectral Decomposition *******/

	/* Compute the spectral decomposition of symmetric positive semi-definite S.
	 * Returns rotation in U and scale factors in result, so that if K is a diagonal
	 * matrix of the scale factors, then S = U K (U transpose). Uses Jacobi method.
	 * See Gene H. Golub and Charles F. Van Loan. Matrix Computations. Hopkins 1983.
	 */
	HVect spect_decomp(HMatrix S, HMatrix U)
	{
		HVect kv;
		double Diag[3],OffD[3]; /* OffD is off-diag (by omitted index) */
		double g,h,fabsh,fabsOffDi,t,theta,c,s,tau,ta,OffDq,a,b;
		static char nxt[] = {Vista::Y,Vista::Z,Vista::X};
		int sweep, i, j;
		mat_copy(U,=,mat_id,4);
		Diag[Vista::X] = S[Vista::X][Vista::X]; Diag[Vista::Y] = S[Vista::Y][Vista::Y]; Diag[Vista::Z] = S[Vista::Z][Vista::Z];
		OffD[Vista::X] = S[Vista::Y][Vista::Z]; OffD[Vista::Y] = S[Vista::Z][Vista::X]; OffD[Vista::Z] = S[Vista::X][Vista::Y];
		for (sweep=20; sweep>0; sweep--) {
			float sm = (float)( std::abs(OffD[Vista::X])+std::abs(OffD[Vista::Y])+std::abs(OffD[Vista::Z]) );
			if (sm==0.0) break;
			for (i=Vista::Z; i>=Vista::X; i--) {
				int p = nxt[i]; int q = nxt[p];
				fabsOffDi = std::abs(OffD[i]);
				g = 100.0*fabsOffDi;
				if (fabsOffDi>0.0) {
					h = Diag[q] - Diag[p];
					fabsh = std::abs(h);
					if (fabsh+g==fabsh) {
						t = OffD[i]/h;
					} else {
						theta = 0.5f *h/OffD[i];
						t = 1.0/(std::abs(theta)+std::sqrt(theta*theta+1.0));
						if (theta<0.0) t = -t;
					}
					c = 1.0/std::sqrt(t*t+1.0); s = t*c;
					tau = s/(c+1.0);
					ta = t*OffD[i]; OffD[i] = 0.0;
					Diag[p] -= ta; Diag[q] += ta;
					OffDq = OffD[q];
					OffD[q] -= s*(OffD[p] + tau*OffD[q]);
					OffD[p] += s*(OffDq   - tau*OffD[p]);
					for (j=Vista::Z; j>=Vista::X; j--) {
						a = U[j][p]; b = U[j][q];
						U[j][p] -= (float)( s*(b + tau*a) );
						U[j][q] += (float)( s*(a - tau*b) );
					}
				}
			}
		}
		kv.x = (float)Diag[Vista::X];
		kv.y = (float)Diag[Vista::Y];
		kv.z = (float)Diag[Vista::Z];
		kv.w = 1.0f;
		return (kv);
	}

	/******* Spectral Axis Adjustment *******/

	/* Given a unit quaternion, q, and a scale vector, k, find a unit quaternion, p,
	 * which permutes the axes and turns freely in the plane of duplicate scale
	 * factors, such that q p has the largest possible w component, i.e. the
	 * smallest possible angle. Permutes k's components to go with q p instead of q.
	 * See Ken Shoemake and Tom Duff. Matrix Animation and Polar Decomposition.
	 * Proceedings of Graphics Interface 1992. Details on p. 262-263.
	 */
	Quat snuggle(Quat q, HVect *k)
	{
	#define SQRTHALF (0.7071067811865475244f)
	#define sgn(n,v)    ((n)?-(v):(v))
	#define swap(a,i,j) {a[3]=a[i]; a[i]=a[j]; a[j]=a[3];}
	#define cycle(a,p)  if (p) {a[3]=a[0]; a[0]=a[1]; a[1]=a[2]; a[2]=a[3];}\
				else   {a[3]=a[2]; a[2]=a[1]; a[1]=a[0]; a[0]=a[3];}
	#define cycle0(a) {a[3]=a[2]; a[2]=a[1]; a[1]=a[0]; a[0]=a[3];}
	#define cycle1(a) {a[3]=a[0]; a[0]=a[1]; a[1]=a[2]; a[2]=a[3];}

		Quat p;
		float ka[4];
		int i, turn = -1;
		ka[Vista::X] = k->x; ka[Vista::Y] = k->y; ka[Vista::Z] = k->z;
		if (ka[Vista::X]==ka[Vista::Y]) {if (ka[Vista::X]==ka[Vista::Z]) turn = Vista::W; else turn = Vista::Z;}
		else {if (ka[Vista::X]==ka[Vista::Z]) turn = Vista::Y; else if (ka[Vista::Y]==ka[Vista::Z]) turn = Vista::X;}
		if (turn>=0) {
			Quat qtoz, qp;
			unsigned neg[3], win;
			double mag[3], t;
			static Quat qxtoz = {0,SQRTHALF,0,SQRTHALF};
			static Quat qytoz = {SQRTHALF,0,0,SQRTHALF};
			static Quat qppmm = { 0.5f , 0.5f ,-0.5f ,-0.5f };
			static Quat qpppp = { 0.5f , 0.5f , 0.5f , 0.5f };
			static Quat qmpmm = {-0.5f , 0.5f ,-0.5f ,-0.5f };
			static Quat qpppm = { 0.5f , 0.5f , 0.5f ,-0.5f };
			static Quat q0001 = { 0.0, 0.0, 0.0, 1.0};
			static Quat q1000 = { 1.0, 0.0, 0.0, 0.0};
			switch (turn) {
		default: return (Qt_Conj(q));
		case Vista::X: q = Qt_Mul(q, qtoz = qxtoz); swap(ka,Vista::X,Vista::Z) break;
		case Vista::Y: q = Qt_Mul(q, qtoz = qytoz); swap(ka,Vista::Y,Vista::Z) break;
		case Vista::Z: qtoz = q0001; break;
			}
			q = Qt_Conj(q);
			mag[0] = (double)q.z*q.z+(double)q.w*q.w-0.5f ;
			mag[1] = (double)q.x*q.z-(double)q.y*q.w;
			mag[2] = (double)q.y*q.z+(double)q.x*q.w;
			for (i=0; i<3; i++)
			{
				neg[i] = (mag[i]<0.0);
				if( neg[i] )
					mag[i] = -mag[i];
			}
			if (mag[0]>mag[1])
			{
				if (mag[0]>mag[2])
					win = 0;
				else
					win = 2;
			}
			else
			{
				if (mag[1]>mag[2])
					win = 1;
				else win = 2;
			}
			switch (win)
			{
				case 0:
				{
					if (neg[0]) 
						p = q1000; 
					else
						p = q0001; 
					break;
				}
				case 1:
				{
					if (neg[1]) 
						p = qppmm; 
					else 
						p = qpppp; 
					cycle0(ka) 
						break;
				}
				case 2:
				default: // this is just so the compiler doesn't complain that 'win' might have different values than 0,1,2 (it doesn't)
				{
					if (neg[2]) 
						p = qmpmm; 
					else 
						p = qpppm; 
					cycle1(ka) 
						break;
				}
			}
			qp = Qt_Mul(q, p);
			t = std::sqrt(mag[win]+0.5f );
			p = Qt_Mul(p, Qt_(0.0f,0.0f,(float)(-qp.z/t),(float)(qp.w/t)));
			p = Qt_Mul(qtoz, Qt_Conj(p));
		} else {
			float qa[4], pa[4];
			unsigned lo, hi, neg[4], par = 0;
			double all, big, two;
			qa[0] = q.x; qa[1] = q.y; qa[2] = q.z; qa[3] = q.w;
			for (i=0; i<4; i++)
			{
				pa[i] = 0.0;
				neg[i] = (qa[i]<0.0);
				if ( neg[i] ) qa[i] = -qa[i];
				par ^= neg[i];
			}
			/* Find two largest components, indices in hi and lo */
			if (qa[0]>qa[1]) lo = 0; else lo = 1;
			if (qa[2]>qa[3]) hi = 2; else hi = 3;
			if (qa[lo]>qa[hi]) {
				if (qa[lo^1]>qa[hi]) {hi = lo; lo ^= 1;}
				else {hi ^= lo; lo ^= hi; hi ^= lo;}
			} else {if (qa[hi^1]>qa[lo]) lo = hi^1;}
			all = (qa[0]+qa[1]+qa[2]+qa[3])*0.5f ;
			two = (qa[hi]+qa[lo])*SQRTHALF;
			big = qa[hi];
			if (all>two) {
				if (all>big) {/*all*/
					{int i; for (i=0; i<4; i++) pa[i] = sgn(neg[i], 0.5f );}
					cycle(ka,par)
				} else {/*big*/ pa[hi] = sgn(neg[hi],1.0f);}
			} else {
				if (two>big) {/*two*/
					pa[hi] = sgn(neg[hi],SQRTHALF); pa[lo] = sgn(neg[lo], SQRTHALF);
					if (lo>hi) {hi ^= lo; lo ^= hi; hi ^= lo;}
					if (hi==Vista::W) {hi = "\001\002\000"[lo]; lo = 3-hi-lo;}
					swap(ka,hi,lo)
				} else {/*big*/ pa[hi] = sgn(neg[hi],1.0f);}
			}
			p.x = -pa[0]; p.y = -pa[1]; p.z = -pa[2]; p.w = pa[3];
		}
		k->x = ka[Vista::X]; k->y = ka[Vista::Y]; k->z = ka[Vista::Z];
		return (p);
	}


	/******* Decompose Affine Matrix *******/

	/* Decompose 4x4 affine matrix A as TFRUK(U transpose), where t contains the
	 * translation components, q contains the rotation R, u contains U, k contains
	 * scale factors, and f contains the sign of the determinant.
	 * Assumes A transforms column vectors in right-handed coordinates.
	 * See Ken Shoemake and Tom Duff. Matrix Animation and Polar Decomposition.
	 * Proceedings of Graphics Interface 1992.
	 */
	void decomp_affine(HMatrix A, AffineParts *parts)
	{
		HMatrix Q, S, U;
		Quat p;
		float det;
		parts->t = Qt_(A[Vista::X][Vista::W], A[Vista::Y][Vista::W], A[Vista::Z][Vista::W], 0);
		det = polar_decomp(A, Q, S);
		if (det<0.0) {
		mat_copy(Q,=,-Q,3);
		parts->f = -1;
		} else parts->f = 1;
		parts->q = Qt_FromMatrix(Q);
		parts->k = spect_decomp(S, U);
		parts->u = Qt_FromMatrix(U);
		p = snuggle(parts->u, &parts->k);
		parts->u = Qt_Mul(parts->u, p);
	}
}

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

float VistaTransformMatrix::GetAdjunct( const int iRow, const int iColumn ) const
{
	const int iRowP1 = ( iRow + 1 ) % 4;
	const int iRowP2 = ( iRow + 2 ) % 4;
	const int iRowP3 = ( iRow + 3 ) % 4;
	const int iColP1 = ( iColumn + 1 ) % 4;
	const int iColP2 = ( iColumn + 2 ) % 4;
	const int iColP3 = ( iColumn + 3 ) % 4;
	// negative sign if odd(row + col)
	if ( (iRow + iColumn) & 1 )
	{
		return ( operator()( iRowP1, iColP3 ) * operator()( iRowP2, iColP2 ) * operator()( iRowP3, iColP1 ) -
				 operator()( iRowP1, iColP2 ) * operator()( iRowP2, iColP1 ) * operator()( iRowP3, iColP3 ) -
				 operator()( iRowP1, iColP1 ) * operator()( iRowP2, iColP3 ) * operator()( iRowP3, iColP2 ) -
				 operator()( iRowP1, iColP1 ) * operator()( iRowP2, iColP2 ) * operator()( iRowP3, iColP3 ) +
				 operator()( iRowP1, iColP2 ) * operator()( iRowP2, iColP3 ) * operator()( iRowP3, iColP1 ) +
				 operator()( iRowP1, iColP3 ) * operator()( iRowP2, iColP1 ) * operator()( iRowP3, iColP2 ) );
	}
	else
	{
		return ( operator()( iRowP1, iColP1 ) * operator()( iRowP2, iColP2 ) * operator()( iRowP3, iColP3 ) +
				 operator()( iRowP1, iColP2 ) * operator()( iRowP2, iColP3 ) * operator()( iRowP3, iColP1 ) +
				 operator()( iRowP1, iColP3 ) * operator()( iRowP2, iColP1 ) * operator()( iRowP3, iColP2 ) -
				 operator()( iRowP1, iColP3 ) * operator()( iRowP2, iColP2 ) * operator()( iRowP3, iColP1 ) -
				 operator()( iRowP1, iColP2 ) * operator()( iRowP2, iColP1 ) * operator()( iRowP3, iColP3 ) -
				 operator()( iRowP1, iColP1 ) * operator()( iRowP2, iColP3 ) * operator()( iRowP3, iColP2 ) );
	}
}

bool VistaTransformMatrix::Decompose( VistaVector3D& v3Translation,
									VistaQuaternion& qRotation,
									VistaVector3D& v3Scale,
									VistaQuaternion& qScaleRotation ) const
{
	AffineParts parts;
	HMatrix hmatrix;

	// copy matrix
	for(int i =0; i<4; i++)
	{
		for(int j=0; j<4; j++)
		{
			hmatrix[i][j] = operator()( i, j );
		}
	}

	decomp_affine(hmatrix, &parts);

	float mul = 1;
	if( parts.t[Vista::W] != 0 ) 
		mul = 1 / parts.t[Vista::W];

	v3Translation[0] = parts.t[Vista::X] * mul;
	v3Translation[1] = parts.t[Vista::Y] * mul;
	v3Translation[2] = parts.t[Vista::Z] * mul;

	qRotation[0] = parts.q.x;
	qRotation[1] = parts.q.y;
	qRotation[2] = parts.q.z;
	qRotation[3] = parts.q.w;

	mul = 1;
	if(parts.k.w != 0) 
		mul = 1 / parts.k.w;

	// mul be sign of determinant to support negative scales.
	mul *= parts.f;
	v3Scale[0] = parts.k.x * mul;
	v3Scale[1] = parts.k.y * mul;
	v3Scale[2] = parts.k.z * mul;

	qScaleRotation[0] = parts.u.x;
	qScaleRotation[1] = parts.u.y;
	qScaleRotation[2] = parts.u.z;
	qScaleRotation[3] = parts.u.w;

	return true;
}
bool VistaTransformMatrix::Decompose( VistaVector3D& v3Translation,
									VistaQuaternion& qRotation,
									VistaVector3D& v3Scale ) const
{
	AffineParts parts;
	HMatrix hmatrix;

	// copy matrix
	for(int i =0; i<4; i++)
	{
		for(int j=0; j<4; j++)
		{
			hmatrix[i][j] = operator()( i, j );
		}
	}

	decomp_affine(hmatrix, &parts);

	// check if the scale is rotated
	if( parts.u.w < 0.99f )
	{
		// a non-unit rotation is valid only if the scale is uniform!
		float fDiff = 0.00001f * ( parts.k.x + parts.k.y + parts.k.z );
		if( std::abs( parts.k.x - parts.k.y ) > fDiff
			|| std::abs( parts.k.x - parts.k.z ) > fDiff )
		{
			// shearing component, Decomposition fails
			return false;
		}
	}

	float mul = 1;
	if (parts.t[Vista::W] != 0) 
		mul = 1 / parts.t[Vista::W];

	v3Translation[0] = parts.t[Vista::X] * mul;
	v3Translation[1] = parts.t[Vista::Y] * mul;
	v3Translation[2] = parts.t[Vista::Z] * mul;

	qRotation[0] = parts.q.x;
	qRotation[1] = parts.q.y;
	qRotation[2] = parts.q.z;
	qRotation[3] = parts.q.w;

	mul = 1;
	if(parts.k.w != 0) 
		mul = 1 / parts.k.w;

	// mul be sign of determinant to support negative scales.
	mul *= parts.f;
	v3Scale[0] = parts.k.x * mul;
	v3Scale[1] = parts.k.y * mul;
	v3Scale[2] = parts.k.z * mul;

	return true;
}
bool VistaTransformMatrix::Compose( const VistaVector3D& v3Translation,
									const VistaQuaternion& qRotation,
									const VistaVector3D& v3Scale,
									const VistaQuaternion& qScaleRotation )
{
	VistaTransformMatrix matOri( qRotation );
	VistaTransformMatrix matScale;
	matScale.SetToScaleMatrix( v3Scale[0], v3Scale[1], v3Scale[2] );


	VistaTransformMatrix matScaleOri( qScaleRotation );
	VistaTransformMatrix matInvScaleOri( qScaleRotation.GetInverted() );

	(*this) = matOri * matScaleOri * matScale * matInvScaleOri;
	SetTranslation( v3Translation );
	return true;
}

bool VistaTransformMatrix::Compose( const VistaVector3D& v3Translation,
									const VistaQuaternion& qRotation,
									const VistaVector3D& v3Scale )
{
	VistaTransformMatrix matOri( qRotation );
	VistaTransformMatrix matScale;
	matScale.SetToScaleMatrix( v3Scale[0], v3Scale[1], v3Scale[2] );

	(*this) = matOri * matScale;
	SetTranslation( v3Translation );
	return true;
}

VistaQuaternion VistaTransformMatrix::GetRotationAsQuaternion() const
{
	return VistaQuaternion( (*this) );
}

VistaTransformMatrix::VistaTransformMatrix( const VistaVector3D& v3XAxis,
										   const VistaVector3D& v3YAxis,
										   const VistaVector3D& v3ZAxis )
{
	for( int i = 0; i < 4; ++i )
	{
		operator()( i, 0 ) = v3XAxis[i];
		operator()( i, 1 ) = v3YAxis[i];
		operator()( i, 2 ) = v3ZAxis[i];
	}
	operator()( 0, 3 ) = 0.0f;
	operator()( 1, 3 ) = 0.0f;
	operator()( 2, 3 ) = 0.0f;
	operator()( 3, 3 ) = 1.0f;
}

