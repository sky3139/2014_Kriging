
#include <iostream>
#include <vector>

using namespace std;

//-maxcoordinate <= x < maxcoordinate
//-maxcoordinate <= y < maxcoordinate

class PRquadtree
{
public:
    PRquadtree(int _maxcoordinate) : maxcoordinate(_maxcoordinate), k(0){};
    void insert(int x, int y);
    void write();
    int count()
    {
        if (k == 0)
            return 0;
        else
            return k->count();
    }

protected:
    class Node
    {
    public:
        virtual bool isLeaf() const = 0;
        virtual int count() const = 0;
    };
    class Leaf : public Node
    {
    public:
        Leaf(int _x, int _y) : x(_x), y(_y){};
        virtual bool isLeaf() const { return true; }
        int x, y; // co"ordinaten punt

        int count() const
        {
            return 1;
        }
    };
    class NonLeaf : public Node
    {
    public:
        NonLeaf()
        {
            for (int i = 0; i < 4; i++)
                child[i] = 0;
        }
        virtual bool isLeaf() const { return false; }
        void write()
        {
            int count = 0;
            for (int i = 0; i < 4; i++)
            {
                if (child[i] != 0)
                {
                    if (child[i]->isLeaf())
                    {
                        // cout << ++count << endl;
                        cout << "Leaf with index " << i << endl;
                        cout << "x: " << static_cast<Leaf *>(child[i])->x << " y: " << static_cast<Leaf *>(child[i])->y << endl;
                    }
                    else
                    {
                        //
                        cout << "NonLeaf with index " << i << endl;
                        static_cast<NonLeaf *>(child[i])->write();
                    }
                }
            }
        }
        int count() const
        {
            int countLeafs = 0;

            for (int i = 0; i < 4; i++)
            {
                if (child[i] != 0)
                {
                    if (!child[i]->isLeaf())
                    {
                        countLeafs += static_cast<NonLeaf *>(child[i])->count();
                    }
                    else
                    {
                        countLeafs++;
                    }
                }
            }

            return countLeafs;
        }

    public:
        static const int EAST, WEST, NORTH, SOUTH;
        Node *child[4]; // index with wind direction (example. child[NORTH+WEST] contains points
        // with x and y smaller then maxcoordinate)
        // empty area: nullpointer
    };
    const int maxcoordinate;
    Node *k;
};

const int EAST = 0;
const int WEST = 2;
const int NORTH = 0;
const int SOUTH = 1;

class Area
{
public:
    Area(int maxc) : xl(-maxc), xr(maxc), yl(-maxc), yr(maxc), xc(0), yc(0){};
    void makePartition(int x, int y)
    {
        if (x < xc)
            xr = xc;
        else
            xl = xc;
        if (y < yc)
            yr = yc;
        else
            yl = yc;
        xc = (xl + xr) / 2;
        yc = (yl + yr) / 2;
    };
    int getQuadrant(int x, int y) const
    {
        return (x < xc ? WEST : EAST) + (y < yc ? SOUTH : NORTH); // NORTH: SOUTH
    }

private:
    int xl, xr, yl, yr, xc, yc; // given borders Area; l,r,c: left, right, center.
};

void PRquadtree::insert(int x, int y)
{
    Node **h = &k;
    Area Area(maxcoordinate);
    int index;
    while (*h != 0)
    {
        if (!(*h)->isLeaf())
        {
            // niet-Leaf
            index = Area.getQuadrant(x, y);
            h = &(static_cast<NonLeaf *>(*h)->child[index]);
            Area.makePartition(x, y);
        }
        else
        {
            // Leaf
            Leaf *Leaf = static_cast<class Leaf *>(*h);

            if (Leaf->x == x && Leaf->y == y)
            {
                cout << "node already exists!" << endl;
                break;
            }
            *h = new NonLeaf();
            index = Area.getQuadrant(Leaf->x, Leaf->y);
            static_cast<NonLeaf *>(*h)->child[index] = Leaf;
        }
    }
    *h = new Leaf(x, y);
}

void PRquadtree::write()
{
    if (k != 0)
    {
        static_cast<NonLeaf *>(k)->write();
    }
}

int main(int argc, const char *argv[])
{
    PRquadtree tree(10);

    tree.insert(2, 3);
    tree.insert(1, 4);
    tree.insert(5, 8);
    tree.insert(3, 6);
    tree.insert(4, 7);

    cout << "Tree contains " << tree.count() << " nodes\n";

    return 0;
}
