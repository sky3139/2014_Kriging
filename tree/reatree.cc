#include <bits/stdc++.h>

using namespace std;
enum Direction
{
    X,
    Y
};
class Box
{
public:
    int xMin;
    int yMin;
    int xMax;
    int yMax;
    Box();
    Box(int xMin, int yMin, int xMax, int yMax);
};
class Point
{
public:
    int x;
    int y;
    friend bool XSortFunc(Point a, Point b);
    friend bool YSortFunc(Point a, Point b);
    Point operator+(const Point &other);
    friend ostream &operator<<(ostream &out, Point point);
};
bool XSortFunc(Point a, Point b);
bool YSortFunc(Point a, Point b);
class RangeTree
{
public:
    RangeTree *nextDimension;
    Point eleXDir;
    Point eleYDir;
    Direction dir;
    int l;
    int r;
    int depth;
    void BuildTree(RangeTree *rangeTreeX, RangeTree **rangeTreeY, Point *xSortList, Point *ySortList, int l, int r, int index, int indexY, Direction dir);
    void HelpBuildYTree(int rBound, int i, RangeTree **rangeTreeY, Point *xSortList, int l, int r);
    void Bfs(RangeTree *root, int l, int r, Direction dir);
    void DivideAndBuildYTree(RangeTree **rangeTreeY, Point *xSortList, int l, int r, int index);
    void MergeAndBuildYTree(RangeTree **rangeTreeY, Point *xSortList, int l, int r, int index);
    void BoxSearch(RangeTree *rangeTreeX, RangeTree **rangeTreeY, int l, int r, Box box, vector<Point> &points, Direction dir, int index, int indexY);
    RangeTree(RangeTree *nextDimension, Point eleXDir, Point eleYDir, Direction dir);
    RangeTree();
};
int maxN = 0;
int main()
{
    int l = 0, r = 10, lY = 0, rY = 10;
    // cin >> l >> r >> lY >> rY;
    Point *pointList = new Point[r + 1];
    for (int i = l; i <= r; ++i)
    {
        Point point;
        point.x = i;
        point.y = i;
        pointList[i] = point;
    }
    sort(pointList + l, pointList + r + 1, XSortFunc);
    RangeTree *rangeTreeX = new RangeTree[4 * r];
    //第二个维度应该需要开一个二维数组，因为第一个维度每个元素需要指向一棵第二维度的树，
    //如果整体用一棵树，排序后就会有问题，比如(1,4),(2,8),(3,12),(4,1)，对于只是两个点的父节点结果应该是
    //(1,4),(2,8)，但是如果对是这四个结点的父节点来说，结果就成了(4,1) (1,4) (2,8) (3,12)
    //所以必须每个结点指向单独的树
    RangeTree **rangeTreeY = new RangeTree *[4 * r];
    for (int i = 1; i <= 4 * r; ++i)
    {
        rangeTreeY[i] = new RangeTree[4 * r];
    }

    rangeTreeX[1].BuildTree(rangeTreeX, rangeTreeY, pointList, pointList, l, r, 1, 1, X);

    int powCnt = 0;

    // rangeTreeY[1][1].HelpBuildYTree(maxN,1,rangeTreeY,pointList,l,r);
    rangeTreeY[1][1].DivideAndBuildYTree(rangeTreeY, pointList, l, r, 1);

    // for (int i = 1; i <= 15; ++i)
    // {

    //     rangeTreeY[2][1].Bfs(rangeTreeY[i], l, r, Y);
    //     cout << endl
    //          << endl;
    // }

    Box *box = new Box(0, 0, 4, 4);
    vector<Point> tarPoints;
    rangeTreeX[1].BoxSearch(rangeTreeX, rangeTreeY, l, r, *box, tarPoints, X, 1, 1);
    for (vector<Point>::iterator i = tarPoints.begin(); i < tarPoints.end(); ++i)
    {
        cout << *i << endl;
    }
    delete box, rangeTreeX;
    delete[] rangeTreeY, pointList;
    return 0;
}
bool XSortFunc(Point a, Point b)
{
    return a.x < b.x;
}
bool YSortFunc(Point a, Point b)
{
    return a.y < b.y;
}
RangeTree::RangeTree()
{
    nextDimension = NULL;
}
ostream &operator<<(ostream &out, Point point)
{
    out << point.x << " " << point.y;
}
RangeTree::RangeTree(RangeTree *nextDimension, Point eleXDir, Point eleYDir, Direction dir)
{
    this->nextDimension = nextDimension;
    this->eleXDir = eleXDir;
    this->eleYDir = eleYDir;
    this->dir = dir;
}
Point Point::operator+(const Point &other)
{
    Point point;
    point.x = this->x + other.x;
    point.y = this->y + other.y;
    return point;
}
Box::Box()
{
}
Box::Box(int xMin, int yMin, int xMax, int yMax)
{
    this->xMin = xMin;
    this->yMin = yMin;
    this->xMax = xMax;
    this->yMax = yMax;
}
//构建范围树
void RangeTree::BuildTree(RangeTree *rangeTreeX, RangeTree **rangeTreeY, Point *xSortList, Point *ySortList, int l, int r, int index, int indexY, Direction dir)
{
    maxN = max(maxN, index);
    // maxIndex = max(index,maxIndex);
    int m = (l + r) / 2;
    switch (dir)
    {
    case X:
        //递归到了叶结点
        if (l == r)
        {
            rangeTreeX[index].eleXDir = xSortList[m];
            rangeTreeX[index].dir = dir;
            rangeTreeX[index].nextDimension = rangeTreeY[index];
            rangeTreeX[index].l = l;
            rangeTreeX[index].r = r;
            return;
        }
        //递归构建左子树
        rangeTreeX[index].BuildTree(rangeTreeX, rangeTreeY, xSortList, ySortList, l, m, 2 * index, indexY, X);
        //递归构建右子树
        rangeTreeX[index].BuildTree(rangeTreeX, rangeTreeY, xSortList, ySortList, m + 1, r, 2 * index + 1, indexY, X);
        //为了截窗查询，这里取中点
        Point point;
        point.x = (rangeTreeX[2 * index].eleXDir.x + rangeTreeX[2 * index + 1].eleXDir.x) / 2;
        point.y = (rangeTreeX[2 * index].eleXDir.y + rangeTreeX[2 * index + 1].eleXDir.y) / 2;
        //父结点设置为两个子结点的中点
        rangeTreeX[index].eleXDir = point;
        // x方向的维度指针指向y方向的树
        rangeTreeX[index].nextDimension = rangeTreeY[index];
        rangeTreeX[index].l = l;
        rangeTreeX[index].r = r;
        break;
    case Y:

        if (l == r)
        {
            //找到了叶结点，注意这里的l和r指的是x方向的l和r，为了方便用索引建树与x方向一一对应
            rangeTreeY[index][indexY].eleYDir = ySortList[m];
            rangeTreeY[index][indexY].dir = dir;
            rangeTreeY[index][indexY].nextDimension = NULL;
            rangeTreeY[index][indexY].l = l;
            rangeTreeY[index][indexY].r = r;
            return;
        }

        //递归构建左子树
        rangeTreeY[index][indexY].BuildTree(rangeTreeX, rangeTreeY, xSortList, ySortList, l, m, index, 2 * indexY, Y);
        //递归构建右子树
        rangeTreeY[index][indexY].BuildTree(rangeTreeX, rangeTreeY, xSortList, ySortList, m + 1, r, index, 2 * indexY + 1, Y);

        //为了截窗查询，这里取中点
        point.y = (rangeTreeY[index][2 * indexY].eleYDir.y + rangeTreeY[index][2 * indexY + 1].eleYDir.y) / 2;
        point.x = (rangeTreeY[index][2 * indexY].eleYDir.x + rangeTreeY[index][2 * indexY + 1].eleYDir.x) / 2;
        rangeTreeY[index][indexY].eleYDir = point;

        rangeTreeY[index][indexY].dir = dir;
        //因为是二维的，y方向的维度指针没有指向任何树
        rangeTreeY[index][indexY].nextDimension = NULL;
        rangeTreeY[index][indexY].l = l;
        rangeTreeY[index][indexY].r = r;
        break;
    }
}
// debug用
void RangeTree::Bfs(RangeTree *root, int l, int r, Direction dir)
{
    queue<int> q;
    q.push(1);
    int index = 1;
    root[1].depth = 1;
    while (!q.empty())
    {
        int a = q.front();
        q.pop();
        if (index < root[a].depth)
        {
            index = root[a].depth;
            cout << endl;
        }
        if (dir == X)
        {
            cout << root[a].eleXDir << " ";
        }
        else
        {
            cout << root[a].eleYDir << " ";
        }

        if (root[a].r > root[a].l)
        {
            root[2 * a].depth = root[a].depth + 1;
            root[2 * a + 1].depth = root[a].depth + 1;
            q.push(2 * a);
            q.push(2 * a + 1);
        }
    }
}
//这样子建Y方向的树不太好，帮助x为l和r之间的结点建一个y方向的范围树
void RangeTree::HelpBuildYTree(int rBound, int i, RangeTree **rangeTreeY, Point *xSortList, int l, int r)
{
    if (i > rBound)
        return;
    //获取l到r之间的point
    Point *list = new Point[r + 1];
    for (int j = l; j <= r; ++j)
    {
        list[j] = xSortList[j];
    }
    //对l到r之间的point排序
    sort(list + l, list + r + 1, YSortFunc);
    //对y方向建树
    rangeTreeY[i][1].BuildTree(NULL, rangeTreeY, list, list, l, r, i, 1, Y);
    int m = (l + r) / 2;
    //帮助构建左子树和右子树
    if (m >= l)
        HelpBuildYTree(rBound, i * 2, rangeTreeY, xSortList, l, m);
    if (m + 1 <= r)
        HelpBuildYTree(rBound, i * 2 + 1, rangeTreeY, xSortList, m + 1, r);
}
//更好的方法应该是利用递归的归并排序的过程中分和合，保证组内有序，先进行递归，到叶结点的时候再开始建树，这样就不需要对每个结点刻意进行排序了，我这样子弄好像不太好
void RangeTree::DivideAndBuildYTree(RangeTree **rangeTreeY, Point *xSortList, int l, int r, int index)
{
    if (l < r)
    {
        int m = (l + r) / 2;
        DivideAndBuildYTree(rangeTreeY, xSortList, l, m, 2 * index);
        DivideAndBuildYTree(rangeTreeY, xSortList, m + 1, r, 2 * index + 1);
        MergeAndBuildYTree(rangeTreeY, xSortList, l, r, index);
    }
}
void RangeTree::MergeAndBuildYTree(RangeTree **rangeTreeY, Point *xSortList, int l, int r, int index)
{
    int m = (l + r) / 2;
    Point *list = new Point[r - l + 1];
    int pl = l, pr = m + 1;
    int p = 0;
    while (pl <= m && pr <= r)
    {
        list[p++] = xSortList[pl].y < xSortList[pr].y ? xSortList[pl++] : xSortList[pr++];
    }
    while (pl <= m)
    {
        list[p++] = xSortList[pl++];
    }
    while (pr <= r)
    {
        list[p++] = xSortList[pr++];
    }

    if (r - l == 1)
    {
        //分到倒数第二层，需要把单个的结点建树
        BuildTree(NULL, rangeTreeY, xSortList, xSortList, l, l, 2 * index, 1, Y);
        BuildTree(NULL, rangeTreeY, xSortList, xSortList, r, r, 2 * index + 1, 1, Y);
    }

    for (int i = 0; i < p; ++i)
    {
        xSortList[i + l] = list[i];
    }
    //建非单个结点的树
    BuildTree(NULL, rangeTreeY, xSortList, xSortList, l, r, index, 1, Y);
}
//这个是找位于box开区间内的点
void RangeTree::BoxSearch(RangeTree *rangeTreeX, RangeTree **rangeTreeY, int l, int r, Box box, vector<Point> &points, Direction dir, int index, int indexY)
{
    int m = (l + r) / 2;
    switch (dir)
    {
    case X:
        //树中的x完全在要被查找的框内
        if (l > box.xMin && l < box.xMax && r > box.xMin && r < box.xMax)
        {
            BoxSearch(rangeTreeX, rangeTreeY, l, r, box, points, Y, index, 1);
            return;
        }

        //到了叶结点，并且叶结点不在框内
        if (l == r)
        {
            return;
        }

        //树中的x左半边有可能有包含在框内的点
        if (m >= box.xMax)
        {
            BoxSearch(rangeTreeX, rangeTreeY, l, m, box, points, X, 2 * index, 1);
        }
        //树中的x右半边可能有包含在框内的点
        if (m + 1 <= box.xMin)
        {
            BoxSearch(rangeTreeX, rangeTreeY, m + 1, r, box, points, X, 2 * index + 1, 1);
        }
        //两半边都可能存在符合条件的点
        if (m >= box.xMin && m + 1 <= box.xMax)
        {
            BoxSearch(rangeTreeX, rangeTreeY, l, m, box, points, X, 2 * index, 1);
            BoxSearch(rangeTreeX, rangeTreeY, m + 1, r, box, points, X, 2 * index + 1, 1);
        }
        break;

    case Y:
        //走到了这里相当于是X方向是完全在框内的，只需要考虑y方向是不是在框内即可
        //到了叶结点
        if (l == r)
        {
            //找到了一个符合条件的点
            if (rangeTreeY[index][indexY].eleYDir.y > box.yMin && rangeTreeY[index][indexY].eleYDir.y < box.yMax)
            {
                points.push_back(rangeTreeY[index][indexY].eleYDir);
            }
            return;
        }

        if (rangeTreeY[index][indexY].eleYDir.y >= box.yMax)
        {
            //递归查找左子树
            BoxSearch(rangeTreeX, rangeTreeY, l, m, box, points, dir, index, 2 * indexY);
        }

        if (rangeTreeY[index][indexY].eleYDir.y <= box.yMin)
        {
            //递归查找右子树
            BoxSearch(rangeTreeX, rangeTreeY, m + 1, r, box, points, dir, index, 2 * indexY + 1);
        }
        //左右子树都可能出现符合条件的点
        if (rangeTreeY[index][indexY].eleYDir.y >= box.yMin && rangeTreeY[index][indexY].eleYDir.y <= box.yMax)
        {
            BoxSearch(rangeTreeX, rangeTreeY, l, m, box, points, dir, index, 2 * indexY);
            BoxSearch(rangeTreeX, rangeTreeY, m + 1, r, box, points, dir, index, 2 * indexY + 1);
        }

        break;
    }
}