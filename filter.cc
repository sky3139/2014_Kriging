#include "src/InputReader.h"
#include "InverseDist.h"
#include <Eigen/Dense>
#include "Kriging.h"

using namespace Eigen;
int InverseDisttest()
{
    InputReader m_ir;
    int nDiameter = 200;
    vector<double> vecZs;
    Interpolater *pInterpolater = NULL;
    m_ir.Read("../testdata.txt");

    vector<Point3D> &input = const_cast<vector<Point3D> &>(m_ir.Get3DPoints());

    for (int i = 0; i < input.size(); i++)
    {
        cout << i << " " << input[i].x << " " << input[i].y << " " << input[i].z << endl;
    }

    pInterpolater = new InverseDist(200, 4);
    // pInterpolater = new Kriging(input.begin(), input.end(), 4);
    int nRadius = nDiameter / 2;
    for (int i = 0; i < nDiameter; i++)
    {
        for (int j = 0; j < nDiameter; j++)
        {
            double z = pInterpolater->GetInterpolatedZ(j - nRadius, i - nRadius, &input[0], &input[input.size() - 1]);
            vecZs.push_back(z);
        }
    }
    delete pInterpolater;

    /*转换到Eigen矩阵方便在cmd中查看输出的插值后的z**/
    MatrixXd m_matrix = Map<Matrix<double, 1, 4000, RowMajor>>(vecZs.data());
    cout << "After Inverse Dist Interpolate:\n"
         << m_matrix.transpose() << endl;

    return 0;
}
int main()
{
    // InverseDisttest();
    InputReader m_ir;
    int nDiameter = 200;
    vector<double> vecZs;
    Kriging *pInterpolater = NULL;
    m_ir.Read("../testdata.txt");

    vector<Point3D> &input = const_cast<vector<Point3D> &>(m_ir.Get3DPoints());

    for (int i = 0; i < input.size(); i++)
    {
        cout << i << " " << input[i].x << " " << input[i].y << " " << input[i].z << endl;
    }

    pInterpolater = new Kriging( &input.front(),&input.back(),4.0);
    int nRadius = nDiameter / 2;
    for (int i = 0; i < nDiameter; i++)
    {
        for (int j = 0; j < nDiameter; j++)
        {
            double z = pInterpolater->GetInterpolatedZ(j - nRadius, i - nRadius, &input[0], &input[input.size() - 1]);
            vecZs.push_back(z);
        }
    }
    delete pInterpolater;

    /*转换到Eigen矩阵方便在cmd中查看输出的插值后的z**/
    MatrixXd m_matrix = Map<Matrix<double, 1, 4000, RowMajor>>(vecZs.data());
    cout << "After Inverse Dist Interpolate:\n"
         << m_matrix.transpose() << endl;
}