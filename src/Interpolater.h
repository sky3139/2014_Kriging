// Interpolater.h: interface for the Interpolater class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INTERPOLATER_H__D0C4FC82_70D0_461D_8D5D_A5718F4D75DF__INCLUDED_)
#define AFX_INTERPOLATER_H__D0C4FC82_70D0_461D_8D5D_A5718F4D75DF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include <cassert>
using namespace std;

#include "BaseException.h"
#include "BaseData.h"

class InterpolaterException : public BaseException
{
public:
	InterpolaterException(string message) throw() : BaseException(message) {}
	InterpolaterException(string message, string location) throw() : BaseException(message, location) {}
};

template <class ForwardIterator>
class TInterpolater
{
public:
	virtual ~TInterpolater() {}
	virtual float GetInterpolatedZ(float xpos, float ypos, ForwardIterator first, ForwardIterator last) = 0;
	virtual float GetInterpolatedZ2(float xpos, float ypos, ForwardIterator first, ForwardIterator last) { return -1.0f; };
};

typedef TInterpolater<Point3D *> Interpolater;

#endif // !defined(AFX_INTERPOLATER_H__D0C4FC82_70D0_461D_8D5D_A5718F4D75DF__INCLUDED_)
