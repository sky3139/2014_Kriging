#include "PRQuadtree.h"
// #include "pr_quadtree.h"
// #include "../stack/stack.h"
// #include "point.h"
// #include "../util/array_lookup.h"
#include <iostream>
#include <cmath>
#include <stack>
using namespace std;

#define eps .00001

#include <cstring>
#include <stdlib.h>
#include <stdbool.h>
#include <cassert>
// typedef double Point2D[2];
// typedef Point2D Rectangle[2];

static Point2D origin(0,0);// = {0, 0};
static Point2D test_point2 = {5, 5};
static Point2D test_point3 = {-5, 5};
static Point2D test_point4 = {-5, -5};
static Point2D test_point5 = {5, -5};
static Rectangle plane;

static void test_empty_quadtree()
{
    PRQuadtree *q = new PRQuadtree(plane);
    assert(q->prquadtree_is_empty());
    assert(q->prquadtree_size() == 0);
    assert(q->prquadtree_search(origin) == NULL);
    delete q;
}
static void test_singleton_quadtree()
{
    PRQuadtree *q = new PRQuadtree(plane);
    // insert up to three and check invaiants at each step
    assert(-1 == q->prquadtree_height());
    q->prquadtree_add(origin);
    assert(0 == q->prquadtree_height());
    assert(1 == q->prquadtree_size());
    q->prquadtree_add(test_point2);
    assert(0 == q->prquadtree_height());
    assert(2 == q->prquadtree_size());
    q->prquadtree_add(test_point3);
    assert(0 == q->prquadtree_height());
    assert(3 == q->prquadtree_size());
    q->prquadtree_add(test_point4);
    assert(0 == q->prquadtree_height());
    assert(4 == q->prquadtree_size());

    assert(q->prquadtree_search(origin) == true);
    assert(q->prquadtree_search(test_point2) == true);
    assert(q->prquadtree_search(test_point3) == true);
    assert(q->prquadtree_search(test_point4) == true);
    assert(q->prquadtree_search(test_point5) == false);

    delete q;
}

static void test_single_split_quadtree()
{
    PRQuadtree *q = new PRQuadtree(plane);

    q->prquadtree_add(test_point2);
    q->prquadtree_add(test_point3);
    q->prquadtree_add(test_point4);
    q->prquadtree_add(test_point5);
    q->prquadtree_add(origin);

    assert(q->prquadtree_search(origin));
    assert(q->prquadtree_search(test_point2));
    assert(q->prquadtree_search(test_point3));
    assert(q->prquadtree_search(test_point4));
    assert(q->prquadtree_search(test_point5));

    assert(1 == q->prquadtree_height());
    assert(5 == q->prquadtree_size());

    delete q;
}

static void test_repeated_split()
{
    Rectangle smaller = {
        {-10, -10},
        {10, 10}};
    PRQuadtree *q = new PRQuadtree(smaller);

    Point2D q1_points[5] = {
        {1, 1},
        {2, 2},
        {3, 3},
        {4, 4},
        {6, 6},
    };

    for (size_t i = 0; i < 5; i++)
    {
        q->prquadtree_add(q1_points[i]);
    }

    for (size_t i = 0; i < 5; i++)
    {
        assert(q->prquadtree_search(q1_points[i]));
    }

    assert(2 == q->prquadtree_height());

    assert(q->prquadtree_size() == 5);

    delete q;
}

static void test_removal()
{
    Rectangle smaller = {
        {-10, -10},
        {10, 10}};
    PRQuadtree *q = new PRQuadtree(smaller);

    Point2D q1_points[6] = {
        {1, 1},
        {2, 2},
        {3, 3},
        {4, 4},
        {6, 6},
        {2, 3},
    };

    for (size_t i = 0; i < 6; i++)
    {
        q->prquadtree_add(q1_points[i]);
    }
    assert(q->prquadtree_height() == 3);

    q->prquadtree_remove(q1_points[5]);
    assert(q->prquadtree_size() == 5);
    assert(q->prquadtree_height() == 3);
    assert(q->prquadtree_search(q1_points[5]) == NULL);

    for (int i = 0; i < 5; i++)
    {
        assert(q->prquadtree_search(q1_points[i]));
        q->prquadtree_remove(q1_points[i]);
        assert(q->prquadtree_size() == 4 - i);

        if (i == 3)
        {
            assert(q->prquadtree_height() == 2);
        }
    }

    delete q;
}
#include "float.h"
void infinite_rectangle(Rectangle rect)
{
    rect[SW][X] = rect[SW][Y] = -DBL_MAX;
    rect[NE][X] = rect[NE][Y] = DBL_MAX;
}
int main()
{
    point2d_eq_ctx.eq = eq_p_casted;
    point2d_eq_ctx.ctx = &point2d_dims;

    infinite_rectangle(plane);
    test_empty_quadtree();
    test_singleton_quadtree();
    test_single_split_quadtree();
    test_repeated_split();
    test_removal();
};

//
//
// }
