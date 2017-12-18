#ifndef BINARY_TREE_BINTREE_H
#define BINARY_TREE_BINTREE_H

#include <memory>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <ogdf/basic/Graph.h>
#include <ogdf/fileformats/GraphIO.h>
#include <ogdf/tree/TreeLayout.h>
#include <ogdf/layered/SugiyamaLayout.h>

#define NULL_RENDER 1

using namespace ogdf;

template<class T>
class BinTree
{
public:
    void print();

    int size() const
    {
        return tree_size(this->root);
    }

    int tree_height() const
    {
        return get_tree_height(this->root);
    }

    int average_height() const
    {
        return get_average_height(this->root, 1) / size();
    }

    int control_sum() const
    {
        return get_control_sum(this->root);
    }

    void drawGraph()
    {
        draw_graph(this->root);
    }

protected:
    class Leaf
    {
    public:
        Leaf(const T &buf, int data_index)
        {
            data = buf;
            index = data_index;
            Bal = 0;
        }

        Leaf *left;
        Leaf *right;
        int Bal;
        T data;
        int index = 0;
    };

    Leaf *root;

    int tree_size(Leaf *p) const;
    int get_tree_height(Leaf *p) const;
    int get_average_height(Leaf *p, int level) const;
    int get_control_sum(Leaf *p) const;
    void draw_graph(Leaf *p);
    void print_leftToRight(Leaf *p, int indent) const;
    ogdf::node fill_graph(Leaf *p);

    Leaf *searchElementByIndex(Leaf *p, const int &index);

    int index = 0;
    Graph G;

    std::vector<ogdf::node> NullNodes;
    std::string output_filename;

private:
    bool elem_found = false;
    Leaf *search_result;

};

//-----TYPENAME ALIAS DEFINED
template<typename T>
using Leaf = typename BinTree<T>::Leaf;
//----------------------------

template<typename T>
void BinTree<T>::print()
{
    print_leftToRight(this->root, 10);
}

template<typename T>
void BinTree<T>::print_leftToRight(Leaf *p, int indent) const
{
    if (p != nullptr) {
        this->print_leftToRight(p->right, indent + 10);

        if (indent)
            std::cout << std::setw(indent);
        std::cout << p->data << std::endl;

        this->print_leftToRight(p->left, indent + 10);
    }
}

template<typename T>
int BinTree<T>::tree_size(Leaf *p) const
{
    if (p == nullptr) return 0;
    else {
        return 1 + tree_size(p->left) + tree_size(p->right);
    }
}

template<typename T>
int BinTree<T>::get_tree_height(Leaf *p) const
{
    if (p == nullptr) return 0;
    else {
        return 1 + std::max(get_tree_height(p->left), get_tree_height(p->right));
    }
}

template<typename T>
int BinTree<T>::get_average_height(Leaf *p, int level) const
{
    int size = 0;
    if (p == nullptr) return 0;
    else
        return level + get_average_height(p->left, level + 1) + get_average_height(p->right, level + 1);
}

template<typename T>
int BinTree<T>::get_control_sum(Leaf *p) const
{
    int size = 0;
    if constexpr(std::is_same<T, std::string>::value)
        return 0;
    if (p == nullptr ) return 0;
    else
        return p->data + get_control_sum(p->left) + get_control_sum(p->right);
}

template<typename T>
void BinTree<T>::draw_graph(Leaf *p)
{
    fill_graph(this->root);

#if NULL_RENDER == 0
    for (auto i : NullNodes) {
        G.delNode(i);
    }
#endif

    GraphAttributes GA(G);
    GA.init(GraphAttributes::nodeGraphics |
        GraphAttributes::edgeGraphics |
        GraphAttributes::nodeLabel |
        GraphAttributes::nodeStyle |
        GraphAttributes::edgeType |
        GraphAttributes::edgeArrow |
        GraphAttributes::edgeStyle); // Create graph attributes for this graph

    for (node v = G.firstNode(); v; v = v->succ()) { // iterate through all the node in the graph
        GA.fillColor(v) = Color("#FFFF00");
        GA.height(v) = 25.0;
        GA.width(v) = 25.0; // set the width to 40.0
        GA.shape(v) = ogdf::Shape::Ellipse;

        const char *pchar;

        if (std::find(NullNodes.begin(), NullNodes.end(), v) != NullNodes.end()) {
            GA.fillColor(v) = Color("#FF0000");
            GA.width(v) = 35.0;
            pchar = "NULL";
            GA.label(v) = pchar;
        }
        else {
            std::string s;

            if constexpr(std::is_same<T, std::string>::value) {
                s = searchElementByIndex(this->root, v->index())->data;
            }

            if constexpr(std::is_same<T, int>::value) {
                s = to_string(searchElementByIndex(this->root, v->index())->data);
            }

            elem_found = false;
            pchar = s.c_str(); //use char const* as target type
            GA.label(v) = pchar;
        }
    }

    for (edge e = G.firstEdge(); e; e = e->succ())// set default edge color and type
    {
        GA.bends(e);
        GA.arrowType(e) = ogdf::EdgeArrow::Last;
        GA.strokeColor(e) = Color("#0000FF");
    }

    TreeLayout SL; //Compute a hierarchical drawing of G (using SugiyamaLayout)

    SL.call(GA);

    std::fstream fs(output_filename, std::ios::out);
    GraphIO::drawSVG(GA, fs);
}

template<typename T>
ogdf::node BinTree<T>::fill_graph(Leaf *p)
{
    if (p != nullptr) {
        node rooted = G.newNode(p->index);
        G.newEdge(rooted, fill_graph(p->left));
        G.newEdge(rooted, fill_graph(p->right));
        return rooted;
    }
    else { // если попали на nullptr, то создаем узел дерева без привязки
        // чтобы избежать неожиданного поведения
        node s = G.newNode(++this->index);
        this->NullNodes.push_back(s); // добавляем его в вектор NULL-узлов
        return s;
    }
}

template<typename T>
Leaf<T>* BinTree<T>::searchElementByIndex(Leaf *p, const int &index)
{
    if (p != nullptr) {
        searchElementByIndex(p->left, index);
        searchElementByIndex(p->right, index);

        if (p->index == index && !elem_found) {
            elem_found = true;
            search_result = p;
            return search_result;
        }
        if (elem_found) {
            return search_result;
        }
    }
    return nullptr;
}

template<typename T>
class IdealTree: public BinTree<T>
{
public:
    IdealTree<T>(int L, int R, const std::vector<T> &A, const std::string &filename)
    {
        this->output_filename = filename;
        this->index = 0;
        BinTree<T>::root = IDSP(L, R, A);
    }

    IdealTree<T>(const std::vector<T> &A, const std::string &filename)
    {
        this->output_filename = filename;
        this->index = 0;
        BinTree<T>::root = IDSP(0, static_cast<int>(A.size() - 1), A);
    }
private:
    Leaf<T> *IDSP(int L, int R, const std::vector<T> &A);
};

template<typename T>
Leaf<T> *IdealTree<T>::IDSP(int L, int R, const std::vector<T> &A)
{
    int m;
    Leaf<T> *p;

    if (L > R)
        p = nullptr;
    else {
        m = (L + R) / 2;
        Leaf<T> *buf = (new Leaf<T>(A[m], this->index++));
        p = buf;

        p->left = IDSP(L, m - 1, A);

        p->right = IDSP(m + 1, R, A);
    }
    return p;
}

//RANDOM TREE BEGIN

template<typename T>
class RandomTree: public BinTree<T>
{
public:
    explicit RandomTree<T>(const std::vector<T> &a, const std::string &filename)
    {
        this->output_filename = filename;

        for (int i = 0; i < a.size(); i++)
            RDP(a[i]);
    }
    RandomTree<T>()
    {};

    void deleteElem(T data);

    void addElem(T data)
    {
        RDP(data);
    }
protected:
    void RDP(T data);
};

template<typename T>
void RandomTree<T>::RDP(T data)
{
    Leaf<T> **p = &(this->root);

    //Leaf<T> *buf = new Leaf<T>(data, index);

    while (*p != nullptr) {
        if (data < (*p)->data) {
            p = &((*p)->left);
        }
        else if (data > (*p)->data) {
            p = &((*p)->right);
        }
        else {
            break;
        }
    }
    if (*p == nullptr) {
        *p = new Leaf<T>(data, this->index++);
        (*p)->left = nullptr;
        (*p)->right = nullptr;
    }
}

template<typename T>
void RandomTree<T>::deleteElem(T data)
{
    Leaf<T> **p = &(this->root);

    Leaf<T> *q, *r, *S;

    while (*p != nullptr) {
        if (data < (*p)->data) {
            p = &((*p)->left);
        }
        else if (data > (*p)->data) {
            p = &((*p)->right);
        }
        else {
            break;
        }
    }
    if (*p != nullptr) {
        q = *p;
        if (q->left == nullptr) {
            *p = q->right;
        }
        else if (q->right == nullptr) {
            *p = q->left;
        }
        else {
            r = q->left;
            S = q;
            if (r->right == nullptr) {
                r->right = q->right;
                *p = r;
            }
            else {
                while (r->right != nullptr) {
                    S = r;
                    r = r->right;
                }
                S->right = r->left;
                r->left = q->left;
                r->right = q->right;
                *p = r;
            }
        }
        delete (q);
    }
}
// RANDOM TREE END


template<typename T>
class AVLTree: public BinTree<T>
{
public:

    explicit AVLTree(const std::vector<T> &vec, const std::string &filename)
    {
        this->output_filename = filename;

        for (auto p : vec) {
            AVL(this->root, p);
        }
    }

    bool rost;

    void deleteElem(T data)
    {
        delElem(data, this->root, down);
    }

    void delElem(const T &x, Leaf<T> *&p, bool &down);

    void AVL(Leaf<T> *&p, T data);

    void del(Leaf<T> *&r, bool &down, Leaf<T> *&q);

    void LLTurn(Leaf<T> **buf);

    void RRTurn(Leaf<T> **buf);

    void LRTurn(Leaf<T> **buf);

    void RLTurn(Leaf<T> **buf);

    void BR(Leaf<T> *&buf, bool &down);

    void BL(Leaf<T> *&buf, bool &down);

    void LL1(Leaf<T> *&buf, bool &down);

    void RR1(Leaf<T> *&buf, bool &down);

    bool down = false;
};

template<typename T>
void AVLTree<T>::AVL(Leaf<T> *&p, T data)
{
    if (p == nullptr) {
        p = new Leaf<T>(data, this->index++);
        p->left = nullptr;
        p->right = nullptr;
        p->Bal = 0;
        rost = true;
    }
    else {
        if (p->data > data) {
            AVL(p->left, data);
            if (rost) {
                if (p->Bal > 0) {
                    p->Bal = 0;
                    rost = false;
                }
                else {
                    if (p->Bal == 0) {
                        p->Bal = -1;
                        rost = true;
                    }
                    else {
                        if (p->left->Bal < 0) {
                            LLTurn(&p);
                            rost = false;
                        }
                        else {
                            LRTurn(&p);
                            rost = false;
                        }
                    }
                }
            }
        }
        else {
            if (p->data < data) {
                AVL(p->right, data);
                if (rost) {
                    if (p->Bal < 0) {
                        p->Bal = 0;
                        rost = false;
                    }
                    else {
                        if (p->Bal == 0) {
                            p->Bal = 1;
                            rost = true;
                        }
                        else {
                            if (p->right->Bal > 0) {
                                RRTurn(&p);
                                rost = false;
                            }
                            else {
                                RLTurn(&p);
                                rost = false;
                            }
                        }
                    }
                }
            }
        }
    }
}

template<typename T>
void AVLTree<T>::LLTurn(Leaf<T> **buf)
{
    auto **p = &(*buf);
    Leaf<T> *q;
    q = (*p)->left;
    (*p)->Bal = 0;
    q->Bal = 0;
    (*p)->left = q->right;
    q->right = *p;
    *p = q;
}

template<typename T>
void AVLTree<T>::RRTurn(Leaf<T> **buf)
{
    Leaf<T> **p = &(*buf);
    Leaf<T> *q;
    q = (*p)->right;
    (*p)->Bal = 0;
    q->Bal = 0;
    (*p)->right = q->left;
    q->left = (*p);
    (*p) = q;
}

template<typename T>
void AVLTree<T>::LRTurn(Leaf<T> **buf)
{
    Leaf<T> **p = &(*buf);
    Leaf<T> *q, *r;
    q = (*p)->left;
    r = q->right;
    if (r->Bal < 0) {
        (*p)->Bal = 1;
    }
    else {
        (*p)->Bal = 0;
    }
    if (r->Bal > 0) {
        q->Bal = -1;
    }
    else {
        q->Bal = 0;
    }
    q->right = r->left;
    (*p)->left = r->right;
    r->left = q;
    r->right = (*p);
    (*p) = r;
}

template<typename T>
void AVLTree<T>::RLTurn(Leaf<T> **buf)
{
    Leaf<T> **p = &(*buf);
    Leaf<T> *q, *r;
    q = (*p)->right;
    r = q->left;
    if (r->Bal < 0) {
        (*p)->Bal = 1;
    }
    else {
        (*p)->Bal = 0;
    }
    if (r->Bal > 0) {
        q->Bal = -1;
    }
    else {
        q->Bal = 0;
    }
    q->left = r->right;
    (*p)->right = r->left;
    r->right = q;
    r->left = (*p);
    (*p) = r;
}

template<typename T>
void AVLTree<T>::BR(Leaf<T> *&buf, bool &down)
{
    if (buf->Bal == 1)
        buf->Bal = 0;
    else if (buf->Bal == 0) {
        buf->Bal = -1;
        down = false;
    }
    else if (buf->Bal == -1) {
        if (buf->left != nullptr) {
            if (buf->left->Bal <= 0)
                LL1(buf, down);
            else
                LRTurn(&buf);
        }
    }
}

template<typename T>
void AVLTree<T>::BL(Leaf<T> *&buf, bool &down)
{
    if (buf->Bal == -1)
        buf->Bal = 0;
    else if (buf->Bal == 0) {
        buf->Bal = 1;
        down = false;
    }
    else if (buf->Bal == 1) {
        if (buf->left != nullptr) {
            if (buf->left->Bal >= 0)
                RR1(buf, down);
            else
                RLTurn(&buf);
        }
    }
}

template<typename T>
void AVLTree<T>::LL1(Leaf<T> *&buf, bool &down)
{
    Leaf<T> *q = buf->left;
    if (q->Bal == 0) {
        buf->Bal = 1;
        q->Bal = -1;
        down = false;
    }
    else {
        buf->Bal = 0;
        q->Bal = 0;
        buf->left = q->right;
        q->right = buf;
        buf = q;
    }
}

template<typename T>
void AVLTree<T>::RR1(Leaf<T> *&buf, bool &down)
{
    Leaf<T> *q = buf->right;

    if (q->Bal == 0) {
        buf->Bal = 1;
        q->Bal = 1;
        down = false;
    }
    else {
        buf->Bal = 0;
        q->Bal = 0;
    }
    buf->right = q->left;
    q->left = buf;
    buf = q;
}

template<typename T>
void AVLTree<T>::del(Leaf<T> *&r, bool &down, Leaf<T> *&q)
{
    if (r->right != nullptr) {
        del(r->right, down, q);
        if (down)
            BR(r, down);
    }
    else {
        q->data = r->data;
        q = r;
        r = r->left;
        down = true;
    }
}

template<typename T>
void AVLTree<T>::delElem(const T &x, Leaf<T> *&p, bool &down)
{
    Leaf<T> *q, *r;

    if (p == nullptr) {
        std::cout << "No key found!" << std::endl;
        return;
    }
    else if (p->data > x) {
        delElem(x, p->left, down);
        if (down) {
            std::cout << "BL - rotation" << std::endl;
            BL(p, down);
        }
    }
    else if (p->data < x) {
        delElem(x, p->right, down);
        if (down) {
            std::cout << "BR - rotation" << std::endl;
            BR(p, down);
        }
    }
    else {
        q = p;
        if (q->right == nullptr) {
            p = q->left;
            down = true;
        }
        else if (q->left == nullptr) {
            p = q->right;
            down = true;
        }
        else {
            del(q->left, down, q);
            if (down) {
                std::cout << "BL - rotation" << std::endl;
                BL(p, down);
            }
        }
        delete q;
    }
}

// AVL TREE END

// OPTIMAL SEARCH TREE

template<typename T>
class OptimalTree: public RandomTree<T>
{
public:
    explicit OptimalTree(const std::vector<T> &vec, const std::string &filename);

private:
    int **AW, **AR, **AP;
    void perform_creation(int L, int R, const std::vector<T> &V);
    void output_debug();
    int size_n = 0;
};

template<typename T>
OptimalTree<T>::OptimalTree(const std::vector<T> &vec, const std::string &filename)
{
    this->output_filename = filename;

    int n = vec.size();
    this->size_n = n;

    AW = new int *[n];
    AR = new int *[n];
    AP = new int *[n];

    for (int i = 0; i < n; i++) {
        AW[i] = new int[n];
        AP[i] = new int[n];
        AR[i] = new int[n];
    }

    //AW matrix calculation
    int i, j;
    for (i = 0; i < n; i++) {
        for (j = i + 1; j < n; j++) {
            AW[i][j] = 0;
            AP[i][j] = 0;
            AR[i][j] = 0;
        }
    }

    for (i = 0; i < n; i++) {
        for (j = i + 1; j < n; j++) {
            AW[i][j] = AW[i][j - 1] + vec[j];
        }
    }

    //AP & AR matrix calculation

    int h = 0;

    for (i = 0; i < n; i++) {
        j = i + 1;
        AP[i][j] = AW[i][j];
        AR[i][j] = j;
    }

    int m, min, k, x;

    for (h = 1; h < n; h++) {
        for (i = 0; i < n - h; i++) {
            j = i + h + 1;
            m = AR[i][j - 1];
            min = AP[i][m - 1] + AP[m][j];

            for (k = m; k < AR[i + 1][j]; k++) {
                x = AP[i][k - 1] + AP[k][j];
                if (x < min) {
                    m = k;
                    min = x;
                }
            }

            AP[i][j] = min + AW[i][j];
            AR[i][j] = m;
        }
    }

    perform_creation(0, n - 1, vec);
}

template<typename T>
void OptimalTree<T>::perform_creation(int L, int R, const std::vector<T> &V)
{
    int k = 0;
    if (L < R) {
        k = AR[L][R];
        this->addElem(V[k - 1]);

        perform_creation(L, k - 1, V);
        perform_creation(k, R, V);
    }
}

template<typename T>
void OptimalTree<T>::output_debug()
{
    std::cout << "AR matrix" << std::endl << std::setw(6);
    for (int i = 0; i < size_n; i++) {
        for (int j = 0; j < size_n; j++) {
            std::cout << AR[i][j] << std::setw(6);
        }
        std::cout << std::endl;
    }

    std::cout << "AP matrix" << std::endl << std::setw(6);;
    for (int i = 0; i < size_n; i++) {
        for (int j = 0; j < size_n; j++) {
            std::cout << AP[i][j] << std::setw(6);
        }
        std::cout << std::endl;
    }
    std::cout << "AW matrix" << std::endl << std::setw(6);;
    for (int i = 0; i < size_n; i++) {
        for (int j = 0; j < size_n; j++) {
            std::cout << AW[i][j] << std::setw(6);
        }
        std::cout << std::endl;
    }
}


#endif //BINARY_TREE_BINTREE_H
