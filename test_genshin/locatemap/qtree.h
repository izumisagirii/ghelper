#ifndef QTREE_H
#define QTREE_H
#include <vector>
#include <iostream>
template<typename T>
struct Point{
    T x;
    T y;
    Point(){}
    void* item_ptr;
    Point(T _x, T _y, void* _p) :x(_x), y(_y), item_ptr(_p){}
};

template<typename T>
struct Node{
    Node* R[4];
    Point<T> pt;
    Node* parent;
};

template<typename ElemType>
class QuardTree
{
public:
    QuardTree();
    ~QuardTree();
    void Insert(const Point<ElemType>& pos);
    void BalanceInsert(const Point<ElemType>& pos );
    int nodeCount();
    int TPLS();
    int Height();
    void RegionResearch(std::vector<void*>& pointor ,ElemType left, ElemType right, ElemType botom, ElemType top, int& visitednum,int& foundnum);
    void clear();
private:
    Node<ElemType>* root;
    int Compare(const Node<ElemType>* node, const Point<ElemType>& pos);
    bool In_Region(Point<ElemType> t, ElemType left, ElemType right, ElemType botom, ElemType top);
    bool Rectangle_Overlapse_Region(ElemType L, ElemType R, ElemType B, ElemType T, ElemType left, ElemType right, ElemType botom, ElemType top);
    void RegionResearch(std::vector<void*>& pointor ,Node<ElemType>* t, ElemType left, ElemType right, ElemType botom, ElemType top, int& visitednum, int& foundnum);
    int Depth(Node<ElemType>* &);
    int nodeCount(const Node<ElemType>*);
    void clear(Node <ElemType>*& p);
    void Insert(Node<ElemType>*& , const Point<ElemType>& pos);
};

template<typename T>
QuardTree<T>::QuardTree()
{
        root = NULL;
}

template<typename T>
QuardTree<T>::~QuardTree()
{
        clear(root);
}

template<typename T>
int QuardTree<T>::TPLS()
{
        return Depth(root);
}

template<typename T>
int QuardTree<T>::Compare(const Node<T>* node, const Point<T>& pos)
{
        if (pos.x == node->pt.x && pos.y == node->pt.y) return 0;
        if (pos.x >= node->pt.x && pos.y>node->pt.y)  return 1;
        if (pos.x<node->pt.x  && pos.y >= node->pt.y) return 2;
        if (pos.x <= node->pt.x && pos.y<node->pt.y)  return 3;
        if (pos.x>node->pt.x  && pos.y <= node->pt.y) return 4;
        return -1;
}


template<typename T>
void QuardTree<T>::BalanceInsert(const Point<T>& pos)
{
        Node<T>* node = (Node<T>*)malloc(sizeof(Node<T>));
        node->R[0] = NULL;
        node->R[1] = NULL;
        node->R[2] = NULL;
        node->R[3] = NULL;
        node->parent = NULL;
        node->pt = pos;
        if (root == NULL)
        {
                root = node;
                return;
        }
        Node<T>* temp = root;
        int direction = Compare(temp, pos);
        if (direction == 0) return;
        while (temp->R[direction - 1] != NULL)
        {
                temp = temp->R[direction - 1];
                direction = Compare(temp, pos);
                if (direction == 0) return;
        }
        temp->R[direction - 1] = node;
        node->parent = temp;

        Node<T>* tp = temp->parent;
        if (tp == NULL) return;
        int r = Compare(tp, temp->pt);

        if (abs(direction-r) == 2)
        {
                Node<T>* leaf = node;
                if (tp->R[abs(3 - r)] == NULL )
                {
                        tp->R[r - 1] = NULL;
                        temp->parent = leaf;
                        leaf->R[r-1] = temp;

                        temp->R[abs(3 - r)] = NULL;
                        Node<T>* Rt = tp->parent;
                        if (Rt == NULL)
                        {
                                root = leaf;
                                leaf->parent = NULL;

                                leaf->R[abs(3 - r)] = tp;
                                tp->parent = leaf;
                                return;
                        }
                        tp->parent = NULL;
                        int dd = Compare(Rt, tp->pt);

                        Rt->R[dd - 1] = leaf;
                        leaf->parent = Rt;

                        leaf->R[abs(3 - r)] = tp;
                        tp->parent = leaf;
                }
        }
}


template<typename T>
void QuardTree<T>::Insert(Node<T>*& p, const Point<T>& pos)
{
        if (p == NULL)
        {
                Node<T>* node = (Node<T>*)malloc(sizeof(Node<T>));
                node->R[0] = NULL;
                node->R[1] = NULL;
                node->R[2] = NULL;
                node->R[3] = NULL;
                node->pt = pos;
                p = node;
                return;
        }
        else
        {
                int d = Compare(p, pos);
                if (d == 0) return;
                Insert(p->R[d - 1], pos);
        }
}
template<typename T>
void QuardTree<T>::Insert(const Point<T>& pos)
{
        int direction, len = 0;
        Node<T>* node = (Node<T>*)malloc(sizeof(Node<T>));
        node->R[0] = NULL;
        node->R[1] = NULL;
        node->R[2] = NULL;
        node->R[3] = NULL;
        node->pt = pos;
        if (root == NULL)
        {
                root = node;
                return;
        }
        direction = Compare(root, pos);
        Node<T>* temp = root;
        if (direction == 0) return;
        len = 1;
        while (temp->R[direction - 1] != NULL)
        {
                temp = temp->R[direction - 1];
                direction = Compare(temp, pos);
                if (direction == 0) return;
        }
        temp->R[direction - 1] = node;
        //Insert(root, pos);
}

template<typename T>
int QuardTree<T>::nodeCount()
{
        return nodeCount(root);
}

template<typename T>
int QuardTree<T>::nodeCount(const Node<T>* node)
{
        if (node == NULL) return 0;
        return 1 + nodeCount(node->R[0]) + nodeCount(node->R[1]) + nodeCount(node->R[2]) + nodeCount(node->R[3]);
}

template<typename T>
bool QuardTree<T>::In_Region(Point<T> t, T left, T right, T botom, T top)
{
        return t.x >= left && t.x <= right && t.y >= botom && t.y <= top;
}

template<typename ElemType>
bool QuardTree<ElemType>::Rectangle_Overlapse_Region(ElemType L, ElemType R, ElemType B, ElemType T,
        ElemType left, ElemType right, ElemType botom, ElemType top)
{
        return L <= right && R >= left && B <= top && T >= botom;
        //return true;
}

template<typename T>
void QuardTree<T>::RegionResearch(std::vector<void*>& pointor ,Node<T>* t, T left, T right, T botom, T top, int& visitednum, int& foundnum)
{
        if (t == NULL) return;
        T xc = t->pt.x;
        T yc = t->pt.y;
        if (In_Region(t->pt, left, right, botom, top)){
                ++foundnum;
                pointor.push_back(t->pt.item_ptr);
        }
        if (t->R[0] != NULL && Rectangle_Overlapse_Region(xc, right, yc, top, left, right, botom, top))
        {
                visitednum++;
                RegionResearch(pointor,t->R[0], xc>left?xc:left, right, yc>botom?yc:botom, top, visitednum, foundnum);
        }
        if (t->R[1] != NULL && Rectangle_Overlapse_Region(left, xc, yc, top, left, right, botom, top))
        {
                visitednum++;
                RegionResearch(pointor,t->R[1], left, xc>right?right:xc, yc>botom?yc:botom, top, visitednum, foundnum);
        }
        if (t->R[2] != NULL && Rectangle_Overlapse_Region(left, xc, botom, yc, left, right, botom, top))
        {
                visitednum++;
                RegionResearch(pointor,t->R[2], left, xc<right?xc:right, botom, yc<top?yc:top, visitednum, foundnum);
        }
        if (t->R[3] != NULL && Rectangle_Overlapse_Region(xc, right, botom, yc, left, right, botom, top))
        {
                visitednum++;
                RegionResearch(pointor,t->R[3], xc>left ? xc : left, right, botom, yc<top ? yc : top, visitednum, foundnum);
        }
}

template<typename T>
void QuardTree<T>::clear()
{
        clear(root);
}

template<typename T>
void QuardTree<T>::clear(Node<T>* &p)
{
        if (p == NULL) return;
        if (p->R[0]) clear(p->R[0]);
        if (p->R[1]) clear(p->R[1]);
        if (p->R[2]) clear(p->R[2]);
        if (p->R[3]) clear(p->R[3]);
        free(p);
        p = NULL;
}

template<typename T>
void QuardTree<T>::RegionResearch(std::vector<void*>& pointer, T left, T right, T botom, T top, int& visitednum, int& foundnum)
{
        RegionResearch(pointer ,root, left, right, botom, top, visitednum,foundnum);
}

template<typename T>
int QuardTree<T>::Depth(Node<T>* &node)
{
        if (node == NULL) return 0;
        int dep = 0;
        Node<T>* tp = root;
        while (tp->pt.x!=node->pt.x || tp->pt.y!=node->pt.y)
        {
                dep++;
                tp = tp->R[Compare(tp, node->pt) - 1];
                if (tp == NULL) break;
        }
        return dep + Depth(node->R[0]) + Depth(node->R[1]) + Depth(node->R[2]) + Depth(node->R[3]);
}
#endif
