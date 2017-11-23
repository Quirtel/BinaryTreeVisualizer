#include <iostream>
#include <memory>
#include <iomanip>
#include <vector>
#include <algorithm>
#include "bintree.h"
#include <cstdlib>

using namespace std;


int main(int argc, char *argv[]) {
    const int N = 10;

    srand(static_cast<unsigned int>(time(nullptr)));

    vector<int> vec;
    for (int i = 0; i < 100; i++)
    {
        vec.push_back(rand() % 400 + 100);
    }
    AVLTree<int> tree(vec, "test.svg");
    tree.print();
    tree.drawGraph();

    return 0;
}