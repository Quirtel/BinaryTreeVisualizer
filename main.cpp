#include <iostream>
#include <memory>
#include <iomanip>
#include <vector>
#include <algorithm>
#include "bintree.h"
#include <cstdlib>

using namespace std;

int main() {
    const int N = 10;

    srand(static_cast<unsigned int>(time(nullptr)));

    vector<int> A{19,16,12,13,3};

    for (int i = 0; i < 20; i++)
    {
        A.push_back(rand() % 400 + 100);
    }

    for (int i = 0; i < A.size(); i++)
        cout << A[i] << " ";
    cout << endl;

    AVLTree<int> tree(A);

    tree.drawGraph();
    cout << "----------------------------" << endl;

    cout << "Размер дерева: " << tree.size() << endl;
    cout << "Высота дерева: " << tree.tree_height() << endl;
    cout << "Средняя высота дерева: " << tree.average_height() << endl;
    cout << "Контрольная сумма: " << tree.control_sum() << endl;

    return 0;
}