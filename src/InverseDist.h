// InverseDist.h: interface for the InverseDist class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INVERSEDIST_H__BB8C4FFF_A66C_4669_8B04_7D3080F5066A__INCLUDED_)
#define AFX_INVERSEDIST_H__BB8C4FFF_A66C_4669_8B04_7D3080F5066A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Interpolater.h"
#include <cassert>
#include <cmath>

struct WeightAndZ
{
	WeightAndZ(float w, float zz) : weight(w), z(zz) {}
	float weight;
	float z;
};

template <class ForwardIterator>
class TInverseDist : public TInterpolater<ForwardIterator>
{
public:
	TInverseDist(float dRadius = 200, float dExponent = 8) throw() : m_dRadius(dRadius), m_dExponent(dExponent) {}
	float GetInterpolatedZ(float xpos, float ypos, ForwardIterator first, ForwardIterator last)
	{
		// Check if Data is valid
		assert(first != last);

		float distance = 0;
		float weight = 0;
		float totalWeight = 0;

		vector<WeightAndZ> vecWeight;
		ForwardIterator start = first;
		while (start != last)
		{
			distance = ::sqrt((::pow((xpos - (*start).x), 2) + ::pow((ypos - (*start).y), 2)));
			if (distance == 0) // exact match
				return (*start).z;

			if (distance > m_dRadius)
			{
				++start;
				continue;
			}

			weight = 1 / ::pow(distance, m_dExponent);
			vecWeight.push_back(WeightAndZ(weight, (*start).z));

			totalWeight += weight;
			++start;
		}
		for (int i = 0; i < vecWeight.size(); i++)
			vecWeight[i].weight /= totalWeight;

		float nodeValue = 0;
		for (int i = 0; i < vecWeight.size(); i++)
		{
			nodeValue += vecWeight[i].weight * vecWeight[i].z;
			++first;
		}
		return nodeValue;
	}

private:
	float m_dRadius;
	float m_dExponent;
};

typedef TInverseDist<Point3D *> InverseDist;

#endif // !defined(AFX_INVERSEDIST_H__BB8C4FFF_A66C_4669_8B04_7D3080F5066A__INCLUDED_)
