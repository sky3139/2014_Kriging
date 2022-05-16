// InputReader.cpp: implementation of the InputReader class.
//
//////////////////////////////////////////////////////////////////////

#include "InputReader.h"
#include <fstream>
#include <sstream>
using namespace std;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

void InputReader::Read(string filename) throw()
{
	ifstream in(filename.c_str());
	if (!in)
	{
		assert(0);
		return;
	}

	float x, y, z, r, g, b;
	m_vecPoints.clear();
	while (!in.eof())
	{
		string line;
		getline(in, line);
		istringstream iss(line);
		iss >> x >> y >> z >> r >> g >> b;
		// int i = (int)(0.299 * posClr.R + 0.587 * posClr.G + 0.114 * posClr.B);
		//  Gray = R*0.299 + G*0.587 + B*0.114
		// z=r/255.0f;
		m_vecPoints.push_back(Point3D(x, y, z));
	}
}

void InputReader::Dump(ostream &os) const throw()
{
	for (int i = 0; i < m_vecPoints.size(); i++)
		os << m_vecPoints[i].x << "\t" << m_vecPoints[i].y << "\t" << m_vecPoints[i].z << endl;
}