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

	double x, y, z;
	m_vecPoints.clear();
	while (!in.eof())
	{
		string line;
		getline(in, line);
		istringstream iss(line);
		iss >> x >> y >> z;
		m_vecPoints.push_back(Point3D(x, y, z));
	}
}

void InputReader::Dump(ostream &os) const throw()
{
	for (int i = 0; i < m_vecPoints.size(); i++)
		os << m_vecPoints[i].x << "\t" << m_vecPoints[i].y << "\t" << m_vecPoints[i].z << endl;
}