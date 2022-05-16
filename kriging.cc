#include "src/InputReader.h"
#include "InverseDist.h"
#include <Eigen/Dense>
#include "Kriging.h"
#include <fstream>
using namespace std;
using namespace Eigen;
int InverseDisttest()
{

    // MatrixXf m_matrix = Map<Matrix<float, 1, 4000, RowMajor>>(vecZs.data());
    // cout << m_matrix.transpose() << endl;
    return 0;
}

class datacoll
{
public:
    vector<vector<float>> data;

    void toppm(string str = "b.ppm")
    {
        cout << str << data.size() << endl;
        fstream fs;
        fs.open(str, std::ios::out);
        assert(fs.is_open());
        fs << "P6 " << data.size() << " " << data[0].size() << " 255" << endl;
        for (auto &it : data)
        {
            for (auto &ita : it)
            {
                fs << (int)(ita * 255) << " ";
            }
            fs << endl;
        }
    }
    void toxyz(string str = "b.xyz")
    {
        fstream fs("asd.xyz", std::ios::out);

        for (int i = 0; i < data.size(); i += 1)
        {
            for (int j = 0; j < data[0].size(); j += 1)
            {
                fs << i << " " << j << " " << data[i][j] << endl;
            }
        }
        fs.close();
    }
};
int main(int ac, char **av)
{
    // InverseDisttest();
    if (ac <= 1)
    {
        cout << "./app <start>0 step>0.1 end>5.0" << endl;
        return 0;
    }
    InputReader m_ir;

    float start = atof(av[2]); // 50;
    float step = atof(av[3]);
    float end = atof(av[4]); // 50;
    vector<float> vecZs;
    Interpolater *pInterpolater = NULL;
    // m_ir.Read("../testdata.txt");
    m_ir.Read(av[1]);

    vector<Point3D> &input = const_cast<vector<Point3D> &>(m_ir.Get3DPoints());
    // pInterpolater = new Kriging(&input.front(), &input.back(), 0.0020f);
    pInterpolater = new InverseDist(&input.front(), &input.back(), 1.0, 8.0);
        // pInterpolater = new InverseDist(1.0, 8.0);

    int ii = 0, jj = 0;
    datacoll dt;
    fstream fs1("t1.xyz", std::ios::out);
    fstream fs2("t2.xyz", std::ios::out);
    for (float i = start; i < end; i += step)
    {
        vector<float> zval;
        for (float j = start; j < end; j += step)
        {
            // i=2.5;
            // j=2.5;
            float z2 = pInterpolater->GetInterpolatedZ((j), (i), &input[0], &input.back());
            float z3 = pInterpolater->GetInterpolatedZ2((j), (i), &input[0], &input.back());

            // zval.push_back(z2);
            // vecZs.push_back(z);
            fs1 << i << " " << j << " " << z2 << endl;
            fs2 << i << " " << j << " " << z3 << endl;
            // cout << ii << " " << jj << " " <<
            // cout << (int)(z * 255) << " ";
            jj++;
        }
        // dt.data.push_back(zval);
        ii++;
        jj = 0;
        // cout << endl;
    }
    fs2.close();
    fs1.close();
    // dt.toxyz();
    delete pInterpolater;
}