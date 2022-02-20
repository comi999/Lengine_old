#pragma once
#include <tuple>

template < typename T, size_t S >
struct Vector;

template < typename T, size_t M, size_t N = M >
struct Matrix;

enum class RotationOrder : char
{
	XYZ,
	XZY,
	YXZ,
	YZX,
	ZXY,
	ZYX
};

typedef Matrix< float, 2 > Matrix2;
typedef Matrix< float, 3 > Matrix3;
typedef Matrix< float, 4 > Matrix4;
typedef Matrix< int, 2 > Matrix2Int;
typedef Matrix< int, 3 > Matrix3Int;
typedef Matrix< int, 4 > Matrix4Int;

class Math
{
	Math() = delete;

public:

	template < typename T >
	inline static T Abs( T a_Scalar )
	{
		return abs( a_Scalar );
	}

	template < typename T >
	inline static T ACos( T a_Scalar )
	{
		return acos( a_Scalar );
	}

	template < typename T >
	inline static T ACosh( T a_Scalar )
	{
		return acosh( a_Scalar );
	}

	template < typename T >
	inline static T ASin( T a_Scalar )
	{
		return asin( a_Scalar );
	}

	template < typename T >
	inline static T ASinh( T a_Scalar )
	{
		return asinh( a_Scalar );
	}

	template < typename T >
	inline static T ATan( T a_Scalar )
	{
		return atan( a_Scalar );
	}

	template < typename T >
	inline static T ATan( T a_Y, T a_X )
	{
		return atan2( a_Y, a_X );
	}

	template < typename T >
	inline static T ATanh( T a_Scalar )
	{
		return atanh( a_Scalar );
	}

	template < typename T, size_t S >
	inline static Vector< T, S > Abs( Vector< T, S > a_Vector )
	{
		for ( size_t i = 0; i < S; ++i )
		{
			a_Vector[ i ] = abs( a_Vector[ i ] );
		}

		return a_Vector;
	}

	template < typename T, size_t M, size_t N >
	inline static Matrix< T, M, N > Adjoint( const Matrix< T, M, N >& a_Matrix )
	{
		return Transpose( Cofactor( a_Matrix ) );
	}

	template < typename T >
	inline static T Ceil( T a_Scalar )
	{
		return ceil( a_Scalar );
	}

	template < typename T, size_t S >
	inline static Vector< T, S > Ceil( Vector< T, S > a_Vector )
	{
		for ( size_t i = 0; i < S; ++i )
		{
			a_Vector[ i ] = ceil( a_Vector[ i ] );
		}

		return a_Vector;
	}

	template < typename T >
	inline static T Clamp( T a_Scalar, T a_Min, T a_Max )
	{
		if ( a_Scalar < a_Min )
		{
			return a_Min;
		}

		if ( a_Scalar > a_Max )
		{
			return a_Max;
		}

		return a_Scalar;
	}

	template < typename T, size_t S >
	inline static Vector< T, S > Clamp( Vector< T, S > a_Vector, T a_Min, T a_Max )
	{
		for ( size_t i = 0; i < S; ++i )
		{
			T& Value = a_Vector[ i ];

			if ( Value < a_Min )
			{
				Value = a_Min;
			}

			if ( Value > a_Max )
			{
				Value = a_Max;
			}
		}

		return a_Vector;
	}

	template < typename T, size_t S >
	inline static Vector< T, S > Clamp( Vector< T, S > a_Vector, Vector< T, S > a_Min, Vector< T, S > a_Max )
	{
		for ( size_t i = 0; i < S; ++i )
		{
			T& Value = a_Vector[ i ];
			T& Min = a_Min[ i ];
			T& Max = a_Max[ i ];

			if ( Value < Min )
			{
				Value = Min;
			}

			if ( Value > Max )
			{
				Value = Max;
			}
		}

		return a_Vector;
	}

	template < typename T, size_t M, size_t N >
	static Matrix< T, M, N > Cofactor( Matrix< T, M, N > a_Matrix )
	{
		size_t Begin = 1;

		for ( size_t m = 0; m < M; ++m )
		{
			auto& Row = a_Matrix.GetRow( m );

			for ( size_t n = Begin; n < N; n += 2 )
			{
				Row[ n ] *= static_cast< T >( -1 );
			}

			Begin = !Begin;
		}

		return a_Matrix;
	}

	template < typename T >
	inline static T Cos( T a_Radians )
	{
		return cos( a_Radians );
	}

	template < typename T >
	inline static T Cosh( T a_Scalar )
	{
		return cosh( a_Scalar );
	}

	template < typename T0, typename T1 >
	static auto Cross( const Vector< T0, 3 >& a_VectorA, const Vector< T1, 3 >& a_VectorB )
	{
		Matrix< T0, 3 > Result;

		for ( int i = 0; i < 3; ++i )
		{
			Result[ i + 3 ] = a_VectorA[ i ];
			Result[ i + 6 ] = a_VectorB[ i ];
		}

		reinterpret_cast< MatrixIndexer< Indexer< T0, 3, 3 >, Indexer< T0, 3 > >& >( Result ).Cross();
		return Result.GetRow( 0 ).ToVector();
	}

	template < typename T >
	inline static T Degrees( T a_Radians )
	{
		return static_cast< T >( 180 ) * InversePi() * a_Radians;
	}

	template < typename T, size_t S >
	static inline T Determinant( const Matrix< T, S, S >& a_Matrix )
	{
		return reinterpret_cast< const MatrixIndexer< Indexer< T, S, S >, Indexer< T, S > >& >( a_Matrix ).Determinant();
	}

	template < typename T, size_t S >
	static T Distance( const Vector< T, S >& a_VectorA, const Vector< T, S >& a_VectorB )
	{
		return Length( a_VectorB - a_VectorA );
	}

	template < typename T, size_t S >
	static T DistanceSqrd( const Vector< T, S >& a_VectorA, const Vector< T, S >& a_VectorB )
	{
		return LengthSqrd( a_VectorB - a_VectorA );
	}

	template < typename T, typename U, size_t S >
	static T Dot( const Vector< T, S >& a_VectorA, const Vector< U, S >& a_VectorB )
	{
		Vector< T, S > Multiplied = a_VectorA * a_VectorB;
		return Manhatten( Multiplied );
	}

	template < typename T = float >
	inline static constexpr T E()
	{
		return static_cast< T >( 2.71828182845904523536 );
	}

	template < typename T = float >
	inline static constexpr T Epsilon()
	{
		return static_cast< T >( 1.17549435e-38f );
	}

	template < typename T >
	inline static T Exp( T a_Scalar )
	{
		return exp( a_Scalar );
	}

	template < typename T >
	inline static T Exp2( T a_Scalar )
	{
		return exp2( a_Scalar );
	}

	template < typename T >
	inline static T Floor( T a_Scalar )
	{
		return floor( a_Scalar );
	}

	template < typename T, size_t S >
	inline static Vector< T, S > Floor( Vector< T, S > a_Vector )
	{
		for ( size_t i = 0; i < S; ++i )
		{
			a_Vector[ i ] = floor( a_Vector[ i ] );
		}

		return a_Vector;
	}

	template < typename T >
	inline static T Fract( T a_Scalar )
	{
		return a_Scalar - Floor( a_Scalar );
	}

	template < typename T, size_t S >
	inline static Vector< T, S > Fract( const Vector< T, S >& a_Vector )
	{
		return a_Vector - Floor( a_Vector );
	}

	template < typename T >
	inline static float Inverse( T a_Scalar )
	{
		return 1.0f / a_Scalar;
	}

	template < typename T, size_t S >
	static Vector< T, S > Inverse( const Vector< T, S >& a_Vector )
	{
		return a_Vector * static_cast< T >( -1 );
	}

	template < typename T, size_t S >
	static Matrix< T, S > Inverse( const Matrix< T, S >& a_Matrix )
	{
		T Det = static_cast< T >( 0 );
		auto Min = Math::Minor( a_Matrix );
		auto& MinRow = Min.GetRow( 0 );
		auto& MatRow = a_Matrix.GetRow( 0 );

		for ( size_t i = 0; i < S; i += 2 )
		{
			Det += MinRow[ i ] * MatRow[ i ];
		}

		for ( size_t i = 1; i < S; i += 2 )
		{
			Det += MinRow[ i ] * MatRow[ i ] * static_cast< T >( -1 );
		}

		if ( Det == static_cast< T >( 0 ) )
		{
			return Matrix< T, S >::Zero;
		}

		return reinterpret_cast< Matrix< T, S >&& >( Math::Transpose( Math::Cofactor( Min ) ) * ( 1.0f / Det ) );
	}

	template < typename T = float >
	inline static constexpr T InverseE()
	{
		return static_cast< T >( 1.0 / 2.71828182845904523536 );
	}

	template < typename T = float >
	inline static constexpr T InversePi()
	{
		return static_cast< T >( 1.0 / 3.14159265358979323846 );
	}

	template < typename T >
	inline static float InverseSqrt( T a_Scalar )
	{
		long I;
		float X2, Y;
		const float Threehalfs = 1.5f;

		X2 = a_Scalar * 0.5f;
		Y = a_Scalar;
		I = *( long* )&Y;
		I = 0x5f3759df - ( I >> 1 );
		Y = *( float* )&I;
		Y = Y * ( Threehalfs - ( X2 * Y * Y ) );

		return Y;
	}

	template < typename T >
	inline static bool IsEpsilonEqual( T a_ValueA, T a_ValueB )
	{
		return Abs( a_ValueA - a_ValueB ) <= Epsilon();
	}

	template < typename T >
	inline static bool IsEpsilonNotEqual( T a_ValueA, T a_ValueB )
	{
		return Abs( a_ValueA - a_ValueB ) > Epsilon();
	}

	template < typename T >
	inline static bool IsInf( T a_Scalar )
	{
		return isinf( a_Scalar );
	}

	template < typename T >
	inline static bool IsNan( T a_Scalar )
	{
		return isnan( a_Scalar );
	}

	template < typename T, size_t S >
	inline static float Length( const Vector< T, S >& a_Vector )
	{
		const T* Data = reinterpret_cast< const T* >( &a_Vector );
		T Total = 0;

		for ( size_t Index = 0; Index < S; ++Index )
		{
			T Component = Data[ Index ];
			Total += Component * Component;
		}

		return sqrt( Total );
	}

	template < typename T, size_t S >
	inline static T LengthSqrd( const Vector< T, S >& a_Vector )
	{
		const T* Data = reinterpret_cast< const T* >( &a_Vector );
		T Total = 0;

		for ( size_t Index = 0; Index < S; ++Index )
		{
			T Component = Data[ Index ];
			Total += Component * Component;
		}

		return Total;
	}

	template < typename T >
	inline static float Log( T a_Scalar )
	{
		return log( a_Scalar );
	}

	template < typename T >
	inline static float Log2( T a_Scalar )
	{
		return log2( a_Scalar );
	}

	template < typename T, size_t S >
	static T Manhatten( const Vector< T, S >& a_Vector )
	{
		T Total = static_cast< T >( 0 );

		for ( int i = 0; i < S; ++i )
		{
			Total += a_Vector[ i ];
		}

		return Total;
	}

	template < typename T >
	inline static T Max( T a_ScalarA, T a_ScalarB )
	{
		return a_ScalarA > a_ScalarB ? a_ScalarA : a_ScalarB;
	}

	template < typename T, size_t S >
	static Vector< T, S > Max( Vector< T, S > a_VectorA, const Vector< T, S >& a_VectorB )
	{
		for ( size_t i = 0; i < S; ++i )
		{
			T& A = a_VectorA[ i ];
			T& B = a_VectorB[ i ];

			if ( A < B )
			{
				A = B;
			}
		}

		return a_VectorA;
	}

	template < typename T >
	inline static T Min( T a_ScalarA, T a_ScalarB )
	{
		return a_ScalarA < a_ScalarB ? a_ScalarA : a_ScalarB;
	}

	template < typename T, size_t S >
	static Vector< T, S > Min( Vector< T, S > a_VectorA, const Vector< T, S >& a_VectorB )
	{
		for ( size_t i = 0; i < S; ++i )
		{
			T& A = a_VectorA[ i ];
			T& B = a_VectorB[ i ];

			if ( A > B )
			{
				A = B;
			}
		}

		return a_VectorA;
	}

	template < typename T, size_t S >
	inline static Matrix< T, S > Minor( const Matrix< T, S >& a_Matrix )
	{
		return reinterpret_cast< const MatrixIndexer< Indexer< T, S, S >, Indexer< T, S > >& >( a_Matrix ).Minor();
	}

	template < typename T, size_t S >
	inline static Vector< T, S > Normalize( Vector< T, S > a_Vector )
	{
		return a_Vector * InverseSqrt( LengthSqrd( a_Vector ) );
	}

	template < typename T = float >
	inline static constexpr T Pi()
	{
		return static_cast< T >( 3.14159265358979323846 );
	}

	template < typename T >
	inline static float Pow( T a_Base, T a_Pow )
	{
		return pow( a_Base, a_Pow );
	}

	template < typename T >
	inline static T Radians( T a_Degrees )
	{
		return a_Degrees * Pi() * static_cast< float >( 0.00555555555555555555 );
	}

	template < typename T, size_t S >
	inline static Vector< T, S > Reflect( const Vector< T, S >& a_Incident, const Vector< T, S >& a_Normal )
	{
		return a_Incident - a_Normal * static_cast< T >( 2 ) * Dot( a_Incident, a_Normal );
	}

	template < typename T, size_t S >
	static Vector< T, S > Refract( const Vector< T, S >& a_Incident, const Vector< T, S >& a_Normal, T a_Index )
	{
		auto NI = Dot( a_Incident, a_Normal );
		return a_Normal * Math::Sqrt( static_cast< T >( 1 ) - a_Index * a_Index * ( static_cast< T >( 1 ) - NI * NI ) ) +
			( a_Incident - a_Normal * NI ) * a_Index;
	}

	template < typename T >
	inline static T Round( T a_Scalar )
	{
		return round( a_Scalar );
	}

	template < typename T, size_t S >
	inline static Vector< T, S > Round( Vector< T, S > a_Vector )
	{
		for ( size_t i = 0; i < S; ++i )
		{
			a_Vector[ i ] = round( a_Vector[ i ] );
		}

		return a_Vector;
	}

	template < typename T1, typename T2, size_t S >
	inline static Vector< T1, S > Multiply( const Vector< T1, S >& a_VectorA, const Vector< T2, S >& a_VectorB )
	{
		return a_VectorA * a_VectorB;
	}

	template < typename T1, typename T2, size_t M, size_t N >
	static Vector< T1, M > Multiply( const Matrix< T1, M, N >& a_Matrix, const Vector< T2, N >& a_Vector )
	{
		Vector< T1, M > Result;

		for ( size_t m = 0; m < M; ++m )
		{
			Result[ m ] = Manhatten( a_Matrix.GetRow( m ) * a_Vector );
		}

		return Result;
	}

	template < typename T1, typename T2, size_t M, size_t N >
	static Vector< T1, N > Multiply( const Vector< T1, M >& a_Vector, const Matrix< T2, M, N >& a_Matrix )
	{
		Vector< T1, N > Result;

		for ( size_t n = 0; n < N; ++n )
		{
			Result[ n ] = Manhatten( a_Matrix.GetCol( n ) * a_Vector );
		}

		return Result;
	}

	template < typename T1, typename T2, size_t M, size_t N0, size_t N1 >
	static Matrix< T1, N0, N1 > Multiply( const Matrix< T1, M, N0 >& a_MatrixA, const Matrix< T2, N0, N1 >& a_MatrixB )
	{
		Matrix< T1, N0, N1 > Result;

		for ( size_t m = 0; m < N0; ++m )
		{
			auto& Row = Result.GetRow( m );

			for ( size_t n = 0; n < N1; ++n )
			{
				Row[ n ] = Manhatten( a_MatrixA.GetRow( m ) * a_MatrixB.GetCol( n ) );
			}
		}

		return Result;
	}

	template < typename T >
	inline static T Sign( T a_Scalar )
	{
		return a_Scalar >= static_cast< T >( 0 ) ? static_cast< T >( 1 ) : static_cast< T >( -1 );
	}

	template < typename T >
	inline static T Sin( T a_Radians )
	{
		return sin( a_Radians );
	}

	template < typename T >
	inline static T Sinh( T a_Scalar )
	{
		return sinh( a_Scalar );
	}

	template < typename T >
	inline static T SmoothStep( T a_Scalar, T a_EdgeA, T a_EdgeB )
	{
		if ( a_Scalar < a_EdgeA )
		{
			return static_cast< T >( 0 );
		}

		if ( a_Scalar > a_EdgeB )
		{
			return static_cast< T >( 1 );
		}

		return ( a_EdgeB - a_EdgeA ) / ( a_Scalar - a_EdgeA );
	}

	template < typename T >
	inline static T Step( T a_Scalar, T a_Edge )
	{
		if ( a_Scalar < a_Edge )
		{
			return static_cast< T >( 0 );
		}

		return static_cast< T >( 1 );
	}

	template < typename T >
	static float Sqrt( T a_Scalar )
	{
		return sqrt( a_Scalar );
	}

	template < typename T >
	inline static T Tan( T a_Radians )
	{
		return tan( a_Radians );
	}

	template < typename T >
	inline static T Tanh( T a_Scalar )
	{
		return tanh( a_Scalar );
	}

	template < typename T, size_t M, size_t N >
	static Matrix< T, N, M > Transpose( const Matrix< T, M, N >& a_Matrix )
	{
		Matrix< T, N, M > Result;

		for ( int m = 0; m < M; ++m )
		{
			Result.GetCol( m ) = a_Matrix.GetRow( m );
		}

		return Result;
	}

	template < typename T >
	static inline T Truncate( T a_Scalar, T a_Bound )
	{
		a_Bound = Abs( a_Bound );
		return Clamp( a_Scalar, -a_Bound, a_Bound );
	}

private:

	template < typename T, size_t Span, size_t I = 1  >
	struct Indexer
	{
		typedef typename T ValueType;
		static constexpr size_t Span = Span;

		inline auto& operator[]( size_t a_Index )
		{
			return reinterpret_cast< ValueType* >( this )[ a_Index * I ];
		}

		inline constexpr auto& operator[]( size_t a_Index ) const
		{
			return reinterpret_cast< const ValueType* >( this )[ a_Index * I ];
		}
	};

	template < typename _Indexer, size_t Skip, size_t Span >
	struct SkipIndexer
	{
		typedef typename _Indexer::ValueType ValueType;
		static constexpr size_t Span = Span;

		inline auto& operator[]( size_t a_Index )
		{
			size_t Offset = a_Index + ( a_Index >= Skip ? 1 : 0 );
			return reinterpret_cast< _Indexer* >( this )->operator[]( Offset );
		}

		inline constexpr auto& operator[]( size_t a_Index ) const
		{
			size_t Offset = a_Index + ( a_Index >= Skip ? 1 : 0 );
			return reinterpret_cast< const _Indexer* >( this )->operator[]( Offset );
		}
	};

	template < typename _IndexerM, typename _IndexerN >
	struct MatrixIndexer
	{
		static constexpr size_t SizeM = _IndexerM::Span;
		static constexpr size_t SizeN = _IndexerN::Span;

		inline auto& GetRow( size_t a_Index )
		{
			return reinterpret_cast< _IndexerN& >( reinterpret_cast< _IndexerM& >( *this )[ a_Index ] );
		}

		inline constexpr auto& GetRow( size_t a_Index ) const
		{
			return reinterpret_cast< const _IndexerN& >( reinterpret_cast< const _IndexerM& >( *this )[ a_Index ] );
		}

		inline auto& GetCol( size_t a_Index )
		{
			return reinterpret_cast< _IndexerM& >( reinterpret_cast< _IndexerN& >( *this )[ a_Index ] );
		}

		inline constexpr auto& GetCol( size_t a_Index ) const
		{
			return reinterpret_cast< const _IndexerM& >( reinterpret_cast< const _IndexerN& >( *this )[ a_Index ] );
		}

		template < size_t _SkipM, size_t _SkipN >
		inline auto& GetSubMatrixIndexer()
		{
			typedef SkipIndexer< _IndexerM, _SkipM, SizeM - 1 > SubMatrixIndexerM;
			typedef SkipIndexer< _IndexerN, _SkipN, SizeN - 1 > SubMatrixIndexerN;

			return reinterpret_cast< MatrixIndexer< SubMatrixIndexerM, SubMatrixIndexerN >& >( *this );
		}

		template < size_t _SkipM, size_t _SkipN >
		inline constexpr auto& GetSubMatrixIndexer() const
		{
			typedef SkipIndexer< _IndexerM, _SkipM, SizeM - 1 > SubMatrixIndexerM;
			typedef SkipIndexer< _IndexerN, _SkipN, SizeN - 1 > SubMatrixIndexerN;

			return reinterpret_cast< const MatrixIndexer< SubMatrixIndexerM, SubMatrixIndexerN >& >( *this );
		}

		template < size_t Col = 0 >
		void Cross()
		{
			GetRow( 0 )[ Col ] = GetSubMatrixIndexer< 0, Col >().Determinant() * ( Col % 2 == 0 ? 1 : -1 );

			if constexpr ( Col < SizeM - 1 )
			{
				Cross< Col + 1 >();
			}
		}

		Matrix< typename _IndexerM::ValueType, SizeM > Minor() const
		{
			Matrix< typename _IndexerM::ValueType, SizeM > Result;
			Minor< 0, 0 >( Result );
			return Result;
		}

		template < size_t Row, size_t Col >
		void Minor( Matrix< typename _IndexerM::ValueType, SizeM >& a_Result ) const
		{
			a_Result.GetRow( Row )[ Col ] = GetSubMatrixIndexer< Row, Col >().Determinant();

			if constexpr ( Col < SizeN - 1 )
			{
				Minor< Row, Col + 1 >( a_Result );
			}
			else
			{
				if constexpr ( Row < SizeM - 1 )
				{
					Minor< Row + 1, 0 >( a_Result );
				}
			}
		}

		template < size_t Col = 0 >
		inline typename _IndexerM::ValueType Determinant() const
		{
			if constexpr ( SizeM == 1 )
			{
				return GetRow( 0 )[ 0 ];
			}

			typename _IndexerM::ValueType Coeff = ( ( static_cast< int >( Col ) % 2 ) == 0 ? 1 : -1 ) * GetRow( 0 )[ Col ];
			return Coeff * GetSubMatrixIndexer< 0, Col >().Determinant() + Determinant< Col + 1 >();
		}

		template <>
		inline typename _IndexerM::ValueType Determinant< SizeN >() const
		{
			return 0;
		}
	};
};

template < typename T, size_t S, size_t I = 1 >
struct IVector
{
	inline auto& operator[]( size_t a_Index )
	{
		return reinterpret_cast< T* >( this )[ a_Index * I ];
	}

	inline const auto& operator[]( size_t a_Index ) const
	{
		return reinterpret_cast< const T* >( this )[ a_Index * I ];
	}

	template < typename U, size_t S0, size_t I0 >
	IVector< T, S, I >& operator=( const IVector< U, S0, I0 >& a_IVector )
	{
		size_t i = 0;
		size_t Size = Math::Min( S0, S );

		for ( ; i < Size; ++i )
		{
			operator[]( i ) = a_IVector[ i ];
		}

		for ( ; i < S; ++i )
		{
			operator[]( i ) = static_cast< T >( 0 );
		}

		return *this;
	}

	constexpr size_t GetDimensions() const
	{
		return S;
	}

	Vector< T, S > ToVector() const
	{
		Vector< T, S > Result;

		for ( size_t i = 0; i < S; ++i )
		{
			Result[ i ] = operator[]( i );
		}

		return Result;
	}

	template < size_t NewS >
	Vector< T, NewS > ToVectorN() const
	{
		Vector< T, NewS > Result;
		size_t i = 0;
		size_t Size = NewS < S ? NewS : S;

		for ( ; i < Size; ++i )
		{
			Result[ i ] = operator[]( i );
		}

		for ( ; i < NewS; ++i )
		{
			Result[ i ] = static_cast< T >( 0 );
		}

		return Result;
	}

	inline Vector< T, 2 > ToVector2() const
	{
		return ToVectorN< 2 >();
	}

	inline Vector< T, 3 > ToVector3() const
	{
		return ToVectorN< 3 >();
	}

	inline Vector< T, 4 > ToVector4() const
	{
		return ToVectorN< 4 >();
	}

	template < size_t M, size_t N = M >
	inline Matrix< T, M, N > ToMatrixMN() const
	{
		return reinterpret_cast< Matrix< T, M, N >&& >( ToVectorN< M* N >() );
	}
};

template < typename T, typename U, size_t S, size_t I0, size_t I1 >
Vector< T, S > operator+( const IVector< T, S, I0 >& a_VectorA, const IVector< U, S, I1 >& a_VectorB )
{
	Vector< T, S > Result;

	for ( size_t i = 0; i < S; ++i )
	{
		Result[ i ] = a_VectorA[ i ] + static_cast< T >( a_VectorB[ i ] );
	}

	return Result;
}

template < typename T, size_t S, size_t I >
Vector< T, S > operator+( const IVector< T, S, I >& a_Vector )
{
	return reinterpret_cast< const Vector< T, S >& >( a_Vector );
}

template < typename T, typename U, size_t S, size_t I0, size_t I1 >
Vector< T, S >& operator+=( IVector< T, S, I0 >& a_VectorA, const IVector< U, S, I1 >& a_VectorB )
{
	for ( size_t i = 0; i < S; ++i )
	{
		a_VectorA[ i ] += static_cast< T >( a_VectorB[ i ] );
	}

	return reinterpret_cast< Vector< T, S >& >( a_VectorA );
}

template < typename T, typename U, size_t S, size_t I0, size_t I1 >
Vector< T, S > operator-( const IVector< T, S, I0 >& a_VectorA, const IVector< U, S, I1 >& a_VectorB )
{
	Vector< T, S > Result;

	for ( size_t i = 0; i < S; ++i )
	{
		Result[ i ] = a_VectorA[ i ] - static_cast< T >( a_VectorB[ i ] );
	}

	return Result;
}

template < typename T, size_t S, size_t I >
Vector< T, S > operator-( const IVector< T, S, I >& a_Vector )
{
	Vector< T, S > Result;
	
	for ( size_t i = 0; i < S; ++i )
	{
		Result[ i ] = -a_Vector[ i ];
	}

	return Result;
}

template < typename T, typename U, size_t S, size_t I0, size_t I1 >
Vector< T, S >& operator-=( IVector< T, S, I0 >& a_VectorA, const IVector< U, S, I1 >& a_VectorB )
{
	for ( size_t i = 0; i < S; ++i )
	{
		a_VectorA[ i ] -= static_cast< T >( a_VectorB[ i ] );
	}

	return reinterpret_cast< Vector< T, S >& >( a_VectorA );
}

template < typename T, typename U, size_t S, size_t I0, size_t I1 >
Vector< T, S > operator*( const IVector< T, S, I0 >& a_VectorA, const IVector< U, S, I1 >& a_VectorB )
{
	Vector< T, S > Result;

	for ( size_t i = 0; i < S; ++i )
	{
		Result[ i ] = a_VectorA[ i ] * static_cast< T >( a_VectorB[ i ] );
	}

	return Result;
}

template < typename T, typename U, size_t S, size_t I0, size_t I1 >
Vector< T, S >& operator*=( IVector< T, S, I0 >& a_VectorA, const IVector< U, S, I1 >& a_VectorB )
{
	for ( size_t i = 0; i < S; ++i )
	{
		a_VectorA[ i ] *= static_cast< T >( a_VectorB[ i ] );
	}

	return reinterpret_cast< Vector< T, S >& >( a_VectorA );
}

template < typename T, typename U, size_t S, size_t I, typename = std::enable_if_t< std::is_arithmetic_v< U > > >
Vector< T, S > operator*( const IVector< T, S, I >& a_Vector, U a_Scalar )
{
	Vector< T, S > Result;

	for ( size_t i = 0; i < S; ++i )
	{
		Result[ i ] = a_Vector[ i ] * static_cast< T >( a_Scalar );
	}

	return Result;
}

template < typename T, typename U, size_t S, size_t I, typename = std::enable_if_t< std::is_arithmetic_v< U > > >
Vector< T, S >& operator*=( IVector< T, S, I >& a_Vector, U a_Scalar )
{
	for ( size_t i = 0; i < S; ++i )
	{
		a_Vector[ i ] *= static_cast< T >( a_Scalar );
	}

	return reinterpret_cast< Vector< T, S >& >( a_Vector );
}

template < typename T, typename U, size_t S, size_t I, typename = std::enable_if_t< std::is_arithmetic_v< T > > >
Vector< T, S > operator*( T a_Scalar, const IVector< U, S, I >& a_Vector )
{
	Vector< T, S > Result;

	for ( size_t i = 0; i < S; ++i )
	{
		Result[ i ] = static_cast< T >( a_Vector[ i ] ) * a_Scalar;
	}

	return Result;
}

template < typename T, typename U, size_t S, size_t I, typename = std::enable_if_t< std::is_arithmetic_v< U > > >
Vector< T, S > operator/( const IVector< T, S, I >& a_Vector, U a_Scalar )
{
	Vector< T, S > Result;
	float Scalar = 1.0f / a_Scalar;

	for ( size_t i = 0; i < S; ++i )
	{
		Result[ i ] = a_Vector[ i ] * Scalar;
	}

	return Result;
}

template < typename T, typename U, size_t S, size_t I, typename = std::enable_if_t< std::is_arithmetic_v< U > > >
Vector< T, S >& operator/=( IVector< T, S, I >& a_Vector, U a_Scalar )
{
	float Scalar = 1.0f / a_Scalar;

	for ( size_t i = 0; i < S; ++i )
	{
		a_Vector[ i ] *= Scalar;
	}

	return reinterpret_cast< Vector< T, S >& >( a_Vector );
}

template < typename T, typename U, size_t S, size_t I0, size_t I1 >
bool operator==( const IVector< T, S, I0 >& a_VectorA, const IVector< U, S, I1 >& a_VectorB )
{
	for ( size_t i = 0; i < S; ++i )
	{
		if ( a_VectorA[ i ] != static_cast< T >( a_VectorB[ i ] ) )
		{
			return false;
		}
	}

	return true;
}

template < typename T, typename U, size_t S, size_t I0, size_t I1 >
inline bool operator!=( const IVector< T, S, I0 >& a_VectorA, const IVector< U, S, I1 >& a_VectorB )
{
	return !( a_VectorA == a_VectorB );
}

template < typename T, size_t S >
struct Vector : public IVector< T, S >
{
	T Data[ S ];

	Vector() = default;

	template < typename U, typename = std::enable_if_t< std::is_arithmetic_v< U > > >
	Vector( U a_Scalar )
	{
		for ( size_t i = 0; i < S; ++i )
		{
			Data[ i ] = static_cast< T >( a_Scalar );
		}
	}

	template < typename U, typename = std::enable_if_t< std::is_arithmetic_v< U > > >
	Vector( std::initializer_list< U >&& a_InitializerList )
	{
		size_t Size = S < a_InitializerList.size() ? S : a_InitializerList.size();
		auto Begin = a_InitializerList.begin();

		for ( size_t i = 0; i < Size; ++i, ++Begin )
		{
			Data[ i ] = static_cast< T >( *Begin );
		}
	}

	static const Vector< T, S > Zero;
	static const Vector< T, S > One;
};

template < typename T, size_t S > const Vector< T, S > Vector< T, S >::Zero = Vector< T, S >( 0 );
template < typename T, size_t S > const Vector< T, S > Vector< T, S >::One  = Vector< T, S >( 1 );

template < typename T, size_t... I >
struct Swizzler : public IVector< T, sizeof...( I ) >
{
	template < typename U >
	operator Vector< U, sizeof...( I ) >() const
	{
		Vector< U, sizeof...( I ) > Output;
		Repack< U, I... >( reinterpret_cast< U* >( &Output ), reinterpret_cast< const T* >( this ) );
		return Output;
	}

	inline Vector< T, sizeof...( I ) > ToVector() const
	{
		return *this;
	}

	template < size_t NewS >
	Vector< T, NewS > ToVectorN() const
	{
		Vector< T, sizeof...( I ) > Origin = ToVector();
		Vector< T, NewS > Result;
		size_t i = 0;
		size_t Size = NewS < sizeof...( I ) ? NewS : sizeof...( I );

		for ( ; i < Size; ++i )
		{
			Result[ i ] = Origin[ i ];
		}

		for ( ; i < NewS; ++i )
		{
			Result[ i ] = static_cast< T >( 0 );
		}

		return Result;
	}

	inline Vector< T, 2 > ToVector2() const
	{
		return ToVectorN< 2 >();
	}

	inline Vector< T, 3 > ToVector3() const
	{
		return ToVectorN< 3 >();
	}

	inline Vector< T, 4 > ToVector4() const
	{
		return ToVectorN< 4 >();
	}

	template < size_t M, size_t N = M >
	inline Matrix< T, M, N > ToMatrixMN() const
	{
		return reinterpret_cast< Matrix< T, M, N >&& >( ToVectorN< M * N >() );
	}

private:

	template < typename U, size_t I0, size_t... I >
	inline constexpr static void Repack( U* a_To, const T* a_From )
	{
		a_To[ 0 ] = static_cast< U >( a_From[ I0 ] );

		if constexpr ( sizeof...( I ) != 0 )
		{
			Repack< U, I... >( a_To + 1, a_From );
		}
	}
};

template < typename T >
struct Vector< T, 2 > : public IVector< T, 2 >
{
	union
	{
		T Data[ 2 ];
		struct { T x; T y; };

		union
		{
			Swizzler< T, 0, 0 > xx;
			Swizzler< T, 0, 1 > xy;
			Swizzler< T, 1, 0 > yx;
			Swizzler< T, 1, 1 > yy;
		} swizzle;
	};

	Vector() = default;

	template < typename U, typename = std::enable_if_t< std::is_arithmetic_v< U > > >
	Vector( U a_Scalar )
	{
		for ( size_t i = 0; i < 2; ++i )
		{
			Data[ i ] = static_cast< T >( a_Scalar );
		}
	}

	template < typename U >
	Vector( U a_X, U a_Y )
		: x( static_cast< T >( a_X ) )
		, y( static_cast< T >( a_Y ) )
	{ }

	template < typename U, size_t S0, size_t I0, typename... V >
	Vector( const IVector< U, S0, I0 >& a_Vector, V&&... a_Trailing )
	{
		size_t Size = Math::Min( S0, static_cast< size_t >( 2 ) );
		size_t i = 0;
		auto Trailing = std::forward_as_tuple( a_Trailing... );

		for ( ; i < Size; ++i )
		{
			this->operator[]( i ) = static_cast< T >( a_Vector[ i ] );
		}

		if constexpr ( S0 < 2 && std::tuple_size_v< decltype( Trailing ) > > 0 )
		{
			++i;
			this->operator[]( S0 ) = static_cast< T >( std::get< 0 >( Trailing ) );
		}

		if constexpr ( S0 < 1 && std::tuple_size_v< decltype( Trailing ) > > 1 )
		{
			++i;
			this->operator[]( S0 + 1 ) = static_cast< T >( std::get< 1 >( Trailing ) );
		}

		for ( ; i < 2; ++i )
		{
			this->operator[]( i ) = static_cast< T >( 0 );
		}
	}

	static const Vector< T, 2 > Zero;
	static const Vector< T, 2 > One;
	static const Vector< T, 2 > Up;
	static const Vector< T, 2 > Down;
	static const Vector< T, 2 > Left;
	static const Vector< T, 2 > Right;
};

template < typename T > const Vector< T, 2 > Vector< T, 2 >::Zero (  0,  0 );
template < typename T > const Vector< T, 2 > Vector< T, 2 >::One  (  1,  1 );
template < typename T > const Vector< T, 2 > Vector< T, 2 >::Up   (  0,  1 );
template < typename T > const Vector< T, 2 > Vector< T, 2 >::Down (  0, -1 );
template < typename T > const Vector< T, 2 > Vector< T, 2 >::Left ( -1,  0 );
template < typename T > const Vector< T, 2 > Vector< T, 2 >::Right(  1,  0 );

template < typename T >
struct Vector< T, 3 > : public IVector< T, 3 >
{
	union
	{
		T Data[ 3 ];
		struct { T x; T y; T z; };
		
		union
		{
			Swizzler< T, 0, 2 > xx;
			Swizzler< T, 0, 1 > xy;
			Swizzler< T, 0, 2 > xz;
			Swizzler< T, 1, 0 > yx;
			Swizzler< T, 1, 1 > yy;
			Swizzler< T, 1, 2 > yz;
			Swizzler< T, 2, 0 > zx;
			Swizzler< T, 2, 1 > zy;
			Swizzler< T, 2, 2 > zz;
			Swizzler< T, 0, 0, 2 > xxx;
			Swizzler< T, 0, 0, 1 > xxy;
			Swizzler< T, 0, 0, 2 > xxz;
			Swizzler< T, 0, 1, 0 > xyx;
			Swizzler< T, 0, 1, 1 > xyy;
			Swizzler< T, 0, 1, 2 > xyz;
			Swizzler< T, 0, 2, 0 > xzx;
			Swizzler< T, 0, 2, 1 > xzy;
			Swizzler< T, 0, 2, 2 > xzz;
			Swizzler< T, 1, 0, 2 > yxx;
			Swizzler< T, 1, 0, 1 > yxy;
			Swizzler< T, 1, 0, 2 > yxz;
			Swizzler< T, 1, 1, 0 > yyx;
			Swizzler< T, 1, 1, 1 > yyy;
			Swizzler< T, 1, 1, 2 > yyz;
			Swizzler< T, 1, 2, 0 > yzx;
			Swizzler< T, 1, 2, 1 > yzy;
			Swizzler< T, 1, 2, 2 > yzz;
			Swizzler< T, 2, 0, 2 > zxx;
			Swizzler< T, 2, 0, 1 > zxy;
			Swizzler< T, 2, 0, 2 > zxz;
			Swizzler< T, 2, 1, 0 > zyx;
			Swizzler< T, 2, 1, 1 > zyy;
			Swizzler< T, 2, 1, 2 > zyz;
			Swizzler< T, 2, 2, 0 > zzx;
			Swizzler< T, 2, 2, 1 > zzy;
			Swizzler< T, 2, 2, 2 > zzz;
		} swizzle;
	};

	Vector() = default;

	template < typename U, typename = std::enable_if_t< std::is_arithmetic_v< U > > >
	Vector( U a_Scalar )
	{
		for ( size_t i = 0; i < 3; ++i )
		{
			Data[ i ] = static_cast< T >( a_Scalar );
		}
	}

	template < typename U >
	Vector( U a_X, U a_Y, U a_Z )
		: x( static_cast< T >( a_X ) )
		, y( static_cast< T >( a_Y ) )
		, z( static_cast< T >( a_Z ) )
	{ }

	template < typename U, size_t S0, size_t I0, typename... V >
	Vector( const IVector< U, S0, I0 >& a_Vector, V&&... a_Trailing )
	{
		size_t Size = Math::Min( S0, static_cast< size_t >( 3 ) );
		size_t i = 0;
		auto Trailing = std::forward_as_tuple( a_Trailing... );

		for ( ; i < Size; ++i )
		{
			this->operator[]( i ) = static_cast< T >( a_Vector[ i ] );
		}

		if constexpr ( S0 < 3 && std::tuple_size_v< decltype( Trailing ) > > 0 )
		{
			++i;
			this->operator[]( S0 ) = static_cast< T >( std::get< 0 >( Trailing ) );
		}

		if constexpr ( S0 < 2 && std::tuple_size_v< decltype( Trailing ) > > 1 )
		{
			++i;
			this->operator[]( S0 + 1 ) = static_cast< T >( std::get< 1 >( Trailing ) );
		}

		if constexpr ( S0 < 1 && std::tuple_size_v< decltype( Trailing ) > > 2 )
		{
			++i;
			this->operator[]( S0 + 2 ) = static_cast< T >( std::get< 2 >( Trailing ) );
		}

		for ( ; i < 3; ++i )
		{
			this->operator[]( i ) = static_cast< T >( 0 );
		}
	}

	static const Vector< T, 3 > Zero;
	static const Vector< T, 3 > One;
	static const Vector< T, 3 > Up;
	static const Vector< T, 3 > Down;
	static const Vector< T, 3 > Left;
	static const Vector< T, 3 > Right;
	static const Vector< T, 3 > Forward;
	static const Vector< T, 3 > Backward;

};

template < typename T > const Vector< T, 3 > Vector< T, 3 >::Zero    (  0,  0,  0 );
template < typename T > const Vector< T, 3 > Vector< T, 3 >::One     (  1,  1,  1 );
template < typename T > const Vector< T, 3 > Vector< T, 3 >::Up      (  0,  1,  0 );
template < typename T > const Vector< T, 3 > Vector< T, 3 >::Down    (  0, -1,  0 );
template < typename T > const Vector< T, 3 > Vector< T, 3 >::Left    ( -1,  0,  0 );
template < typename T > const Vector< T, 3 > Vector< T, 3 >::Right   (  1,  0,  0 );
template < typename T > const Vector< T, 3 > Vector< T, 3 >::Forward (  0,  0,  1 );
template < typename T > const Vector< T, 3 > Vector< T, 3 >::Backward(  0,  0, -1 );

template < typename T >
struct Vector< T, 4 > : public IVector< T, 4 >
{
	union
	{
		T Data[ 4 ];
		struct { T x; T y; T z; T w; };
		
		union
		{
			Swizzler< T, 0, 0 > xx;
			Swizzler< T, 0, 1 > xy;
			Swizzler< T, 0, 2 > xz;
			Swizzler< T, 0, 3 > xw;
			Swizzler< T, 1, 0 > yx;
			Swizzler< T, 1, 1 > yy;
			Swizzler< T, 1, 2 > yz;
			Swizzler< T, 1, 3 > yw;
			Swizzler< T, 2, 0 > zx;
			Swizzler< T, 2, 1 > zy;
			Swizzler< T, 2, 2 > zz;
			Swizzler< T, 2, 3 > zw;
			Swizzler< T, 3, 0 > wx;
			Swizzler< T, 3, 1 > wy;
			Swizzler< T, 3, 2 > wz;
			Swizzler< T, 3, 3 > ww;
			Swizzler< T, 0, 0, 0 > xxx;
			Swizzler< T, 0, 0, 1 > xxy;
			Swizzler< T, 0, 0, 2 > xxz;
			Swizzler< T, 0, 0, 3 > xxw;
			Swizzler< T, 0, 1, 0 > xyx;
			Swizzler< T, 0, 1, 1 > xyy;
			Swizzler< T, 0, 1, 2 > xyz;
			Swizzler< T, 0, 1, 3 > xyw;
			Swizzler< T, 0, 2, 0 > xzx;
			Swizzler< T, 0, 2, 1 > xzy;
			Swizzler< T, 0, 2, 2 > xzz;
			Swizzler< T, 0, 2, 3 > xzw;
			Swizzler< T, 0, 3, 0 > xwx;
			Swizzler< T, 0, 3, 1 > xwy;
			Swizzler< T, 0, 3, 2 > xwz;
			Swizzler< T, 0, 3, 3 > xww;
			Swizzler< T, 1, 0, 0 > yxx;
			Swizzler< T, 1, 0, 1 > yxy;
			Swizzler< T, 1, 0, 2 > yxz;
			Swizzler< T, 1, 0, 3 > yxw;
			Swizzler< T, 1, 1, 0 > yyx;
			Swizzler< T, 1, 1, 1 > yyy;
			Swizzler< T, 1, 1, 2 > yyz;
			Swizzler< T, 1, 1, 3 > yyw;
			Swizzler< T, 1, 2, 0 > yzx;
			Swizzler< T, 1, 2, 1 > yzy;
			Swizzler< T, 1, 2, 2 > yzz;
			Swizzler< T, 1, 2, 3 > yzw;
			Swizzler< T, 1, 3, 0 > ywx;
			Swizzler< T, 1, 3, 1 > ywy;
			Swizzler< T, 1, 3, 2 > ywz;
			Swizzler< T, 1, 3, 3 > yww;
			Swizzler< T, 2, 0, 0 > zxx;
			Swizzler< T, 2, 0, 1 > zxy;
			Swizzler< T, 2, 0, 2 > zxz;
			Swizzler< T, 2, 0, 3 > zxw;
			Swizzler< T, 2, 1, 0 > zyx;
			Swizzler< T, 2, 1, 1 > zyy;
			Swizzler< T, 2, 1, 2 > zyz;
			Swizzler< T, 2, 1, 3 > zyw;
			Swizzler< T, 2, 2, 0 > zzx;
			Swizzler< T, 2, 2, 1 > zzy;
			Swizzler< T, 2, 2, 2 > zzz;
			Swizzler< T, 2, 2, 3 > zzw;
			Swizzler< T, 2, 3, 0 > zwx;
			Swizzler< T, 2, 3, 1 > zwy;
			Swizzler< T, 2, 3, 2 > zwz;
			Swizzler< T, 2, 3, 3 > zww;
			Swizzler< T, 3, 0, 0 > wxx;
			Swizzler< T, 3, 0, 1 > wxy;
			Swizzler< T, 3, 0, 2 > wxz;
			Swizzler< T, 3, 0, 3 > wxw;
			Swizzler< T, 3, 1, 0 > wyx;
			Swizzler< T, 3, 1, 1 > wyy;
			Swizzler< T, 3, 1, 2 > wyz;
			Swizzler< T, 3, 1, 3 > wyw;
			Swizzler< T, 3, 2, 0 > wzx;
			Swizzler< T, 3, 2, 1 > wzy;
			Swizzler< T, 3, 2, 2 > wzz;
			Swizzler< T, 3, 2, 3 > wzw;
			Swizzler< T, 3, 3, 0 > wwx;
			Swizzler< T, 3, 3, 1 > wwy;
			Swizzler< T, 3, 3, 2 > wwz;
			Swizzler< T, 3, 3, 3 > www;
			Swizzler< T, 0, 0, 0, 0 > xxxx;
			Swizzler< T, 0, 0, 0, 1 > xxxy;
			Swizzler< T, 0, 0, 0, 2 > xxxz;
			Swizzler< T, 0, 0, 0, 3 > xxxw;
			Swizzler< T, 0, 0, 1, 0 > xxyx;
			Swizzler< T, 0, 0, 1, 1 > xxyy;
			Swizzler< T, 0, 0, 1, 2 > xxyz;
			Swizzler< T, 0, 0, 1, 3 > xxyw;
			Swizzler< T, 0, 0, 2, 0 > xxzx;
			Swizzler< T, 0, 0, 2, 1 > xxzy;
			Swizzler< T, 0, 0, 2, 2 > xxzz;
			Swizzler< T, 0, 0, 2, 3 > xxzw;
			Swizzler< T, 0, 0, 3, 0 > xxwx;
			Swizzler< T, 0, 0, 3, 1 > xxwy;
			Swizzler< T, 0, 0, 3, 2 > xxwz;
			Swizzler< T, 0, 0, 3, 3 > xxww;
			Swizzler< T, 0, 1, 0, 0 > xyxx;
			Swizzler< T, 0, 1, 0, 1 > xyxy;
			Swizzler< T, 0, 1, 0, 2 > xyxz;
			Swizzler< T, 0, 1, 0, 3 > xyxw;
			Swizzler< T, 0, 1, 1, 0 > xyyx;
			Swizzler< T, 0, 1, 1, 1 > xyyy;
			Swizzler< T, 0, 1, 1, 2 > xyyz;
			Swizzler< T, 0, 1, 1, 3 > xyyw;
			Swizzler< T, 0, 1, 2, 0 > xyzx;
			Swizzler< T, 0, 1, 2, 1 > xyzy;
			Swizzler< T, 0, 1, 2, 2 > xyzz;
			Swizzler< T, 0, 1, 2, 3 > xyzw;
			Swizzler< T, 0, 1, 3, 0 > xywx;
			Swizzler< T, 0, 1, 3, 1 > xywy;
			Swizzler< T, 0, 1, 3, 2 > xywz;
			Swizzler< T, 0, 1, 3, 3 > xyww;
			Swizzler< T, 0, 2, 0, 0 > xzxx;
			Swizzler< T, 0, 2, 0, 1 > xzxy;
			Swizzler< T, 0, 2, 0, 2 > xzxz;
			Swizzler< T, 0, 2, 0, 3 > xzxw;
			Swizzler< T, 0, 2, 1, 0 > xzyx;
			Swizzler< T, 0, 2, 1, 1 > xzyy;
			Swizzler< T, 0, 2, 1, 2 > xzyz;
			Swizzler< T, 0, 2, 1, 3 > xzyw;
			Swizzler< T, 0, 2, 2, 0 > xzzx;
			Swizzler< T, 0, 2, 2, 1 > xzzy;
			Swizzler< T, 0, 2, 2, 2 > xzzz;
			Swizzler< T, 0, 2, 2, 3 > xzzw;
			Swizzler< T, 0, 2, 3, 0 > xzwx;
			Swizzler< T, 0, 2, 3, 1 > xzwy;
			Swizzler< T, 0, 2, 3, 2 > xzwz;
			Swizzler< T, 0, 2, 3, 3 > xzww;
			Swizzler< T, 0, 3, 0, 0 > xwxx;
			Swizzler< T, 0, 3, 0, 1 > xwxy;
			Swizzler< T, 0, 3, 0, 2 > xwxz;
			Swizzler< T, 0, 3, 0, 3 > xwxw;
			Swizzler< T, 0, 3, 1, 0 > xwyx;
			Swizzler< T, 0, 3, 1, 1 > xwyy;
			Swizzler< T, 0, 3, 1, 2 > xwyz;
			Swizzler< T, 0, 3, 1, 3 > xwyw;
			Swizzler< T, 0, 3, 2, 0 > xwzx;
			Swizzler< T, 0, 3, 2, 1 > xwzy;
			Swizzler< T, 0, 3, 2, 2 > xwzz;
			Swizzler< T, 0, 3, 2, 3 > xwzw;
			Swizzler< T, 0, 3, 3, 0 > xwwx;
			Swizzler< T, 0, 3, 3, 1 > xwwy;
			Swizzler< T, 0, 3, 3, 2 > xwwz;
			Swizzler< T, 0, 3, 3, 3 > xwww;
			Swizzler< T, 1, 0, 0, 0 > yxxx;
			Swizzler< T, 1, 0, 0, 1 > yxxy;
			Swizzler< T, 1, 0, 0, 2 > yxxz;
			Swizzler< T, 1, 0, 0, 3 > yxxw;
			Swizzler< T, 1, 0, 1, 0 > yxyx;
			Swizzler< T, 1, 0, 1, 1 > yxyy;
			Swizzler< T, 1, 0, 1, 2 > yxyz;
			Swizzler< T, 1, 0, 1, 3 > yxyw;
			Swizzler< T, 1, 0, 2, 0 > yxzx;
			Swizzler< T, 1, 0, 2, 1 > yxzy;
			Swizzler< T, 1, 0, 2, 2 > yxzz;
			Swizzler< T, 1, 0, 2, 3 > yxzw;
			Swizzler< T, 1, 0, 3, 0 > yxwx;
			Swizzler< T, 1, 0, 3, 1 > yxwy;
			Swizzler< T, 1, 0, 3, 2 > yxwz;
			Swizzler< T, 1, 0, 3, 3 > yxww;
			Swizzler< T, 1, 1, 0, 0 > yyxx;
			Swizzler< T, 1, 1, 0, 1 > yyxy;
			Swizzler< T, 1, 1, 0, 2 > yyxz;
			Swizzler< T, 1, 1, 0, 3 > yyxw;
			Swizzler< T, 1, 1, 1, 0 > yyyx;
			Swizzler< T, 1, 1, 1, 1 > yyyy;
			Swizzler< T, 1, 1, 1, 2 > yyyz;
			Swizzler< T, 1, 1, 1, 3 > yyyw;
			Swizzler< T, 1, 1, 2, 0 > yyzx;
			Swizzler< T, 1, 1, 2, 1 > yyzy;
			Swizzler< T, 1, 1, 2, 2 > yyzz;
			Swizzler< T, 1, 1, 2, 3 > yyzw;
			Swizzler< T, 1, 1, 3, 0 > yywx;
			Swizzler< T, 1, 1, 3, 1 > yywy;
			Swizzler< T, 1, 1, 3, 2 > yywz;
			Swizzler< T, 1, 1, 3, 3 > yyww;
			Swizzler< T, 1, 2, 0, 0 > yzxx;
			Swizzler< T, 1, 2, 0, 1 > yzxy;
			Swizzler< T, 1, 2, 0, 2 > yzxz;
			Swizzler< T, 1, 2, 0, 3 > yzxw;
			Swizzler< T, 1, 2, 1, 0 > yzyx;
			Swizzler< T, 1, 2, 1, 1 > yzyy;
			Swizzler< T, 1, 2, 1, 2 > yzyz;
			Swizzler< T, 1, 2, 1, 3 > yzyw;
			Swizzler< T, 1, 2, 2, 0 > yzzx;
			Swizzler< T, 1, 2, 2, 1 > yzzy;
			Swizzler< T, 1, 2, 2, 2 > yzzz;
			Swizzler< T, 1, 2, 2, 3 > yzzw;
			Swizzler< T, 1, 2, 3, 0 > yzwx;
			Swizzler< T, 1, 2, 3, 1 > yzwy;
			Swizzler< T, 1, 2, 3, 2 > yzwz;
			Swizzler< T, 1, 2, 3, 3 > yzww;
			Swizzler< T, 1, 3, 0, 0 > ywxx;
			Swizzler< T, 1, 3, 0, 1 > ywxy;
			Swizzler< T, 1, 3, 0, 2 > ywxz;
			Swizzler< T, 1, 3, 0, 3 > ywxw;
			Swizzler< T, 1, 3, 1, 0 > ywyx;
			Swizzler< T, 1, 3, 1, 1 > ywyy;
			Swizzler< T, 1, 3, 1, 2 > ywyz;
			Swizzler< T, 1, 3, 1, 3 > ywyw;
			Swizzler< T, 1, 3, 2, 0 > ywzx;
			Swizzler< T, 1, 3, 2, 1 > ywzy;
			Swizzler< T, 1, 3, 2, 2 > ywzz;
			Swizzler< T, 1, 3, 2, 3 > ywzw;
			Swizzler< T, 1, 3, 3, 0 > ywwx;
			Swizzler< T, 1, 3, 3, 1 > ywwy;
			Swizzler< T, 1, 3, 3, 2 > ywwz;
			Swizzler< T, 1, 3, 3, 3 > ywww;
			Swizzler< T, 2, 0, 0, 0 > zxxx;
			Swizzler< T, 2, 0, 0, 1 > zxxy;
			Swizzler< T, 2, 0, 0, 2 > zxxz;
			Swizzler< T, 2, 0, 0, 3 > zxxw;
			Swizzler< T, 2, 0, 1, 0 > zxyx;
			Swizzler< T, 2, 0, 1, 1 > zxyy;
			Swizzler< T, 2, 0, 1, 2 > zxyz;
			Swizzler< T, 2, 0, 1, 3 > zxyw;
			Swizzler< T, 2, 0, 2, 0 > zxzx;
			Swizzler< T, 2, 0, 2, 1 > zxzy;
			Swizzler< T, 2, 0, 2, 2 > zxzz;
			Swizzler< T, 2, 0, 2, 3 > zxzw;
			Swizzler< T, 2, 0, 3, 0 > zxwx;
			Swizzler< T, 2, 0, 3, 1 > zxwy;
			Swizzler< T, 2, 0, 3, 2 > zxwz;
			Swizzler< T, 2, 0, 3, 3 > zxww;
			Swizzler< T, 2, 1, 0, 0 > zyxx;
			Swizzler< T, 2, 1, 0, 1 > zyxy;
			Swizzler< T, 2, 1, 0, 2 > zyxz;
			Swizzler< T, 2, 1, 0, 3 > zyxw;
			Swizzler< T, 2, 1, 1, 0 > zyyx;
			Swizzler< T, 2, 1, 1, 1 > zyyy;
			Swizzler< T, 2, 1, 1, 2 > zyyz;
			Swizzler< T, 2, 1, 1, 3 > zyyw;
			Swizzler< T, 2, 1, 2, 0 > zyzx;
			Swizzler< T, 2, 1, 2, 1 > zyzy;
			Swizzler< T, 2, 1, 2, 2 > zyzz;
			Swizzler< T, 2, 1, 2, 3 > zyzw;
			Swizzler< T, 2, 1, 3, 0 > zywx;
			Swizzler< T, 2, 1, 3, 1 > zywy;
			Swizzler< T, 2, 1, 3, 2 > zywz;
			Swizzler< T, 2, 1, 3, 3 > zyww;
			Swizzler< T, 2, 2, 0, 0 > zzxx;
			Swizzler< T, 2, 2, 0, 1 > zzxy;
			Swizzler< T, 2, 2, 0, 2 > zzxz;
			Swizzler< T, 2, 2, 0, 3 > zzxw;
			Swizzler< T, 2, 2, 1, 0 > zzyx;
			Swizzler< T, 2, 2, 1, 1 > zzyy;
			Swizzler< T, 2, 2, 1, 2 > zzyz;
			Swizzler< T, 2, 2, 1, 3 > zzyw;
			Swizzler< T, 2, 2, 2, 0 > zzzx;
			Swizzler< T, 2, 2, 2, 1 > zzzy;
			Swizzler< T, 2, 2, 2, 2 > zzzz;
			Swizzler< T, 2, 2, 2, 3 > zzzw;
			Swizzler< T, 2, 2, 3, 0 > zzwx;
			Swizzler< T, 2, 2, 3, 1 > zzwy;
			Swizzler< T, 2, 2, 3, 2 > zzwz;
			Swizzler< T, 2, 2, 3, 3 > zzww;
			Swizzler< T, 2, 3, 0, 0 > zwxx;
			Swizzler< T, 2, 3, 0, 1 > zwxy;
			Swizzler< T, 2, 3, 0, 2 > zwxz;
			Swizzler< T, 2, 3, 0, 3 > zwxw;
			Swizzler< T, 2, 3, 1, 0 > zwyx;
			Swizzler< T, 2, 3, 1, 1 > zwyy;
			Swizzler< T, 2, 3, 1, 2 > zwyz;
			Swizzler< T, 2, 3, 1, 3 > zwyw;
			Swizzler< T, 2, 3, 2, 0 > zwzx;
			Swizzler< T, 2, 3, 2, 1 > zwzy;
			Swizzler< T, 2, 3, 2, 2 > zwzz;
			Swizzler< T, 2, 3, 2, 3 > zwzw;
			Swizzler< T, 2, 3, 3, 0 > zwwx;
			Swizzler< T, 2, 3, 3, 1 > zwwy;
			Swizzler< T, 2, 3, 3, 2 > zwwz;
			Swizzler< T, 2, 3, 3, 3 > zwww;
			Swizzler< T, 3, 0, 0, 0 > wxxx;
			Swizzler< T, 3, 0, 0, 1 > wxxy;
			Swizzler< T, 3, 0, 0, 2 > wxxz;
			Swizzler< T, 3, 0, 0, 3 > wxxw;
			Swizzler< T, 3, 0, 1, 0 > wxyx;
			Swizzler< T, 3, 0, 1, 1 > wxyy;
			Swizzler< T, 3, 0, 1, 2 > wxyz;
			Swizzler< T, 3, 0, 1, 3 > wxyw;
			Swizzler< T, 3, 0, 2, 0 > wxzx;
			Swizzler< T, 3, 0, 2, 1 > wxzy;
			Swizzler< T, 3, 0, 2, 2 > wxzz;
			Swizzler< T, 3, 0, 2, 3 > wxzw;
			Swizzler< T, 3, 0, 3, 0 > wxwx;
			Swizzler< T, 3, 0, 3, 1 > wxwy;
			Swizzler< T, 3, 0, 3, 2 > wxwz;
			Swizzler< T, 3, 0, 3, 3 > wxww;
			Swizzler< T, 3, 1, 0, 0 > wyxx;
			Swizzler< T, 3, 1, 0, 1 > wyxy;
			Swizzler< T, 3, 1, 0, 2 > wyxz;
			Swizzler< T, 3, 1, 0, 3 > wyxw;
			Swizzler< T, 3, 1, 1, 0 > wyyx;
			Swizzler< T, 3, 1, 1, 1 > wyyy;
			Swizzler< T, 3, 1, 1, 2 > wyyz;
			Swizzler< T, 3, 1, 1, 3 > wyyw;
			Swizzler< T, 3, 1, 2, 0 > wyzx;
			Swizzler< T, 3, 1, 2, 1 > wyzy;
			Swizzler< T, 3, 1, 2, 2 > wyzz;
			Swizzler< T, 3, 1, 2, 3 > wyzw;
			Swizzler< T, 3, 1, 3, 0 > wywx;
			Swizzler< T, 3, 1, 3, 1 > wywy;
			Swizzler< T, 3, 1, 3, 2 > wywz;
			Swizzler< T, 3, 1, 3, 3 > wyww;
			Swizzler< T, 3, 2, 0, 0 > wzxx;
			Swizzler< T, 3, 2, 0, 1 > wzxy;
			Swizzler< T, 3, 2, 0, 2 > wzxz;
			Swizzler< T, 3, 2, 0, 3 > wzxw;
			Swizzler< T, 3, 2, 1, 0 > wzyx;
			Swizzler< T, 3, 2, 1, 1 > wzyy;
			Swizzler< T, 3, 2, 1, 2 > wzyz;
			Swizzler< T, 3, 2, 1, 3 > wzyw;
			Swizzler< T, 3, 2, 2, 0 > wzzx;
			Swizzler< T, 3, 2, 2, 1 > wzzy;
			Swizzler< T, 3, 2, 2, 2 > wzzz;
			Swizzler< T, 3, 2, 2, 3 > wzzw;
			Swizzler< T, 3, 2, 3, 0 > wzwx;
			Swizzler< T, 3, 2, 3, 1 > wzwy;
			Swizzler< T, 3, 2, 3, 2 > wzwz;
			Swizzler< T, 3, 2, 3, 3 > wzww;
			Swizzler< T, 3, 3, 0, 0 > wwxx;
			Swizzler< T, 3, 3, 0, 1 > wwxy;
			Swizzler< T, 3, 3, 0, 2 > wwxz;
			Swizzler< T, 3, 3, 0, 3 > wwxw;
			Swizzler< T, 3, 3, 1, 0 > wwyx;
			Swizzler< T, 3, 3, 1, 1 > wwyy;
			Swizzler< T, 3, 3, 1, 2 > wwyz;
			Swizzler< T, 3, 3, 1, 3 > wwyw;
			Swizzler< T, 3, 3, 2, 0 > wwzx;
			Swizzler< T, 3, 3, 2, 1 > wwzy;
			Swizzler< T, 3, 3, 2, 2 > wwzz;
			Swizzler< T, 3, 3, 2, 3 > wwzw;
			Swizzler< T, 3, 3, 3, 0 > wwwx;
			Swizzler< T, 3, 3, 3, 1 > wwwy;
			Swizzler< T, 3, 3, 3, 2 > wwwz;
			Swizzler< T, 3, 3, 3, 3 > wwww;
		} swizzle;
	};

	Vector() = default;

	template < typename U, typename = std::enable_if_t< std::is_arithmetic_v< U > > >
	Vector( U a_Scalar )
	{
		for ( size_t i = 0; i < 4; ++i )
		{
			Data[ i ] = static_cast< T >( a_Scalar );
		}
	}

	template < typename U >
	Vector( U a_X, U a_Y, U a_Z, U a_W )
		: x( static_cast< T >( a_X ) )
		, y( static_cast< T >( a_Y ) )
		, z( static_cast< T >( a_Z ) )
		, w( static_cast< T >( a_W ) )
	{ }

	template < typename U, size_t S0, size_t I0, typename... V >
	Vector( const IVector< U, S0, I0 >& a_Vector, V&&... a_Trailing )
	{
		size_t Size = Math::Min( S0, static_cast< size_t >( 3 ) );
		size_t i = 0;
		auto Trailing = std::forward_as_tuple( a_Trailing... );

		for ( ; i < Size; ++i )
		{
			this->operator[]( i ) = static_cast< T >( a_Vector[ i ] );
		}

		if constexpr ( S0 < 4 && std::tuple_size_v< decltype( Trailing ) > > 0 )
		{
			++i;
			this->operator[]( S0 ) = static_cast< T >( std::get< 0 >( Trailing ) );
		}

		if constexpr ( S0 < 3 && std::tuple_size_v< decltype( Trailing ) > > 1 )
		{
			++i;
			this->operator[]( S0 + 1 ) = static_cast< T >( std::get< 1 >( Trailing ) );
		}

		if constexpr ( S0 < 2 && std::tuple_size_v< decltype( Trailing ) > > 2 )
		{
			++i;
			this->operator[]( S0 + 2 ) = static_cast< T >( std::get< 2 >( Trailing ) );
		}

		if constexpr ( S0 < 1 && std::tuple_size_v< decltype( Trailing ) > > 3 )
		{
			++i;
			this->operator[]( S0 + 3 ) = static_cast< T >( std::get< 3 >( Trailing ) );
		}

		for ( ; i < 4; ++i )
		{
			this->operator[]( i ) = static_cast< T >( 0 );
		}
	}

	inline Vector< T, 2 > ToVector2() const
	{
		return { x, y };
	}

	inline Vector< T, 3 > ToVector3() const
	{
		return { x, y, z };
	}

	static const Vector< T, 4 > Zero;
	static const Vector< T, 4 > One;
};

template < typename T > const Vector< T, 4 > Vector< T, 4 >::Zero( 0, 0, 0, 0 );
template < typename T > const Vector< T, 4 > Vector< T, 4 >::One ( 1, 1, 1, 1 );

typedef Vector< float, 2 > Vector2;
typedef Vector< float, 3 > Vector3;
typedef Vector< float, 4 > Vector4;
typedef Vector< int, 2 > Vector2Int;
typedef Vector< int, 3 > Vector3Int;
typedef Vector< int, 4 > Vector4Int;

template < typename T, size_t M, size_t N >
struct Matrix;

template < typename T, size_t M, size_t N = M >
struct IMatrix
{
	inline auto& operator[]( size_t a_Index )
	{
		return reinterpret_cast< T* >( this )[ a_Index ];
	}

	inline const auto& operator[]( size_t a_Index ) const
	{
		return reinterpret_cast< const T* >( this )[ a_Index ];
	}

	template < typename U >
	IMatrix< T, M, N >& operator=( const IMatrix< U, M, N >& a_Matrix )
	{
		for ( size_t i = 0; i < M * N; ++i )
		{
			operator[]( i ) = static_cast< T >( a_Matrix[ i ] );
		}

		return *this;
	}

	inline IVector< T, N >& GetRow( size_t a_Row )
	{
		return reinterpret_cast< IVector< T, N >& >( reinterpret_cast< T* >( this )[ a_Row * N ] );
	}

	inline const IVector< T, N >& GetRow( size_t a_Row ) const
	{
		return reinterpret_cast< const IVector< T, N >& >( reinterpret_cast< const T* >( this )[ a_Row * N ] );
	}

	inline IVector< T, M, N >& GetCol( size_t a_Col )
	{
		return reinterpret_cast< IVector< T, M, N >& >( reinterpret_cast< T* >( this )[ a_Col ] );
	}

	inline const IVector< T, M, N >& GetCol( size_t a_Col ) const
	{
		return reinterpret_cast< const IVector< T, M, N >& >( reinterpret_cast< const T* >( this )[ a_Col ] );
	}

	constexpr size_t GetRows() const
	{
		return M;
	}

	constexpr size_t GetCols() const
	{
		return N;
	}

	Matrix< T, M, N > ToMatrix() const
	{
		return reinterpret_cast< Matrix< T, M, N >&& >( std::forward< Vector< T, M * N >&& >( this->ToVector() ) );
	}

	template < size_t NewM, size_t NewN >
	Matrix< T, NewM, NewN > ToMatrixMN() const
	{
		Matrix< T, NewM, NewN > Result;

		for ( size_t m = 0; m < NewM; ++m )
		{
			auto& Row = Result.GetRow( m );

			if ( m >= M )
			{
				for ( size_t n = 0; n < NewN; ++n )
				{
					Row[ n ] = 0;
				}
			}
			else
			{
				auto& SubRow = GetRow( m );
				size_t n = 0;
				size_t max = NewN < N ? NewN : N;

				for ( ; n < max; ++n )
				{
					Row[ n ] = SubRow[ n ];
				}

				for ( ; n < NewN; ++n )
				{
					Row[ n ] = 0;
				}
			}
		}

		return Result;
	}

	template < size_t NewM, size_t NewN >
	Matrix< T, NewM, NewN > ToMatrixMN( size_t a_OriginM, size_t a_OriginN ) const
	{
		Matrix< T, NewM, NewN > Result;
		size_t SubM = 0;

		for ( size_t m = 0; m < NewM; ++m )
		{
			auto& Row = Result.GetRow( m );

			if ( m < a_OriginM || m >= a_OriginM + M )
			{
				for ( size_t n = 0; n < NewN; ++n )
				{
					Row[ n ] = 0;
				}
			}
			else
			{
				size_t n = 0;

				for ( ; n < a_OriginN; ++n )
				{
					Row[ n ] = 0;
				}

				auto& SubRow = GetRow( SubM );

				for ( size_t SubN = 0; n < a_OriginN + N; ++n, ++SubN )
				{
					Row[ n ] = SubRow[ SubN ];
				}

				for ( ; n < NewN; ++n )
				{
					Row[ n ] = 0;
				}

				++SubM;
			}
		}

		return Result;
	}

	inline Matrix< T, 2, 2 > ToMatrix2() const
	{
		return ToMatrixMN< 2, 2 >();
	}

	inline Matrix< T, 3, 3 > ToMatrix3() const
	{
		return ToMatrixMN< 3, 3 >();
	}

	inline Matrix< T, 4, 4 > ToMatrix4() const
	{
		return ToMatrixMN< 4, 4 >();
	}
};

template < typename T, typename U, size_t M, size_t N >
Matrix< T, M, N > operator+( const IMatrix< T, M, N >& a_MatrixA, const IMatrix< U, M, N >& a_MatrixB )
{
	Matrix< T, M, N > Result;

	for ( size_t i = 0; i < M * N; ++i )
	{
		Result[ i ] = a_MatrixA[ i ] + static_cast< T >( a_MatrixB[ i ] );
	}

	return Result;
}

template < typename T, size_t M, size_t N >
Matrix< T, M, N > operator+( const IMatrix< T, M, N >& a_Matrix )
{
	return reinterpret_cast< const Matrix< T, M, N >& >( a_Matrix );
}

template < typename T, typename U, size_t M, size_t N >
Matrix< T, M, N >& operator+=( IMatrix< T, M, N >& a_MatrixA, const IMatrix< U, M, N >& a_MatrixB )
{
	for ( size_t i = 0; i < M * N; ++i )
	{
		a_MatrixA[ i ] += static_cast< T >( a_MatrixB[ i ] );
	}

	return reinterpret_cast< Matrix< T, M, N >& >( a_MatrixA );
}

template < typename T, typename U, size_t M, size_t N >
Matrix< T, M, N > operator-( const IMatrix< T, M, N >& a_MatrixA, const IMatrix< U, M, N >& a_MatrixB )
{
	Matrix< T, M, N > Result;

	for ( int i = 0; i < M * N; ++i )
	{
		Result[ i ] = a_MatrixA[ i ] - static_cast< T >( a_MatrixB[ i ] );
	}

	return Result;
}

template < typename T, size_t M, size_t N >
Matrix< T, M, N > operator-( const IMatrix< T, M, N >& a_Matrix )
{
	Matrix< T, M, N > Result;

	for ( size_t i = 0; i < M * N; ++i )
	{
		Result[ i ] = -a_Matrix[ i ];
	}

	return Result;
}

template < typename T, typename U, size_t M, size_t N >
Matrix< T, M, N >& operator-=( IMatrix< T, M, N >& a_MatrixA, const IMatrix< U, M, N >& a_MatrixB )
{
	for ( size_t i = 0; i < M * N; ++i )
	{
		a_MatrixA[ i ] -= static_cast< T >( a_MatrixB[ i ] );
	}

	return reinterpret_cast< Matrix< T, M, N >& >( a_MatrixA );
}

template < typename T, typename U, size_t M, size_t N >
Matrix< T, M, N > operator*( const IMatrix< T, M, N >& a_MatrixA, const IMatrix< U, M, N >& a_MatrixB )
{
	Matrix< T, M, N > Result;

	for ( size_t i = 0; i < M * N; ++i )
	{
		Result[ i ] = a_MatrixA[ i ] * static_cast< T >( a_MatrixB[ i ] );
	}

	return Result;
}

template < typename T, typename U, size_t M, size_t N >
Matrix< T, M, N >& operator*=( IMatrix< T, M, N >& a_MatrixA, const IMatrix< U, M, N >& a_MatrixB )
{
	for ( size_t i = 0; i < M * N; ++i )
	{
		a_MatrixA[ i ] *= static_cast< T >( a_MatrixB[ i ] );
	}

	return reinterpret_cast< Matrix< T, M, N >& >( a_MatrixA );
}

template < typename T, typename U, size_t M, size_t N, typename = std::enable_if_t< std::is_arithmetic_v< U > > >
Matrix< T, M, N > operator*( const IMatrix< T, M, N >& a_Matrix, U a_Scalar )
{
	Matrix< T, M, N > Result;

	for ( size_t i = 0; i < M * N; ++i )
	{
		Result[ i ] = a_Matrix[ i ] * static_cast< T >( a_Scalar );
	}

	return Result;
}

template < typename T, typename U, size_t M, size_t N, typename = std::enable_if_t< std::is_arithmetic_v< U > > >
Matrix< T, M, N > operator*( T a_Scalar, const IMatrix< U, M, N >& a_Matrix )
{
	Matrix< T, M, N > Result;

	for ( size_t i = 0; i < M * N; ++i )
	{
		Result[ i ] = a_Scalar * static_cast< T >( a_Matrix[ i ] );
	}

	return Result;
}

template < typename T, typename U, size_t M, size_t N, typename = std::enable_if_t< std::is_arithmetic_v< U > > >
Matrix< T, M, N >& operator*=( IMatrix< T, M, N >& a_Matrix, U a_Scalar )
{
	for ( size_t i = 0; i < M * N; ++i )
	{
		a_Matrix[ i ] *= static_cast< T >( a_Scalar );
	}

	return reinterpret_cast< Matrix< T, M, N >& >( a_Matrix );
}

template < typename T, typename U, size_t M, size_t N,typename = std::enable_if_t< std::is_arithmetic_v< U > > >
Matrix< T, M, N > operator/( const IMatrix< T, M, N >& a_Matrix, U a_Scalar )
{
	Matrix< T, M, N > Result;
	float Scalar = 1.0f / a_Scalar;

	for ( size_t i = 0; i < M * N; ++i )
	{
		Result[ i ] = a_Matrix[ i ] * Scalar;
	}

	return Result;
}

template < typename T, typename U, size_t M, size_t N, typename = std::enable_if_t< std::is_arithmetic_v< U > > >
Matrix< T, M, N >& operator/=( IMatrix< T, M, N >& a_Matrix, U a_Scalar )
{
	float Scalar = 1.0f / a_Scalar;

	for ( size_t i = 0; i < M * N; ++i )
	{
		a_Matrix[ i ] *= Scalar;
	}

	return reinterpret_cast< Matrix< T, M, N >& >( a_Matrix );
}

template < typename T, typename U, size_t M, size_t N >
bool operator==( const Matrix< T, M, N >& a_MatrixA, const Matrix< U, M, N >& a_MatrixB )
{
	for ( size_t i = 0; i < M * N; ++i )
	{
		if ( a_MatrixA[ i ] != static_cast< T >( a_MatrixB[ i ] ) )
		{
			return false;
		}
	}

	return true;
}

template < typename T, typename U, size_t M, size_t N >
inline bool operator!=( const Matrix< T, M, N >& a_MatrixA, const Matrix< U, M, N >& a_MatrixB )
{
	return !( a_MatrixA == a_MatrixB );
}

template < typename T, size_t M, size_t N >
struct Matrix : public IMatrix< T, M, N >
{
	union
	{
		T Data[ M * N ];
		IVector< T, ( M < N ? M : N ), N + 1 > diagonal;
	};
	
	Matrix() = default;

	template < typename U, typename = std::enable_if_t< std::is_arithmetic_v< U > > >
	Matrix( U a_Scalar )
	{
		for ( size_t i = 0; i < M * N; ++i )
		{
			Data[ i ] = static_cast< T >( a_Scalar );
		}
	}

	template < typename U, typename = std::enable_if_t< std::is_arithmetic_v< U > > >
	Matrix( std::initializer_list< U >&& a_InitializerList )
	{
		size_t Size = M * N < a_InitializerList.size() ? M * N : a_InitializerList.size();
		auto Begin = a_InitializerList.begin();

		for ( size_t i = 0; i < Size; ++i, ++Begin )
		{
			Data[ i ] = static_cast< T >( *Begin );
		}
	}

	static const Matrix< T, M, N > Zero;
	static const Matrix< T, M, N > One;
	static const Matrix< T, M, N > Identity;
};

template < typename T, size_t M, size_t N > const Matrix< T, M, N > Matrix< T, M, N >::Zero = Matrix< T, M, N >( 0 );
template < typename T, size_t M, size_t N > const Matrix< T, M, N > Matrix< T, M, N >::One  = Matrix< T, M, N >( 1 );
template < typename T, size_t M, size_t N > const Matrix< T, M, N > Matrix< T, M, N >::Identity = []()
{ 
	Matrix< T, M, N > Result( 0 );
	auto& Diagonal = Result.diagonal;

	for ( int i = 0; i < Diagonal.GetDimensions(); ++i )
	{
		Diagonal[ i ] = static_cast< T >( 1 );
	}

	return Result;
}();

template < typename T >
struct Matrix< T, 2 > : IMatrix< T, 2 >
{
	union
	{
		T Data[ 4 ];

		struct
		{
			T x0; T x1;
			T y0; T y1;
		};

		IVector< T, 2, 3 > diagonal;
		Swizzler< T, 0, 2, 1, 3 > transpose;

		IVector< T, 2, 1 > r0;
		
		struct
		{
			T x0; T x1;
			IVector< T, 2, 1 > r1;
		};

		IVector< T, 2, 2 > c0;
		
		struct
		{
			T x0;
			IVector< T, 2, 2 > c1;
		};
	};

	Matrix() = default;

	template < typename U, typename = std::enable_if_t< std::is_arithmetic_v< U > > >
	Matrix( U a_Scalar )
	{
		for ( size_t i = 0; i < 4; ++i )
		{
			Data[ i ] = static_cast< T >( a_Scalar );
		}
	}

	template < typename U >
	Matrix( U a_X0, U a_Y0, U a_X1, U a_Y1 )
		: x0( static_cast< T >( a_X0 ) )
		, y0( static_cast< T >( a_Y0 ) )
		, x1( static_cast< T >( a_X1 ) )
		, y1( static_cast< T >( a_Y1 ) )
	{ }

	template < typename U, typename V, size_t S0, size_t S1, size_t I0, size_t I1 >
	Matrix( const IVector< U, S0, I0 >& a_VectorX, const IVector< V, S1, I1 >& a_VectorY )
	{
		c0 = a_VectorX;
		c1 = a_VectorY;
	}

	template < typename U, typename = std::enable_if_t< std::is_arithmetic_v< U > > >
	Matrix( std::initializer_list< U > && a_InitializerList )
	{
		size_t Size = 4 < a_InitializerList.size() ? 4 : a_InitializerList.size();
		auto Begin = a_InitializerList.begin();

		for ( size_t i = 0; i < Size; ++i, ++Begin )
		{
			Data[ i ] = static_cast< T >( *Begin );
		}
	}

	static const Matrix< T, 2 > Zero;
	static const Matrix< T, 2 > One;
	static const Matrix< T, 2 > Identity;
};

template < typename T > const Matrix< T, 2 > Matrix< T, 2 >::Zero = Matrix< T, 2 >( 0 );
template < typename T > const Matrix< T, 2 > Matrix< T, 2 >::One  = Matrix< T, 2 >( 1 );
template < typename T > const Matrix< T, 2 > Matrix< T, 2 >::Identity = []()
{
	Matrix< T, 2 > Result( 0 );
	auto& Diagonal = Result.diagonal;

	for ( int i = 0; i < 2; ++i )
	{
		Diagonal[ i ] = static_cast< T >( 1 );
	}

	return Result;
}( );

template < typename T >
struct Matrix< T, 3 > : IMatrix< T, 3 >
{
	union
	{
		T Data[ 9 ];

		struct
		{
			T x0; T x1; T x2;
			T y0; T y1; T y2;
			T z0; T z1; T z2;
		};

		IVector< T, 3, 4 > diagonal;
		Swizzler< T, 0, 3, 6, 1, 4, 7, 2, 5, 8 > transpose;

		IVector< T, 3, 1 > r0;

		struct
		{
			T x0; T x1; T x2;
			IVector< T, 3, 1 > r1;
		};

		struct
		{
			T x0; T x1; T x2;
			T y0; T y1; T y2;
			IVector< T, 3, 1 > r2;
		};

		IVector< T, 3, 3 > c0;

		struct
		{
			T x0;
			IVector< T, 3, 3 > c1;
		};

		struct
		{
			T x0; T x1;
			IVector< T, 3, 3 > c2;
		};
	};

	Matrix() = default;

	template < typename U, typename = std::enable_if_t< std::is_arithmetic_v< U > > >
	Matrix( U a_Scalar )
	{
		for ( size_t i = 0; i < 9; ++i )
		{
			Data[ i ] = static_cast< T >( a_Scalar );
		}
	}

	template < typename U >
	Matrix( U a_X0, U a_Y0, U a_Z0, U a_X1, U a_Y1, U a_Z1, U a_X2, U a_Y2, U a_Z2 )
		: x0( static_cast< T >( a_X0 ) )
		, y0( static_cast< T >( a_Y0 ) )
		, z0( static_cast< T >( a_Z0 ) )
		, x1( static_cast< T >( a_X1 ) )
		, y1( static_cast< T >( a_Y1 ) )
		, z1( static_cast< T >( a_Z1 ) )
		, x2( static_cast< T >( a_X2 ) )
		, y2( static_cast< T >( a_Y2 ) )
		, z2( static_cast< T >( a_Z2 ) )
	{ }

	template < typename U, typename V, typename W, size_t S0, size_t S1, size_t S2, size_t I0, size_t I1, size_t I2 >
	Matrix( const IVector< U, S0, I0 >& a_VectorX, const IVector< V, S1, I1 >& a_VectorY, const IVector< W, S2, I2 >& a_VectorZ )
	{
		c0 = a_VectorX;
		c1 = a_VectorY;
		c2 = a_VectorZ;
	}

	template < typename U, typename = std::enable_if_t< std::is_arithmetic_v< U > > >
	Matrix( std::initializer_list< U >&& a_InitializerList )
	{
		size_t Size = 9 < a_InitializerList.size() ? 9 : a_InitializerList.size();
		auto Begin = a_InitializerList.begin();

		for ( size_t i = 0; i < Size; ++i, ++Begin )
		{
			Data[ i ] = static_cast< T >( *Begin );
		}
	}

	template < typename U >
	inline static Matrix< T, 3 > CreateTranslation( const Vector< U, 2 >& a_Vector )
	{
		return Matrix< T, 3 >(
			static_cast< T >( 1 ),
			static_cast< T >( 0 ),
			static_cast< T >( 0 ),
			static_cast< T >( 0 ),
			static_cast< T >( 1 ),
			static_cast< T >( 0 ),
			static_cast< T >( a_Vector.x ),
			static_cast< T >( a_Vector.y ),
			static_cast< T >( 1 ) );
	}

	template < typename U, typename V >
	inline static void Translate( Matrix< U, 3 >& a_Matrix, const Vector< V, 2 >& a_Vector )
	{
		a_Matrix.x2 += static_cast< U >( a_Vector.x );
		a_Matrix.y2 += static_cast< U >( a_Vector.y );
	}

	template < typename U >
	static Matrix< T, 3 > CreateRotation( U a_Radians )
	{
		U C = Math::Cos( a_Radians );
		U S = Math::Sin( a_Radians );

		return Matrix< T, 3 >(
			static_cast< T >( C ),
			static_cast< T >( -S ),
			static_cast< T >( 0 ),
			static_cast< T >( S ),
			static_cast< T >( C ),
			static_cast< T >( 0 ),
			static_cast< T >( 0 ),
			static_cast< T >( 0 ),
			static_cast< T >( 1 ) );
	}

	template < typename U, typename V >
	inline static void Rotate( Matrix< U, 3 >& a_Matrix, V a_Radians )
	{
		a_Matrix = Math::Multiply( CreateRotation( a_Radians ), a_Matrix );
	}

	template < typename U >
	inline static Matrix< T, 3 > CreateScale( const Vector< U, 2 >& a_Vector )
	{
		return Matrix< T, 3 >(
			static_cast< T >( a_Vector.x ),
			static_cast< T >( 0 ),
			static_cast< T >( 0 ),
			static_cast< T >( 0 ),
			static_cast< T >( a_Vector.y ),
			static_cast< T >( 0 ),
			static_cast< T >( 0 ),
			static_cast< T >( 0 ),
			static_cast< T >( 1 ) );
	}

	template < typename U, typename V >
	inline static void Scale( Matrix< U, 3 >& a_Matrix, const Vector< V, 2 >& a_Vector )
	{
		a_Matrix.x0 *= static_cast< U >( a_Vector.x );
		a_Matrix.y1 *= static_cast< U >( a_Vector.y );
	}

	template < typename U, typename V, typename W >
	inline static Matrix< T, 3 > CreateTransform( const Vector< U, 2 >& a_Translation, V a_Rotation, const Vector< W, 2 >& a_Scale )
	{
		Matrix< T, 3 > Result = CreateScale( a_Scale );
		Rotate( Result, a_Rotation );
		Translate( Result, a_Translation );
		return Result;
	}

	template < typename U, typename V, typename W, typename X >
	inline static void Transform( Matrix< U, 3 >& a_Matrix, const Vector< U, 2 >& a_Translation, V a_Rotation, const Vector< W, 2 >& a_Scale )
	{
		Scale( a_Matrix, a_Scale );
		Rotate( a_Matrix, a_Rotation );
		Translate( a_Matrix, a_Translation );
	}

	static const Matrix< T, 3 > Zero;
	static const Matrix< T, 3 > One;
	static const Matrix< T, 3 > Identity;
};

template < typename T > const Matrix< T, 3 > Matrix< T, 3 >::Zero = Matrix< T, 3 >( 0 );
template < typename T > const Matrix< T, 3 > Matrix< T, 3 >::One  = Matrix< T, 3 >( 1 );
template < typename T > const Matrix< T, 3 > Matrix< T, 3 >::Identity = []()
{
	Matrix< T, 3 > Result( 0 );
	auto& Diagonal = Result.diagonal;

	for ( int i = 0; i < 3; ++i )
	{
		Diagonal[ i ] = static_cast< T >( 1 );
	}

	return Result;
}( );

template < typename T >
struct Matrix< T, 4 > : public IMatrix< T, 4 >
{
	union
	{
		T Data[ 16 ];

		struct
		{
			T x0; T x1; T x2; T x3;
			T y0; T y1; T y2; T y3;
			T z0; T z1; T z2; T z3;
			T w0; T w1; T w2; T w3;
		};

		IVector< T, 4, 5 > diagonal;
		Swizzler< T, 0, 4, 8, 12, 1, 5, 9, 13, 2, 6, 10, 14, 3, 7, 11, 15 > transpose;

		IVector< T, 4, 1 > r0;

		struct
		{
			T x0; T x1; T x2; T x3;
			IVector< T, 4, 1 > r1;
		};

		struct
		{
			T x0; T x1; T x2; T x3;
			T y0; T y1; T y2; T y3;
			IVector< T, 4, 1 > r2;
		};

		struct
		{
			T x0; T x1; T x2; T x3;
			T y0; T y1; T y2; T y3;
			T z0; T z1; T z2; T z3;
			IVector< T, 4, 1 > r3;
		};

		IVector< T, 4, 4 > c0;

		struct
		{
			T x0;
			IVector< T, 4, 4 > c1;
		};

		struct
		{
			T x0; T x1;
			IVector< T, 4, 4 > c2;
		};

		struct
		{
			T x0; T x1; T x2;
			IVector< T, 4, 4 > c3;
		};
	};

	Matrix() = default;

	template < typename U, typename = std::enable_if_t< std::is_arithmetic_v< U > > >
	Matrix( U a_Scalar )
	{
		for ( size_t i = 0; i < 16; ++i )
		{
			Data[ i ] = static_cast< T >( a_Scalar );
		}
	}

	template < typename U >
	Matrix( U a_X0, U a_Y0, U a_Z0, U a_W0, U a_X1, U a_Y1, U a_Z1, U a_W1, U a_X2, U a_Y2, U a_Z2, U a_W2, U a_X3, U a_Y3, U a_Z3, U a_W3 )
		: x0( static_cast< T >( a_X0 ) )
		, x1( static_cast< T >( a_X1 ) )
		, x2( static_cast< T >( a_X2 ) )
		, x3( static_cast< T >( a_X3 ) )
		, y0( static_cast< T >( a_Y0 ) )
		, y1( static_cast< T >( a_Y1 ) )
		, y2( static_cast< T >( a_Y2 ) )
		, y3( static_cast< T >( a_Y3 ) )
		, z0( static_cast< T >( a_Z0 ) )
		, z1( static_cast< T >( a_Z1 ) )
		, z2( static_cast< T >( a_Z2 ) )
		, z3( static_cast< T >( a_Z3 ) )
		, w0( static_cast< T >( a_W0 ) )
		, w1( static_cast< T >( a_W1 ) )
		, w2( static_cast< T >( a_W2 ) )
		, w3( static_cast< T >( a_W3 ) )
	{ }

	template < typename U, typename V, typename W, typename X, size_t S0, size_t S1, size_t S2, size_t S3, size_t I0, size_t I1, size_t I2, size_t I3 >
	Matrix( const IVector< U, S0, I0 >& a_VectorX, const IVector< V, S1, I1 >& a_VectorY, const IVector< W, S2, I2 >& a_VectorZ, const IVector< X, S3, I3 >& a_VectorW )
	{
		c0 = a_VectorX;
		c1 = a_VectorY;
		c2 = a_VectorZ;
		c3 = a_VectorW;
	}

	template < typename U, typename = std::enable_if_t< std::is_arithmetic_v< U > > >
	Matrix( std::initializer_list< U >&& a_InitializerList )
	{
		size_t Size = 16 < a_InitializerList.size() ? 16 : a_InitializerList.size();
		auto Begin = a_InitializerList.begin();

		for ( size_t i = 0; i < Size; ++i, ++Begin )
		{
			Data[ i ] = static_cast< T >( *Begin );
		}
	}

	template < typename U >
	inline static Matrix< T, 4 > CreateTranslation( const Vector< U, 3 >& a_Vector )
	{
		return Matrix< T, 4 >(
			static_cast< T >( 1 ),
			static_cast< T >( 0 ),
			static_cast< T >( 0 ),
			static_cast< T >( a_Vector.x ),
			static_cast< T >( 0 ),
			static_cast< T >( 1 ),
			static_cast< T >( 0 ),
			static_cast< T >( a_Vector.y ),
			static_cast< T >( 0 ),
			static_cast< T >( 0 ),
			static_cast< T >( 1 ),
			static_cast< T >( a_Vector.z ),
			static_cast< T >( 0 ),
			static_cast< T >( 0 ),
			static_cast< T >( 0 ),
			static_cast< T >( 1 ) );
	}

	template < typename U, typename V >
	inline static void Translate( Matrix< U, 4 >& a_Matrix, const Vector< V, 3 >& a_Vector )
	{
		a_Matrix.x3 += static_cast< U >( a_Vector.x );
		a_Matrix.y3 += static_cast< U >( a_Vector.y );
		a_Matrix.z3 += static_cast< U >( a_Vector.z );
	}

	template < typename U >
	static Matrix< T, 4 > CreateRotation( const Vector< U, 3 >& a_Vector, RotationOrder a_RotationOrder = RotationOrder::ZXY )
	{
		switch ( a_RotationOrder )
		{
		case RotationOrder::XYZ:
		{
			auto Rotation = CreateRotationX( a_Vector.x );
			RotateY( Rotation, a_Vector.y );
			RotateZ( Rotation, a_Vector.z );
			return Rotation;
		}
		case RotationOrder::XZY:
		{
			auto Rotation = CreateRotationX( static_cast< T >( a_Vector.x ) );
			RotateZ( Rotation, a_Vector.z );
			RotateY( Rotation, a_Vector.y );
			return Rotation; 
		}
		case RotationOrder::YXZ:
		{
			auto Rotation = CreateRotationY( static_cast< T >( a_Vector.y ) );
			RotateX( Rotation, a_Vector.x );
			RotateZ( Rotation, a_Vector.z );
			return Rotation; 
		}
		case RotationOrder::YZX:
		{
			auto Rotation = CreateRotationY( static_cast< T >( a_Vector.y ) );
			RotateZ( Rotation, a_Vector.z );
			RotateX( Rotation, a_Vector.x );
			return Rotation;
		}
		case RotationOrder::ZXY:
		{
			auto Rotation = CreateRotationZ( static_cast< T >( a_Vector.z ) );
			RotateX( Rotation, a_Vector.x );
			RotateY( Rotation, a_Vector.y );
			return Rotation;
		}
		case RotationOrder::ZYX:
		{
			auto Rotation = CreateRotationZ( static_cast< T >( a_Vector.z ) );
			RotateY( Rotation, a_Vector.y );
			RotateX( Rotation, a_Vector.x );
			return Rotation; 
		}
		}
	}

	template < typename U >
	static Matrix< T, 4 > CreateRotationX( U a_Radians )
	{
		U C = Math::Cos( a_Radians );
		U S = Math::Sin( a_Radians );

		return Matrix< T, 4 >( 
			static_cast< T >( 1 ), 
			static_cast< T >( 0 ), 
			static_cast< T >( 0 ), 
			static_cast< T >( 0 ), 
			static_cast< T >( 0 ), 
			static_cast< T >( C ),
			static_cast< T >( -S ),
			static_cast< T >( 0 ), 
			static_cast< T >( 0 ), 
			static_cast< T >( S ),
			static_cast< T >( C ),
			static_cast< T >( 0 ), 
			static_cast< T >( 0 ), 
			static_cast< T >( 0 ), 
			static_cast< T >( 0 ), 
			static_cast< T >( 1 ) );
	}

	template < typename U >
	static Matrix< T, 4 > CreateRotationY( U a_Radians )
	{
		U C = Math::Cos( a_Radians );
		U S = Math::Sin( a_Radians );

		return Matrix< T, 4 >(
			static_cast< T >( C ),
			static_cast< T >( 0 ),
			static_cast< T >( S ),
			static_cast< T >( 0 ),
			static_cast< T >( 0 ),
			static_cast< T >( 1 ),
			static_cast< T >( 0 ),
			static_cast< T >( 0 ),
			static_cast< T >( -S ),
			static_cast< T >( 0 ),
			static_cast< T >( C ),
			static_cast< T >( 0 ),
			static_cast< T >( 0 ),
			static_cast< T >( 0 ),
			static_cast< T >( 0 ),
			static_cast< T >( 1 ) );
	}

	template < typename U >
	static Matrix< T, 4 > CreateRotationZ( U a_Radians )
	{
		U C = Math::Cos( a_Radians );
		U S = Math::Sin( a_Radians );

		return Matrix< T, 4 >(
			static_cast< T >( C ),
			static_cast< T >( -S ),
			static_cast< T >( 0 ),
			static_cast< T >( 0 ),
			static_cast< T >( S ),
			static_cast< T >( C ),
			static_cast< T >( 0 ),
			static_cast< T >( 0 ),
			static_cast< T >( 0 ),
			static_cast< T >( 0 ),
			static_cast< T >( 1 ),
			static_cast< T >( 0 ),
			static_cast< T >( 0 ),
			static_cast< T >( 0 ),
			static_cast< T >( 0 ),
			static_cast< T >( 1 ) );
	}

	template < typename U, typename V >
	inline static void Rotate( Matrix< U, 4 >& a_Matrix, const Vector< V, 3 >& a_Vector, RotationOrder a_RotationOrder = RotationOrder::ZXY )
	{
		a_Matrix = Math::Multiply( CreateRotation( a_Vector, a_RotationOrder ), a_Matrix );
	}

	template < typename U, typename V >
	inline static void RotateX( Matrix< U, 4 >& a_Matrix, V a_Radians )
	{
		a_Matrix = Math::Multiply( CreateRotationX( a_Radians ), a_Matrix );
	}

	template < typename U, typename V >
	inline static void RotateY( Matrix< U, 4 >& a_Matrix, V a_Radians )
	{
		a_Matrix = Math::Multiply( CreateRotationY( a_Radians ), a_Matrix );
	}

	template < typename U, typename V >
	inline static void RotateZ( Matrix< U, 4 >& a_Matrix, V a_Radians )
	{
		a_Matrix = Math::Multiply( CreateRotationZ( a_Radians ), a_Matrix );
	}

	template < typename U >
	inline static Matrix< T, 4 > CreateScale( const Vector< U, 3 >& a_Vector )
	{
		return Matrix< T, 4 >(
			static_cast< T >( a_Vector.x ),
			static_cast< T >( 0 ),
			static_cast< T >( 0 ),
			static_cast< T >( 0 ),
			static_cast< T >( 0 ),
			static_cast< T >( a_Vector.y ),
			static_cast< T >( 0 ),
			static_cast< T >( 0 ),
			static_cast< T >( 0 ),
			static_cast< T >( 0 ),
			static_cast< T >( a_Vector.z ),
			static_cast< T >( 0 ),
			static_cast< T >( 0 ),
			static_cast< T >( 0 ),
			static_cast< T >( 0 ),
			static_cast< T >( 1 ) );
	}

	template < typename U, typename V >
	inline static void Scale( Matrix< U, 4 >& a_Matrix, const Vector< V, 3 >& a_Vector )
	{
		a_Matrix.x0 *= static_cast< U >( a_Vector.x );
		a_Matrix.y1 *= static_cast< U >( a_Vector.y );
		a_Matrix.z2 *= static_cast< U >( a_Vector.z );
	}

	template < typename U, typename V, typename W >
	inline static Matrix< T, 4 > CreateTransform( const Vector< U, 3 >& a_Translation, const Vector< V, 3 >& a_Rotation, const Vector< W, 3 >& a_Scale, RotationOrder a_RotationOrder = RotationOrder::ZXY )
	{
		auto Result = CreateScale( a_Scale );
		Rotate( Result, a_Rotation, a_RotationOrder );
		Translate( Result, a_Translation );
		return Result;
	}

	template < typename U, typename V, typename W, typename X >
	inline static void Transform( Matrix< U, 4 >& a_Matrix, const Vector< V, 3 >& a_Translation, const Vector< W, 3 >& a_Rotation, const Vector< X, 3 >& a_Scale, RotationOrder a_RotationOrder )
	{
		Scale( a_Matrix, a_Scale );
		Rotate( a_Matrix, a_Rotation, a_RotationOrder );
		Translate( a_Matrix, a_Translation );
	}

	template < typename U, typename V, typename W >
	inline static Matrix< T, 4 > CreateLookAt( const Vector< U, 3 >& a_Eye, const Vector< V, 3 >& a_Centre, const Vector< W, 3 >& a_Up )
	{
		auto Forward = Math::Normalize( a_Eye - a_Centre );
		auto Right = Math::Cross( a_Up, Forward );
		auto Up = Math::Cross( Forward, Right );

		return Matrix< T, 4 >(
			static_cast< T >( Right.x ),
			static_cast< T >( Up.x ),
			static_cast< T >( Forward.x ),
			static_cast< T >( -Math::Dot( Right, a_Eye ) ),
			static_cast< T >( Right.y ),
			static_cast< T >( Up.y ),
			static_cast< T >( Forward.y ),
			static_cast< T >( -Math::Dot( Up, a_Eye ) ),
			static_cast< T >( Right.z ),
			static_cast< T >( Up.z ),
			static_cast< T >( Forward.z ),
			static_cast< T >( -Math::Dot( Forward, a_Eye ) ),
			static_cast< T >( 0 ),
			static_cast< T >( 0 ),
			static_cast< T >( 0 ),
			static_cast< T >( 1 ) );
	}

	template < typename U, typename V >
	inline static Matrix< T, 4 > CreateView( const Vector< U, 3 >& a_Position, const Vector< V, 3 >& a_Rotation )
	{
		/*float cosPitch = Math::Cos( a_Rotation.x );
		float sinPitch = Math::Sin( a_Rotation.x );
		float cosYaw   = Math::Cos( a_Rotation.y );
		float sinYaw   = Math::Sin( a_Rotation.y );

		Vector3 AxisX( cosYaw, 0.0f, -sinYaw );
		Vector3 AxisY( sinYaw * sinPitch, cosPitch, cosYaw * sinPitch );
		Vector3 AxisZ( sinYaw * cosPitch, -sinPitch, cosPitch * cosYaw );
	
		return Matrix< T, 4 >(
			static_cast< T >( AxisX.x ),
			static_cast< T >( AxisY.x ),
			static_cast< T >( AxisZ.x ),
			static_cast< T >( 0 ),
			static_cast< T >( AxisX.y ),
			static_cast< T >( AxisY.y ),
			static_cast< T >( AxisZ.y ),
			static_cast< T >( 0 ),
			static_cast< T >( AxisX.z ),
			static_cast< T >( AxisY.z ),
			static_cast< T >( AxisZ.z ),
			static_cast< T >( 0 ),
			static_cast< T >( -Math::Dot( AxisX, a_Position ) ),
			static_cast< T >( -Math::Dot( AxisY, a_Position ) ),
			static_cast< T >( -Math::Dot( AxisZ, a_Position ) ),
			static_cast< T >( 1 ) );*/

		auto Result = CreateRotation( a_Rotation );
		Translate( Result, a_Position );
		return Math::Inverse( Result );

		/*auto Result = CreateTranslation( -a_Position );
		Rotate( Result, -a_Rotation, RotationOrder::YXZ );
		return Result;*/
	}
	
	inline static Matrix< T, 4 > CreateProjection( float a_FOV, float a_Aspect, float a_NearZ = 0.1f, float a_FarZ = 1000.0f )
	{
		float HalfCot = 1.0f / Math::Tan( a_FOV * 0.5f );

		return Matrix< T, 4 >(
			static_cast< T >( HalfCot / a_Aspect ),
			static_cast< T >( 0 ), 
			static_cast< T >( 0 ), 
			static_cast< T >( 0 ), 
			static_cast< T >( 0 ), 
			static_cast< T >( HalfCot ),
			static_cast< T >( 0 ), 
			static_cast< T >( 0 ), 
			static_cast< T >( 0 ), 
			static_cast< T >( 0 ), 
			static_cast< T >( ( a_FarZ + a_NearZ ) / ( a_FarZ - a_NearZ ) ),
			static_cast< T >( 1 ),
			static_cast< T >( 0 ), 
			static_cast< T >( 0 ), 
			static_cast< T >( 2.0f * a_FarZ * a_NearZ / ( a_NearZ - a_FarZ ) ), 
			static_cast< T >( 0 ) );
	}

	static const Matrix< T, 4 > Zero;
	static const Matrix< T, 4 > One;
	static const Matrix< T, 4 > Identity;
};

template < typename T > const Matrix< T, 4 > Matrix< T, 4 >::Zero = Matrix< T, 4 >( 0 );
template < typename T > const Matrix< T, 4 > Matrix< T, 4 >::One  = Matrix< T, 4 >( 1 );
template < typename T > const Matrix< T, 4 > Matrix< T, 4 >::Identity = []()
{
	Matrix< T, 4 > Result( 0 );
	auto& Diagonal = Result.diagonal;

	for ( int i = 0; i < 4; ++i )
	{
		Diagonal[ i ] = static_cast< T >( 1 );
	}

	return Result;
}( );

