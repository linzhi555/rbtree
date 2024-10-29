#include <string.h>
#include <cstdio>
#include <iostream>
#include <random>
#include "RedBlackTree.h"

int main() {
    std::cout << "hello world" << std::endl;

    int testNumber[] = { 18, 23, 34, 27, 15, 9, 6, 8, 5, 25, -1 };
    RedBlackTree<uint32_t> tree(17);
    for (int i = 0; testNumber[i] >= 0; i++) {
        uint32_t num = (uint32_t) testNumber[i];

        std::cout << "===============insert node num:" << num << std::endl;
        RedBlackNode<uint32_t>* nnode = tree.insert(num);

        if (nnode == NULL) {
            std::cout << "insert node fail" << num << std::endl;

        } else {
            std::cout << "after insert node:" << num << std::endl;
            tree.debug();
        }
    }
}
