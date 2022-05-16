#pragma once

#include <iostream>
#include <cmath>
#include <stack>

#include <cstring>
#include <stdlib.h>
#include <stdbool.h>
#include <cassert>

using namespace std;

#define eps .00001

typedef double Point2D[2];
typedef Point2D Rectangle[2];

void copy_rectangle(Rectangle r1, Rectangle r2)
{
    memcpy(r1, r2, sizeof(*r2) * 2);
}
typedef struct Node_
{
    // null = VACUUM
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
        VACUUM,
        GRAY
    } node_kind;

    void print()
    {
        if (node_kind == BLACK)
        {
            printf("BLACK:cur_size=%d\n", contents->element_record.cur_size);
            for (int i = 0; i < contents->element_record.cur_size; i++)
            {
                cout << contents->element_record.points[i][0] << " " << contents->element_record.points[i][1] << endl;
            }
        }
        else if (node_kind == VACUUM)
        {
            printf("VACUUM%d\n", contents->element_record.cur_size);
            // for (int i = 0; i < contents->element_record.cur_size; i++)
            // {
            //     cout << contents->element_record.points[i][0] << " " << contents->element_record.points[i][1] << endl;
            // }
        }
        else if (node_kind == GRAY)
        {
            printf("GRAY:cur_size=%d\n", contents->element_record.cur_size);
            // for (int i = 0; i < contents->element_record.cur_size; i++)
            // {
            //     cout << contents->element_record.points[i][0] << " " << contents->element_record.points[i][1] << endl;
            // }
        }
    }
} Node;

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

Node *find_loc(Node *cur, Rectangle rect, const Point2D point);

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

static void prquadtree_add_r(Node *subtree, Rectangle rect, Point2D point)
{
    Node *cur = find_loc(subtree, rect, point);

    switch (cur->node_kind)
    {
    case Node::VACUUM:
        cur->node_kind = Node::BLACK;
        cur->contents = new Node::Contents;
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
            Point2D points[4];//拿出原来储存的点
            memcpy(points, cur->contents->element_record.points, 4 * sizeof(*points));
            for (size_t i = 0; i < 4; i++)
            {
                cur->contents->children[i] = new Node;
                cur->contents->children[i]->node_kind = Node::VACUUM;
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

typedef int (*comp_fn)(const void *key1, const void *key2, const void *ctx);
typedef bool (*eq_fn)(const void *v1, const void *v2, const void *ctx);

typedef struct
{
    // eq_fn eq;
    const void *ctx;
} eq_ctx;

typedef struct
{
    comp_fn comp;
    const void *ctx;
} comp_ctx;
struct array_eq
{
    size_t nmemb = 2, item_size = 0;
    const eq_ctx *item_eq = NULL;
};

struct array_eq point2d_dims;
static inline bool eq_d(double d1, double d2)
{
    return fabs(d1 - d2) < eps;
}

bool eq_p(size_t k, const double *ds1, const double *ds2); //判断多个是否相等

int array_ind(void *arr, size_t nmemb, size_t item_size, const void *key,
              const eq_ctx *eq_ctx)
{
    for (size_t i = 0; i < nmemb; i++)
    {
        struct array_eq *arr_ctx = (struct array_eq *)eq_ctx->ctx;
        if (eq_p(arr_ctx->nmemb, (const double *)key, (const double *)((char *)arr + i * item_size)))
        {
            return i;
        }
    }

    return -1;
}
bool eq_p(size_t k, const double *ds1, const double *ds2) //判断多个是否相等
{
    for (size_t i = 0; i < k; i++)
    {
        printf("%f,%f\n", ds1[i], ds2[i]);
        if (!eq_d(ds1[i], ds2[i])) //判断相等
        {
            return false;
        }
    }

    return true;
}

eq_ctx point2d_eq_ctx;

size_t node_height(const Node *n)
{
    int max_child_height = -1;
    switch (n->node_kind)
    {
    case Node::VACUUM:
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
Node *find_loc(Node *cur, Rectangle rect, const Point2D point)
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

struct PRQuadtree
{
public:
    Node *root;
    size_t size, max_size;
    Rectangle boundaries;
    PRQuadtree(Rectangle boundary)
    {
        root = new Node;
        root->node_kind = Node::VACUUM;
        size = max_size = 0;
        copy_rectangle(boundaries, boundary);
    }

    size_t prquadtree_height()
    {
        return node_height(root);
    }

    size_t prquadtree_size() { return this->size; }

    bool prquadtree_is_empty() { return this->size == 0; }

    ~PRQuadtree()
    {
        stack<Node *> s;
        s.push(root);
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
                delete (cur->contents);
                __attribute__((fallthrough));
            case Node::VACUUM:
                delete (cur);
            }
        }
    }
    bool prquadtree_search(const Point2D point)
    {
        Rectangle rect;
        copy_rectangle(rect, boundaries);
        Node *cur = find_loc(root, rect, point);
        if (cur->node_kind == Node::VACUUM)
            return false;

        // return cur->node_kind == Node::VACUUM ?false:true;

        // for (int i = 0; i < cur->contents->element_record.cur_size; i++)
        // {
        //     for(int j=0;j<4;j++)
        //     cout << cur->contents->element_record.points[j][0] << " -" << cur->contents->element_record.points[j][1] << endl;
        //     // cout << cur->contents->element_record.points[i][0] << " " << cur->contents->element_record.points[i][1] << endl;
        //     // cout << cur->contents->element_record.points[i][0] << " " << cur->contents->element_record.points[i][1] << endl;
        //     // cout << cur->contents->element_record.points[i][0] << " " << cur->contents->element_record.points[i][1] << endl;
        // }
        return array_ind(cur->contents->element_record.points, cur->contents->element_record.cur_size, sizeof(Point2D), point, &point2d_eq_ctx) != -1;
    }
    void prquadtree_add(Point2D point)
    {
        Rectangle rect;
        copy_rectangle(rect, boundaries);
        prquadtree_add_r(root, rect, point);
        size++;
        if (size > max_size)
        {
            max_size = size;
        }
    }

    void prquadtree_remove(const Point2D point)
    {
        Rectangle rect;
        copy_rectangle(rect, boundaries);
        Node *cur = find_loc(root, rect, point);
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
                delete (cur->contents);
                cur->node_kind = Node::VACUUM;
            }
            else
            {
                /* shift all elements left one to override */
                memmove(cur->contents->element_record.points + index,
                        cur->contents->element_record.points + index + 1,
                        (cur->contents->element_record.cur_size-- - index - 1) * sizeof(Point2D));
            }
            size--;
        }
    }
};
