// InputReader.h: interface for the InputReader class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INPUTREADER_H__5975EFD1_AEFB_4A37_AF24_80EB0E8894D3__INCLUDED_)
#define AFX_INPUTREADER_H__5975EFD1_AEFB_4A37_AF24_80EB0E8894D3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include <vector>
#include "Interpolater.h"
using namespace std;

class InputReader
{
public:
	InputReader() throw() {}
	InputReader(string filename) throw() { Read(filename); }
	void Read(string filename) throw();
	const vector<Point3D> &Get3DPoints() const throw() { return m_vecPoints; }
	void Dump(ostream &os) const throw();

	void print()
	{
		for (int i = 0; i < m_vecPoints.size(); i++)
		{
			// cout << i << " " << m_vecPoints[i].x << " " << m_vecPoints[i].y << " " << m_vecPoints[i].z << endl;
			cout << m_vecPoints[i].x << " " << m_vecPoints[i].y << " " << m_vecPoints[i].z << endl;
		}
	}

private:
	vector<Point3D> m_vecPoints;
};

#endif // !defined(AFX_INPUTREADER_H__5975EFD1_AEFB_4A37_AF24_80EB0E8894D3__INCLUDED_)
