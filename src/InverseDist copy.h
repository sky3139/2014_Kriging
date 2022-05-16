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

#include "opencv2/opencv.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/flann.hpp"
#include <iostream>
#include <algorithm>
#include <memory>
#include <functional>
#include <chrono>
#include <random>
#include <cstdlib>
#include <iomanip>
using namespace std;
using namespace cv;

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
	cv::flann::GenericIndex<cv::flann::L2<float>> *kdtree; // = cv::flann::GenericIndex<cv::flann::L2<float>>(cv::Mat(pts).reshape(1), cvflann::KDTreeIndexParams{2});
	vector<Point2f> pts;
	// 在半径m_dRadius的球内的点插值 m_dExponent 幂   W=1/(d^m)
	TInverseDist(ForwardIterator first, ForwardIterator last, float dRadius = 200, float dExponent = 8) throw() : m_dRadius(dRadius), m_dExponent(dExponent)
	{

		ForwardIterator start = first;
		while (start != last)
		{
			pts.emplace_back((*start).x, (*start).y);
			++start;
		}
		kdtree = new cv::flann::GenericIndex<cv::flann::L2<float>>(cv::Mat(pts).reshape(1), cvflann::KDTreeIndexParams{2});
	}
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
			// cout << (*start).x << " " << (*start).y << " " << distance << " " << weight << endl;

			totalWeight += weight;
			++start;
		}
		// cout << "totalWeight" << totalWeight << "   " << vecWeight.size() << endl;
		for (int i = 0; i < vecWeight.size(); i++)
			vecWeight[i].weight /= totalWeight;

		float nodeValue = 0;
		for (int i = 0; i < vecWeight.size(); i++)
		{
			nodeValue += vecWeight[i].weight * vecWeight[i].z;
			++first;
		}
		// cout << (*start).x << " " << (*start).y << " " << distance << " " << nodeValue << endl;

		return nodeValue;
	}
	float lastz = 0.0f;
	float GetInterpolatedZ2(float xpos, float ypos, ForwardIterator first, ForwardIterator last)
	{
		float distance = 0;
		float weight = 0;
		float totalWeight = 0;

		vector<WeightAndZ> vecWeight; //权重和Z
		ForwardIterator start = first;

		constexpr int K = 200;
		vector<int> indices(K, -1);
		vector<float> dists(K, -1);
		// kdtree->knnSearch({xpos, ypos}, indices, dists, K, cvflann::SearchParams{});
		cvflann::SearchParams sp(0x7fff, 0, false);
		kdtree->radiusSearch({xpos, ypos}, indices, dists, m_dRadius*m_dRadius, sp);

		// cout << "->查询点坐标为：" << searchPoint << "\n";
		for (int i = 0; i < indices.size(); i++)
		{
			if (indices[i] < 0)
				break;
			auto pt = pts[indices[i]];

			distance = ::sqrt(dists[i]);
			// cout << pt << " " << distance << " " <<  << " id=" << indices[i] << endl;

			if (distance == 0) // exact match
				return (*start).z;

			if (distance > m_dRadius)
			{
				++start;
				continue;
			}
			// cout <<pt << " " << distance  << endl;
			// 0.4 0.1 0.364005 56.9598
			weight = 1.0f / ::pow(distance, m_dExponent);
			vecWeight.push_back(WeightAndZ(weight, (*start).z));
			totalWeight += weight;
			++start;
		}

		cout << totalWeight << "   " << vecWeight.size() << endl;
		// assert(0);
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
	~TInverseDist()
	{
		delete kdtree;
	}

private:
	float m_dRadius;
	float m_dExponent;
};

typedef TInverseDist<Point3D *> InverseDist;

#endif // !defined(AFX_INVERSEDIST_H__BB8C4FFF_A66C_4669_8B04_7D3080F5066A__INCLUDED_)
