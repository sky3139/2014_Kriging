// #include "point.h"
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
typedef double Point2D[2];
typedef Point2D Rectangle[2];

static Point2D origin = {0, 0};
static Point2D test_point2 = {5, 5};
static Point2D test_point3 = {-5, 5};
static Point2D test_point4 = {-5, -5};
static Point2D test_point5 = {5, -5};
static Rectangle plane;

typedef struct PRQuadtree_ PRQuadtree;

PRQuadtree *prquadtree_init(Rectangle boundary);
void prquadtree_destroy(PRQuadtree *tree);

void prquadtree_add(PRQuadtree *tree, Point2D point);
bool prquadtree_search(const PRQuadtree *tree, const Point2D point);
void prquadtree_remove(PRQuadtree *tree, const Point2D point);

// range query
size_t prquadtree_size(const PRQuadtree *tree);
bool prquadtree_is_empty(const PRQuadtree *tree);

size_t prquadtree_height(const PRQuadtree *tree);

typedef struct Node_
{
    // null = WHITE
    union Contents
    {
        struct
        {
            Point2D points[4];
            size_t cur_size;
        } element_record;
        struct Node_ *children[4];
    };
    Contents *contents;
    enum
    {
        BLACK,
        WHITE,
        GRAY
    } node_kind;
} Node;

struct PRQuadtree_
{
    Node *root;
    size_t size, max_size;
    Rectangle boundaries;
};
typedef enum
{
    X,
    Y,
    Z
} dims;
typedef enum
{
    SW,
    NE,
    SE,
    NW
} direction2D;
void get_center(Rectangle rect, Point2D center)
{
    center[X] = (rect[SW][X] + rect[NE][X]) / 2;
    center[Y] = (rect[SW][Y] + rect[NE][Y]) / 2;
}
direction2D quadrant(const Point2D center, const Point2D point)
{
    bool right = point[X] - center[X] > 0;
    bool above = point[Y] - center[Y] > 0;

    return right ? above ? NE : SE : above ? NW
                                           : SW;
}
void build_rectangle(Rectangle r1, double swx, double swy, double nex, double ney)
{
    r1[SW][X] = swx;
    r1[SW][Y] = swy;
    r1[NE][X] = nex;
    r1[NE][Y] = ney;
}

void split_rectangle(Rectangle rect, Rectangle rects[4])
{
    Point2D center;
    get_center(rect, center);

    /*
     * ---------
     * |NW |NE |
     * ---------
     * |SW |SE |
     * ---------
     */
    // rects are in SW, NE, SE, NW order
    build_rectangle(rects[SW], rect[SW][X], rect[SW][Y], center[X], center[Y]);
    build_rectangle(rects[NE], center[X], center[Y], rect[NE][X], rect[NE][Y]);
    build_rectangle(rects[SE], center[X], rect[SW][Y], rect[NE][X], center[Y]);
    build_rectangle(rects[NW], rect[SW][Y], center[X], center[Y], rect[NE][X]);
}
void copy_rectangle(Rectangle r1, Rectangle r2)
{
    memcpy(r1, r2, sizeof(*r2) * 2);
}
static Node *find_loc(Node *cur, Rectangle rect, const Point2D point)
{
    Point2D center;
    Rectangle rectangles[4];

    while (cur->node_kind == Node::GRAY)
    {
        get_center(rect, center);
        direction2D cur_quadrant = quadrant(center, point);

        // split the quadrant and move to the correct one for recursion
        split_rectangle(rect, rectangles);
        copy_rectangle(rect, rectangles[cur_quadrant]);
        cur = cur->contents->children[cur_quadrant];
    }

    return cur;
}

PRQuadtree *prquadtree_init(Rectangle boundary)
{
    PRQuadtree *tree = new PRQuadtree; //  (PRQuadtree *)malloc(sizeof(*tree));

    tree->root = new Node; // malloc(sizeof(*tree->root));
    tree->root->node_kind = Node::WHITE;
    tree->size = tree->max_size = 0;
    copy_rectangle(tree->boundaries, boundary);

    return tree;
}

void prquadtree_destroy(PRQuadtree *tree)
{
    stack<Node *> s; //*s = stack_init(tree->max_size + 1, NULL);
    s.push(tree->root);
    // stack_push(s, tree->root);

    while (!s.empty())
    {
        Node *cur = s.top();
        s.pop();

        // fallthroughs on purpose
        switch (cur->node_kind)
        {
        case Node::GRAY:
            for (int i = 0; i < 4; i++)
            {
                // stack_push(s, cur->contents->children[i]);
                s.push(cur->contents->children[i]);
            }
            __attribute__((fallthrough));
        case Node::BLACK:
            free(cur->contents);
            __attribute__((fallthrough));
        case Node::WHITE:
            free(cur);
        }
    }

    delete tree;
}

static void prquadtree_add_r(Node *subtree, Rectangle rect, Point2D point)
{
    Node *cur = find_loc(subtree, rect, point);

    switch (cur->node_kind)
    {
    case Node::WHITE:
        cur->node_kind = Node::BLACK;
        cur->contents = new Node::Contents; // malloc(sizeof(Node::Contents));
        cur->contents->element_record.cur_size = 0;
        memcpy(
            cur->contents->element_record.points[cur->contents->element_record.cur_size++],
            point,
            2 * sizeof(*point));
        break;
    case Node::BLACK:;
        size_t cur_size = cur->contents->element_record.cur_size;
        // need to decide if splitting or not
        if (cur_size == 4)
        {
            cur->node_kind = Node::GRAY;
            Point2D points[4];
            memcpy(points, cur->contents->element_record.points, 4 * sizeof(*points));
            for (size_t i = 0; i < 4; i++)
            {
                cur->contents->children[i] = new Node; // malloc(sizeof(Node));
                cur->contents->children[i]->node_kind = Node::WHITE;
            }
            for (size_t i = 0; i < 4; i++)
            {
                Rectangle child_copy;
                copy_rectangle(child_copy, rect);
                prquadtree_add_r(cur, child_copy, points[i]);
            }
            prquadtree_add_r(cur, rect, point);
        }
        else
        {
            memcpy(cur->contents->element_record.points[cur_size], point, 2 * sizeof(*point));
            cur->contents->element_record.cur_size++;
        }
        break;
        // impossible case
    }
}

void prquadtree_add(PRQuadtree *tree, Point2D point)
{
    Rectangle rect;
    copy_rectangle(rect, tree->boundaries);
    prquadtree_add_r(tree->root, rect, point);
    tree->size++;
    if (tree->size > tree->max_size)
    {
        tree->max_size = tree->size;
    }
}

typedef int (*comp_fn)(const void *key1, const void *key2, const void *ctx);
typedef bool (*eq_fn)(const void *v1, const void *v2, const void *ctx);

typedef struct
{
    eq_fn eq;
    const void *ctx;
} eq_ctx;

typedef struct
{
    comp_fn comp;
    const void *ctx;
} comp_ctx;
struct array_eq
{
    size_t nmemb, item_size;
    const eq_ctx *item_eq;
};

struct array_eq point2d_dims = {
    2, 0, NULL};
static inline bool eq_d(double d1, double d2)
{
    return fabs(d1 - d2) < eps;
}
static inline int comp_d(double d1, double d2)
{
    return eq_d(d1, d2) ? 0 : d1 - d2;
}

int array_ind(void *arr, size_t nmemb, size_t item_size, const void *key,
              const eq_ctx *eq_ctx)
{
    for (size_t i = 0; i < nmemb; i++)
    {
        if (eq_ctx->eq(key, (char *)arr + i * item_size, eq_ctx->ctx))
        {
            return i;
        }
    }

    return -1;
}
bool array_contains(void *arr, size_t nmemb, size_t item_size, const void *key, const eq_ctx *eq_ctx)
{
    return array_ind(arr, nmemb, item_size, key, eq_ctx) != -1;
}
bool eq_p(size_t k, const double *ds1, const double *ds2)
{
    for (size_t i = 0; i < k; i++)
    {
        if (!eq_d(ds1[i], ds2[i]))
        {
            return false;
        }
    }

    return true;
}

bool eq_p_casted(const void *ds1, const void *ds2, const void *ctx)
{
    struct array_eq *arr_ctx = (struct array_eq *)ctx;

    return eq_p(arr_ctx->nmemb, (const double *)ds1, (const double *)ds2);
}

eq_ctx point2d_eq_ctx;

bool prquadtree_search(const PRQuadtree *tree, const Point2D point)
{
    Rectangle rect;
    copy_rectangle(rect, (double(*)[2])tree->boundaries);
    Node *cur = find_loc(tree->root, rect, point);
    // return cur->node_kind == Node::WHITE ?false:true;

    return cur->node_kind == Node::WHITE ? false : array_contains(cur->contents->element_record.points, cur->contents->element_record.cur_size, sizeof(Point2D), point, &point2d_eq_ctx);
}

void prquadtree_remove(PRQuadtree *tree, const Point2D point)
{
    Rectangle rect;
    copy_rectangle(rect, tree->boundaries);
    Node *cur = find_loc(tree->root, rect, point);
    if (cur->node_kind == Node::BLACK)
    {
        int index = array_ind(
            cur->contents->element_record.points,
            cur->contents->element_record.cur_size,
            sizeof(Point2D),
            point,
            &point2d_eq_ctx);

        if (index == -1)
        {
            return;
        }

        if (cur->contents->element_record.cur_size == 1)
        {
            free(cur->contents);
            cur->node_kind = Node::WHITE;
        }
        else
        {
            /* shift all elements left one to override */
            memmove(cur->contents->element_record.points + index,
                    cur->contents->element_record.points + index + 1,
                    (cur->contents->element_record.cur_size-- - index - 1) * sizeof(Point2D));
        }
        tree->size--;
    }
}

// nn
//  knn

// range query

size_t prquadtree_size(const PRQuadtree *tree)
{
    return tree->size;
}

bool prquadtree_is_empty(const PRQuadtree *tree) { return tree->size == 0; }

size_t node_height(const Node *n)
{
    int max_child_height = -1;
    switch (n->node_kind)
    {
    case Node::WHITE:
        return -1;
    case Node::BLACK:
        return 0;
    case Node::GRAY:
        for (int i = 0; i < 4; i++)
        {
            int child_height = node_height(n->contents->children[i]);
            if (child_height > max_child_height)
            {
                max_child_height = child_height;
            }
        }
    }

    return 1 + max_child_height;
}

size_t prquadtree_height(const PRQuadtree *tree)
{
    return node_height(tree->root);
}

static void test_empty_quadtree()
{

    PRQuadtree *q = prquadtree_init(plane);

    assert(prquadtree_is_empty(q));
    assert(prquadtree_size(q) == 0);
    assert(prquadtree_search(q, origin) == NULL);

    prquadtree_destroy(q);
}

static void test_singleton_quadtree()
{
    PRQuadtree *q = prquadtree_init(plane);

    // insert up to three and check invaiants at each step
    assert(-1 == prquadtree_height(q));
    prquadtree_add(q, origin);
    assert(0 == prquadtree_height(q));
    assert(1 == prquadtree_size(q));
    prquadtree_add(q, test_point2);
    assert(0 == prquadtree_height(q));
    assert(2 == prquadtree_size(q));
    prquadtree_add(q, test_point3);
    assert(0 == prquadtree_height(q));
    assert(3 == prquadtree_size(q));
    prquadtree_add(q, test_point4);
    assert(0 == prquadtree_height(q));
    assert(4 == prquadtree_size(q));

    assert(prquadtree_search(q, origin) == true);
    assert(prquadtree_search(q, test_point2) == true);
    assert(prquadtree_search(q, test_point3) == true);
    assert(prquadtree_search(q, test_point4) == true);
    assert(prquadtree_search(q, test_point5) == false);

    prquadtree_destroy(q);
}

static void test_single_split_quadtree()
{
    PRQuadtree *q = prquadtree_init(plane);

    prquadtree_add(q, test_point2);
    prquadtree_add(q, test_point3);
    prquadtree_add(q, test_point4);
    prquadtree_add(q, test_point5);
    prquadtree_add(q, origin);

    assert(prquadtree_search(q, origin));
    assert(prquadtree_search(q, test_point2));
    assert(prquadtree_search(q, test_point3));
    assert(prquadtree_search(q, test_point4));
    assert(prquadtree_search(q, test_point5));

    assert(1 == prquadtree_height(q));
    assert(5 == prquadtree_size(q));

    prquadtree_destroy(q);
}

static void test_repeated_split()
{
    Rectangle smaller = {
        {-10, -10},
        {10, 10}};
    PRQuadtree *q = prquadtree_init(smaller);

    Point2D q1_points[5] = {
        {1, 1},
        {2, 2},
        {3, 3},
        {4, 4},
        {6, 6},
    };

    for (size_t i = 0; i < 5; i++)
    {
        prquadtree_add(q, q1_points[i]);
    }

    for (size_t i = 0; i < 5; i++)
    {
        assert(prquadtree_search(q, q1_points[i]));
    }

    assert(2 == prquadtree_height(q));

    assert(prquadtree_size(q) == 5);

    prquadtree_destroy(q);
}

static void test_removal()
{
    Rectangle smaller = {
        {-10, -10},
        {10, 10}};
    PRQuadtree *q = prquadtree_init(smaller);

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
        prquadtree_add(q, q1_points[i]);
    }
    assert(prquadtree_height(q) == 3);

    prquadtree_remove(q, q1_points[5]);
    assert(prquadtree_size(q) == 5);
    assert(prquadtree_height(q) == 3);
    assert(prquadtree_search(q, q1_points[5])==NULL);

    for (int i = 0; i < 5; i++)
    {
        assert(prquadtree_search(q, q1_points[i]));
        prquadtree_remove(q, q1_points[i]);
        assert(prquadtree_size(q) == 4 - i);

        if (i == 3)
        {
            assert(prquadtree_height(q) == 2);
        }
    }

    prquadtree_destroy(q);
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
