/* matrix.h
**
**	Defines a matrix template. Abstract and simple.
** If you want [] to check bounds, define CHECK_BOUNDS.
** The lines frequently run over 80 chars, so turn off wrapping if you can.
** believe me, it looks much worse formatted to fit.
**
** includes:
**	mdefs.h		- for WORD
**	array2d.h	- base class
*/

#ifndef __MATRIX_H
#define __MATRIX_H

#include <mdefs.h>	 
#include <array2d.h>
#include <stdlib.h>
			  
typedef Array2dSizeType			MatrixSizeType;

template <class MatrixEl>
class Matrix : public Array2d<MatrixEl> {
public:
    INLINE_CTOR			Matrix (WORD h = 0, WORD w = 0);
    INLINE_CTOR			Matrix (const Matrix<MatrixEl>& AMatrix);
    INLINE_CTOR			Matrix (const MatrixEl * AMatrix, MatrixSizeType AHeight, MatrixSizeType AWidth);
			
    inline Matrix<MatrixEl>&		operator+ (const Matrix<MatrixEl>& toAdd) const;
    inline Matrix<MatrixEl>&		operator- (const Matrix<MatrixEl>& toSub) const;
    inline Matrix<MatrixEl>&		operator* (MatrixEl scalar) const;
    inline virtual Matrix<MatrixEl>&	operator* (const Matrix<MatrixEl>& toMul) const;
    
    inline void				Transpose (void);
    inline void				RowReduce (void);
    inline MatrixEl			Determinant (void);
    inline virtual void			ReadTextStream (istream& is);
    inline virtual void			WriteTextStream (ostream& os);
};

/*--------------------------------------------------------------------------*/

template <class MatrixEl>
inline Matrix<MatrixEl> :: Matrix 
(WORD h, WORD w) 
 : Array2d<MatrixEl> (h, w)
{   
    m_Width = w;
    m_Height = h; 
}

template <class MatrixEl>
inline Matrix<MatrixEl> :: Matrix
(const Matrix<MatrixEl>& AMatrix) : Array2d<MatrixEl> (AMatrix)
{   
    m_Width = AMatrix.m_Width;
    m_Height = AMatrix.m_Height;	   
};

template <class MatrixEl>
inline Matrix<MatrixEl> :: Matrix 
(const MatrixEl * AMatrix, MatrixSizeType AHeight, MatrixSizeType AWidth) 
 : Array2d<MatrixEl> (AMatrix, AHeight, AWidth)
{ 	   
    m_Width = AWidth;
    m_Height = AHeight;	   
};
 
template <class MatrixEl>
inline Matrix<MatrixEl>& Matrix<MatrixEl> :: operator+ 
(const Matrix<MatrixEl>& toAdd) const
{
MatrixSizeType row, col;				       
Matrix<MatrixEl> * sum;

    if (toAdd.m_Height != m_Height || toAdd.m_Width != m_Width) {
	cerr << "Add: matrices must have same dimentions.\n";
	exit (1);
    }
		
    sum = new Matrix<MatrixEl> (m_Height, m_Width);
    
    for (row = 0; row < m_Height; ++ row)
	for (col = 0; col < m_Width; ++ col)
	    sum->m_Data [row * sum->m_Width + col] = m_Data [row * m_Width + col] + toAdd.m_Data [row * toAdd.m_Width + col];
       
    return (*sum);
}

template <class MatrixEl>
inline Matrix<MatrixEl>& Matrix<MatrixEl> :: operator- 
(const Matrix<MatrixEl>& toSub) const
{
MatrixSizeType row, col;				       
Matrix<MatrixEl> * diff;

    if (toSub.m_Height != m_Height || toSub.m_Width != m_Width) {
	cerr << "Sub: matrices must have same dimentions.\n";
	exit (1);
    }
		
    diff = new Matrix<MatrixEl> (m_Height, m_Width);
    
    for (row = 0; row < m_Height; ++ row)
	for (col = 0; col < m_Width; ++ col)
	    diff->m_Data [row * diff->m_Width + col] = m_Data [row * m_Width + col] - toSub.m_Data [row * toSub.m_Width + col];
       
    return (*diff);
}

template <class MatrixEl>
inline Matrix<MatrixEl>& Matrix<MatrixEl> :: operator* 
(MatrixEl scalar) const
{
MatrixSizeType row, col;
Matrix<MatrixEl> * product;

    product = new Matrix<MatrixEl> (m_Height, m_Width);
    
    for (row = 0; row < m_Height; ++ row)
	for (col = 0; col < m_Width; ++ col)
	    product->m_Data [row * product->m_Width + col] = m_Data [row * m_Width + col] * scalar;
       
    return (*product);
}

template <class MatrixEl>
inline Matrix<MatrixEl>& Matrix<MatrixEl> :: operator* 
(const Matrix<MatrixEl>& toMul) const
{
MatrixSizeType row, col, idx;
Matrix<MatrixEl> * product;
MatrixSizeType ProductOffset, MyOffset, HisOffset;

    if (m_Width != toMul.m_Height) {
	cerr << "Mul: m_Width != m_Height.\n";
	exit (1);
    }
		
    product = new Matrix<MatrixEl> (m_Height, toMul.m_Width);
    
    ProductOffset = 0;
    MyOffset = 0;
    
    for (row = 0; row < m_Height; ++ row) {
	for (col = 0; col < toMul.m_Width; ++ col) {
	    HisOffset = col;
	    product->m_Data [ProductOffset] = 0;
	
	    for (idx = 0; idx < m_Width; ++ idx) {
		product->m_Data [ProductOffset] += m_Data [MyOffset + idx] * 
						toMul.m_Data [HisOffset];
		HisOffset += toMul.m_Width;
	    }
	    ++ ProductOffset;   
	}
	MyOffset += m_Width;
    }
       
    return (*product);
}
    
template <class MatrixEl>
inline void Matrix<MatrixEl> :: Transpose (void)
{		    
MatrixSizeType temp, x, y;
MatrixEl * NewValue;

    NewValue = new MatrixEl [m_Width * m_Height];
    for (y = 0; y < m_Height; ++ y)
	for (x = 0; x < m_Width; ++ x)
	    NewValue [x * m_Height + y] = m_Data [y * m_Width + x];
    for (x = 0; x < m_Width * m_Height; ++ x)
	m_Data [x] = NewValue [x];

    temp = m_Width;
    m_Width = m_Height;
    m_Height = m_Width;	 
}

template <class MatrixEl>
inline void Matrix<MatrixEl> :: RowReduce (void)
{
    cerr << "Row reduction is not yet implemented!\n";
    exit (1);
} 
 
template <class MatrixEl>
inline MatrixEl Matrix<MatrixEl> :: Determinant (void)
{
MatrixEl det = 1;
MatrixSizeType x;

    if (m_Width != m_Height) {
	cerr << "Determinant: can only work with square matrices\n";
	exit (1);
    }
    RowReduce();
    for (x = 0; x < m_Width; ++ x)
	det *= m_Data [x * m_Width + x];

    return (det);
} 
    	     				
template <class MatrixEl>
inline void Matrix<MatrixEl> :: ReadTextStream
(istream& is)
{	    
MatrixSizeType row, col, SavedWidth = 0, SavedHeight = 0;
  						
    is >> SavedHeight;
    is >> SavedWidth;			
    Remsize (SavedHeight, SavedWidth);
    for (row = 0; row < m_Height; ++ row) {
	for (col = 0; col < m_Width; ++ col)
	    is >> m_Data [row * m_Width + col];
    }
} 

template <class MatrixEl>
inline void Matrix<MatrixEl> :: WriteTextStream
(ostream& os)
{	    
MatrixSizeType row, col;
  		 
    os << m_Height << " " << m_Width << "\n";
    for (row = 0; row < m_Height; ++ row) {
	for (col = 0; col < m_Width; ++ col)
	    os << m_Data [row * m_Width + col] << " ";
	os << "\n";
    }
} 
       
#endif

