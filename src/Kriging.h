// Kriging.h: interface for the Kriging class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_KRIGING_H__2D4FB688_334E_464E_9E9F_55D489A8E5FC__INCLUDED_)
#define AFX_KRIGING_H__2D4FB688_334E_464E_9E9F_55D489A8E5FC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Interpolater.h"
#include "Matrix.h"
#include "Numeric.h"
#include <string>
#include <vector>
// #include <tchar.h>
#include <iostream>
using namespace std;

template <class ForwardIterator>
float GetDistance(const ForwardIterator start, int i, int j)
{
	return ::sqrt(::pow(((*(start + i)).x - (*(start + j)).x), 2) + ::pow(((*(start + i)).y - (*(start + j)).y), 2));
}

template <class ForwardIterator>
float GetDistance(float xpos, float ypos, const ForwardIterator start, int i)
{
	return ::sqrt(::pow(((*(start + i)).x - xpos), 2) + ::pow(((*(start + i)).y - ypos), 2));
}

template <class T, class ForwardIterator>
class TKriging : public TInterpolater<ForwardIterator>
{
public:
	TKriging(const ForwardIterator first, const ForwardIterator last,float dSemivariance) : m_dSemivariance(dSemivariance)
	{
		m_nSize = 0;
		ForwardIterator start = first;
		while (start != last)
		{
			++m_nSize;
			++start;
		}

		m_matA.SetDimension(m_nSize, m_nSize);

		for (int j = 0; j < m_nSize; j++)
		{
			for (int i = 0; i < m_nSize; i++)
			{
				if (i == m_nSize - 1 || j == m_nSize - 1)
				{
					m_matA(i, j) = 1;
					if (i == m_nSize - 1 && j == m_nSize - 1)
						m_matA(i, j) = 0;
					continue;
				}
				m_matA(i, j) = ::GetDistance(first, i, j) * m_dSemivariance;
			}
		}
		int nD;
		LUDecompose(m_matA, m_Permutation, nD);
	}
	float GetInterpolatedZ(float xpos, float ypos, ForwardIterator first, ForwardIterator last)
	{
		std::vector<float> vecB(m_nSize);
		for (int i = 0; i < m_nSize; i++)
		{
			float dist = ::GetDistance(xpos, ypos, first, i);
			vecB[i] = dist * m_dSemivariance;
		}
		vecB[m_nSize - 1] = 1;

		LUBackSub(m_matA, m_Permutation, vecB);

		float z = 0;
		for (int i = 0; i < m_nSize - 1; i++)
		{
			float inputz = (*(first + i)).z;
			z += vecB[i] * inputz;
		}
		if (z < 0)
			z = 0;
		return z;
	}

private:
	TMatrix<T> m_matA;
	vector<int> m_Permutation;
	int m_nSize;
	float m_dSemivariance;
};

typedef TKriging<float, Point3D *> Kriging;

#endif // !defined(AFX_KRIGING_H__2D4FB688_334E_464E_9E9F_55D489A8E5FC__INCLUDED_)
