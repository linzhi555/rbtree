#include <cassert>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <random>
#include "RedBlackTree.hpp"

int main() {
    std::cout << "hello world" << std::endl;

    std::vector<int> testNumber = { 17, 18, 23, 34, 27, 15, 9, 6, 8, 5, 25, -1 };

    std::cout << "=====insest test============" << std::endl;

    using RBT = RBTree<uint32_t>;
    using RBN = RBNode<uint32_t>;

    RBT tree;
    for (int i = 0; testNumber[i] >= 0; i++) {
        uint32_t num = (uint32_t) testNumber[i];

        std::cout << "--------insert node num:" << num << std::endl;
        RBN* nnode = tree.insert(num);

        if (nnode == NULL) {
            std::cout << "insert node fail" << num << std::endl;

        } else {
            std::cout << "after insert node:" << num << std::endl;
            tree.debug();

            RBT::Error err = tree.check_tree_rules();
            if (err != RBT::Error::NO_ERR) {
                std::cout << "break the rbtree rule ,ERROR: " << RBT::err_to_str(err) << std::endl;
            }
            assert(err == RBT::Error::NO_ERR);
        }
    }

    std::cout << "=====remove test============" << std::endl;
    for (int i = 0; testNumber[i] >= 0; i++) {
        uint32_t num = (uint32_t) testNumber[i];
        std::cout << "--------remove node num:" << num << std::endl;

        bool result = tree.remove(num);
        std::cout << "remove" << num << ":" << result << std::endl;
        assert(result == true);
        tree.debug();
        RBT::Error err = tree.check_tree_rules();
        if (err != RBT::Error::NO_ERR) {
            std::cout << "break the rbtree rule ,ERROR: " << RBT::err_to_str(err) << std::endl;
        }
        assert(err == RBT::Error::NO_ERR);
    }
}
