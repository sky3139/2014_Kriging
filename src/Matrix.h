// Matrix.h: interface for the Matrix class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MATRIX_H__3461DABF_4919_4B31_81EA_AC9397ABBAE1__INCLUDED_)
#define AFX_MATRIX_H__3461DABF_4919_4B31_81EA_AC9397ABBAE1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>

class MatrixException
{
public:
	MatrixException(std::string message) throw() {}
};

template<class T>
class TMatrix
{
public:
	TMatrix() throw() : m_nWidth(0), m_nHeight(0), m_arByte(NULL) {}
	TMatrix(int nWidth, int nHeight) throw();
	TMatrix(int nWidth, int nHeight, T* pByte) throw();
	TMatrix(const TMatrix& rhs) throw();
	virtual ~TMatrix();
	void SetDimension(int nWidth, int nHeight) throw();
	void ZeroAll() throw();
	void Clean() throw();
	TMatrix& operator=(const TMatrix& rhs) throw();
	void operator+=(const TMatrix& rhs) ;
	void operator-=(const TMatrix& rhs) ;
	void PriorityAdd(const TMatrix& rhs) ;
	// inline access functions
	T& operator[](int nIndex) ;
	T operator[](int nIndex) const ;
	T& operator()(int nRow, int nCol) ;
	T operator()(int nRow, int nCol) const ;
	// getter
	int GetWidth()  { return m_nWidth; }
	int GetHeight()  { return m_nHeight; }
	// uitilies
	T* GetRow(int nIndex) ;
	T* GetArray() throw();
	bool IsSameDimension(const TMatrix& rhs) ;
	void MakeSameDimension(TMatrix& output) const ;
	// virtual functions
	virtual void Load(std::string filename)  { throw MatrixException(("Not Implemented")); }
	virtual void Save(std::string filename) const  { throw MatrixException(("Not Implemented")); }
#ifdef _DEBUG
	virtual void Dump(std::ostream& os) ;
#endif
protected:
	T* m_arByte;
	int m_nWidth;
	int m_nHeight;
};

template<class T>
inline T& TMatrix<T>::operator[](int nIndex) 
{
#ifdef _DEBUG
	if(nIndex < 0 || nIndex >= m_nWidth * m_nHeight) {
		TCHAR buffer[MAX_PATH];
		::sprintf(buffer, ("Index out of bound : %d"), nIndex);
		throw MatrixException(buffer);
	}
#endif
	return m_arByte[nIndex];
}

template<class T>
inline T TMatrix<T>::operator[](int nIndex) const 
{
#ifdef _DEBUG
	if(nIndex < 0 || nIndex >= m_nWidth * m_nHeight)
		throw MatrixException(("Index out of bound const []"));
#endif
	return m_arByte[nIndex];
}

template<class T>
inline T& TMatrix<T>::operator()(int nRow, int nCol) 
{
#ifdef _DEBUG
	if(nCol <0 || nCol >= m_nWidth || nRow < 0 || nRow >= m_nHeight) {
		TCHAR buffer[MAX_PATH];
		::sprintf(buffer, ("Index out of bound : row %d, col %d"), nRow, nCol);
		throw MatrixException(buffer);
	}
#endif
	return m_arByte[nCol + nRow * m_nWidth];
}

template<class T>
inline T TMatrix<T>::operator()(int nRow, int nCol) const 
{
#ifdef _DEBUG
	if(nCol <0 || nCol >= m_nWidth || nRow < 0 || nRow >= m_nHeight) {
		TCHAR buffer[MAX_PATH];
		::sprintf(buffer, ("Index out of bound : row %d, col %d"), nRow, nCol);
		throw MatrixException(buffer);
	}
#endif
	return m_arByte[nCol + nRow * m_nWidth];
}

template<class T>
TMatrix<T>::TMatrix(int nWidth, int nHeight) throw() : m_nWidth(nWidth), m_nHeight(nHeight)
{
	int nTotalSize = nWidth * nHeight;
	m_arByte = new T[nTotalSize];
	::memset(m_arByte, 0, sizeof(T) * nTotalSize);
}

template<class T>
TMatrix<T>::TMatrix(int nWidth, int nHeight, T* pByte) throw() : m_nWidth(nWidth), m_nHeight(nHeight)
{
	int nTotalSize = nWidth * nHeight;
	m_arByte = new T[nTotalSize];
	::memcpy(m_arByte, pByte, sizeof(T) * nTotalSize);
}

template<class T>
TMatrix<T>::TMatrix(const TMatrix<T>& rhs) throw()
{
	m_nWidth = rhs.m_nWidth;
	m_nHeight = rhs.m_nHeight;
	int nTotalSize = m_nWidth * m_nHeight;
	m_arByte = new T[nTotalSize];
	::memcpy(m_arByte, rhs.m_arByte, sizeof(T) * nTotalSize);
}

template<class T>
TMatrix<T>::~TMatrix()
{
	Clean();
}

template<class T>
void TMatrix<T>::SetDimension(int nWidth, int nHeight) throw()
{
	Clean();
	m_nWidth = nWidth;
	m_nHeight = nHeight;
	int nTotalSize = m_nWidth * m_nHeight;
	m_arByte = new T[nTotalSize];
	::memset(m_arByte, 0, sizeof(T) * nTotalSize);
}

template<class T>
void TMatrix<T>::ZeroAll() throw()
{
	::memset(m_arByte, 0, sizeof(T) * m_nWidth * m_nHeight);
}

template<class T>
void TMatrix<T>::Clean() throw()
{
	m_nWidth = 0;
	m_nHeight = 0;
	if(m_arByte) {
		delete[] m_arByte;
		m_arByte = NULL;
	}
}

template<class T>
TMatrix<T>& TMatrix<T>::operator=(const TMatrix<T>& rhs) throw()
{
	if(&rhs == this)
		return *this;

	if(!IsSameDimension(rhs)) {
		m_nWidth = rhs.m_nWidth;
		m_nHeight = rhs.m_nHeight;
		if(m_arByte)
			delete[] m_arByte;

		int nTotalSize = m_nWidth * m_nHeight;
		m_arByte = new T[nTotalSize];
	}

	::memcpy(m_arByte, rhs.m_arByte, sizeof(T) * m_nWidth * m_nHeight);

	return *this;
}

template<class T>
void TMatrix<T>::operator+=(const TMatrix<T>& rhs) 
{
	if(m_nWidth != rhs.GetWidth() || m_nHeight != rhs.GetHeight())
		throw MatrixException(("Different dimension in argument") + LOCATION);

	int nTotalByte = m_nWidth * m_nHeight;
	for(int i=0; i<nTotalByte; i++) {
		m_arByte[i] += rhs[i];
		if(m_arByte[i] > 255)
			m_arByte[i] = 255;
	}
}

template<class T>
void TMatrix<T>::operator-=(const TMatrix<T>& rhs) 
{
	if(m_nWidth != rhs.GetWidth() || m_nHeight != rhs.GetHeight())
		throw MatrixException(("Different dimension in argument") + LOCATION);

	int nTotalByte = m_nWidth * m_nHeight;
	for(int i=0; i<nTotalByte; i++) {
		m_arByte[i] = abs(m_arByte[i] - rhs[i]);
		if(m_arByte[i] > 255)
			m_arByte[i] = 255;
	}
}

template<class T>
void TMatrix<T>::PriorityAdd(const TMatrix<T>& rhs) 
{
	if(m_nWidth != rhs.GetWidth() || m_nHeight != rhs.GetHeight())
		throw MatrixException(("Different dimension in argument") + LOCATION);

	int nTotalByte = m_nWidth * m_nHeight;
	for(int i=0; i<nTotalByte; i++)
		m_arByte[i] = max(m_arByte[i], rhs[i]);
}

template<class T>
T* TMatrix<T>::GetRow(int nIndex) 
{
	if(nIndex < 0 || nIndex >= m_nHeight)
		throw MatrixException(("Not existing row index") + LOCATION);

	T* pByte = m_arByte;
	pByte += m_nWidth * nIndex * sizeof(T);
	return pByte;
}

template<class T>
T* TMatrix<T>::GetArray() throw()
{
	return m_arByte;
}

template<class T>
bool TMatrix<T>::IsSameDimension(const TMatrix& rhs) 
{
	if(m_nWidth == rhs.m_nWidth && m_nHeight == rhs.m_nHeight)
		return true;
	else
		return false;
}

template<class T>
void TMatrix<T>::MakeSameDimension(TMatrix<T>& output) const
{
	if(output.m_nHeight != m_nHeight || output.m_nWidth != m_nWidth) {
		output.Clean();
		output.m_nHeight = m_nHeight;
		output.m_nWidth = m_nWidth;
		output.m_arByte = new T[m_nWidth * m_nHeight];
		::memset(output.m_arByte, 0, sizeof(T) * m_nHeight * m_nWidth);
	}
}

#ifdef _DEBUG
template<class T>
void TMatrix<T>::Dump(std::ostream& os) 
{
	for(int i=0; i<m_nHeight; i++)
		for(int j=0; j<m_nWidth; j++)
			os << "(" << i << ", " << j << ") : " << (int)m_arByte[i * m_nWidth + j] << std::endl;
}
#endif

#endif // !defined(AFX_MATRIX_H__3461DABF_4919_4B31_81EA_AC9397ABBAE1__INCLUDED_)
