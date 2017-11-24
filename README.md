# BinaryTreeVisualizer
This utility builds the tree from the given data and shows its visualization

# Screenshots
The example of building trees
![Imgur](https://i.imgur.com/XtWzD4z.png)
is an AVL-Tree :)

Also, you can use strings to build a tree:
![Imgur](https://i.imgur.com/e3aLLD7.png)

Easy, as never before!

A simple example of building the trees above:
```CPP
#include <iostream>
#include <vector>
#include "bintree.h"

using namespace std;

int main(int argc, char *argv[]) {

    vector<int> A{100,200,300,233,521,666};

    vector<string> B{"A","B","C","D","E"};

    AVLTree<int> tree(A, "test.svg");
    AVLTree<string> tree2(B, "test1.svg");

    tree.print();
    tree.drawGraph();
    tree2.print();
    tree2.drawGraph();

    return 0;
}
```

# Building the project
 - First, you need to install CMake
 - Clone the repository into a desired directory
 - launch the following commands:
 ```bash
 cmake -G "Unix Makefiles" -DBUILD_SHARED_LIBS=OFF -DCMAKE_BUILD_TYPE=Debug
 make -j 4
 ```
 - launch Binary_tree

run "make" if you want to rebuild the project(e.g. edited source code)
