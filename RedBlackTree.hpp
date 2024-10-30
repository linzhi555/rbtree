#include <cassert>
#include <iostream>
#include <string>
#include <utility>

template <typename T>
class RBTree;

template <typename T>
struct RBNode {
    RBNode<T>* parent;
    RBNode<T>* left;
    RBNode<T>* right;
    bool isRed;
    bool isNull;
    T data;

    static RBNode* new_null(RBNode* parent) {
        RBNode* n = new RBNode();
        n->parent = parent;
        return n;
    }

    static RBNode* new_red(RBNode* parent, T data) {
        RBNode* n = new RBNode();
        n->isRed = true;
        n->isNull = false;
        n->parent = parent;
        n->data = data;
        return n;
    }

    static RBNode* new_black(RBNode* parent, T data) {
        RBNode* n = new_red(parent, data);
        n->isRed = false;
        return n;
    }

    // return a null leaf
    RBNode() {
        this->parent = nullptr;
        this->left = nullptr;
        this->right = nullptr;
        this->isNull = true;
        this->isRed = false;
    };

    T find_min() {
        assert(!this->isNull);
        if (this->left->isNull) return this->data;
        return this->left->find_min();
    }

    T find_max() {
        assert(!this->isNull);
        if (this->right->isNull) return this->data;
        return this->right->find_max();
    }

    bool is_leaf() {
        if (this->isNull) return false;
        return this->left->isNull && this->right->isNull;
    }

    // find the parent which RBNode* point this
    RBNode** parent_ptr_this() {
        if (this->parent == nullptr) return nullptr;
        return this->parent->left == this ? &(this->parent->left) : &(this->parent->right);
    }

    RBNode* find_uncle() {
        if (this->parent == nullptr) return nullptr;
        if (this->parent->parent == nullptr) return nullptr;
        return this->parent->parent->left == this->parent ? this->parent->parent->right : this->parent->parent->left;
    }

    RBNode* find_sibling() {
        if (this->parent == nullptr) return nullptr;
        return this->parent->left == this ? this->parent->right : this->parent->left;
    }

    RBNode* find(T value) {
        if (value == this->data)
            return this;

        else if (value < this->data) {
            if (this->left->isNull != true) {
                RBNode* res = this->left->find(value);
                if (res != nullptr) return res;
            }
        }

        else if (value > this->data) {
            if (this->right->isNull != true) {
                RBNode* res = this->right->find(value);
                if (res != nullptr) return res;
            }
        }

        return nullptr;
    }

    // insert the node directly, as if the tree is common binary search tree
    RBNode* direct_insert(T value) {
        if (value < this->data) {
            if (this->left->isNull) {
                RBNode<T>* newnode = RBNode<T>::new_red(this, value);
                newnode->left = this->left;
                newnode->right = RBNode<T>::new_null(newnode);

                this->left->parent = newnode;
                this->left = newnode;
                return this->left;
            } else {
                return this->left->insert(value);
            }
        }

        if (value > this->data) {
            if (this->right->isNull) {
                RBNode<T>* newnode = RBNode<T>::new_red(this, value);
                newnode->left = RBNode<T>::new_null(newnode);
                newnode->right = this->right;

                this->right->parent = newnode;
                this->right = newnode;
                return this->right;
            } else {
                return this->right->insert(value);
            }
        }
        return nullptr;
    }

    // remove the node directly, as if the tree is common binary search tree
    // return the null that replace the original tree node, and if need to solve diff black confict
    std::pair<RBNode*, bool> direct_remove(T value) {
        RBNode* node = this->find(value);
        if (node == nullptr) return std::make_pair(nullptr, false);

        if (node != this) return node->direct_remove(value);

        if (this->is_leaf()) {
            RBNode* parent = this->parent;
            if (parent == nullptr) return std::make_pair(nullptr, false);

            RBNode** parent_slot = this->parent_ptr_this();
            *parent_slot = RBNode<T>::new_null(this->parent);
            bool needSolve = !this->isRed;
            this->delete_all();
            return std::make_pair(*parent_slot, needSolve);
        }

        else if (!this->left->isNull) {
            T temp = this->left->find_max();
            this->data = temp;
            return this->left->direct_remove(temp);
        }

        else if (!node->right->isNull) {
            T temp = this->right->find_min();
            this->data = temp;
            return this->right->direct_remove(temp);
        }

        return std::make_pair(nullptr, false);
    }

    static void swap_value(RBNode* a, RBNode* b) {
        T temp = a->data;
        a->data = b->data;
        b->data = temp;
    }

    // solve confict that the tree lack one black node

    void solve_lack_black() {
        // root condition: do no thing if the all tree lack one black
        if (this->parent == nullptr) return;

        // if the node is red, we can simply change the color to solve the problem
        if (this->isRed) {
            this->isRed = false;
            return;
        }

        RBNode* sibi = this->find_sibling();

        // sibiling red ,  change the condition to sibiling black
        if (sibi->isRed) {
            RBNode* paren = this->parent;
            std::cout << "sibiling red adjust" << sibi->data << " "
                      << (this->isNull ? "null" : std::to_string(this->data)) << std::endl;
            RBNode::swap_value(sibi, paren);
            if (paren->left == sibi) {
                paren->left = sibi->left;
                sibi->left->parent = paren;
                sibi->left = sibi->right;
                sibi->right = this;
                this->parent = sibi;
                sibi->parent = paren;
                paren->right = sibi;
            } else {
                paren->right = sibi->right;
                sibi->right->parent = paren;
                sibi->right = sibi->left;
                sibi->left = this;
                this->parent = sibi;
                sibi->parent = paren;
                paren->left = sibi;
            }

            return this->solve_lack_black();
        }

        // sibiling's all child is black, we can change the sibiling color to red
        // then judge if we need to solve the parent layer conflict
        if (!sibi->left->isRed && !sibi->right->isRed) {
            std::cout << "sibiling all child black adjust" << std::endl;
            sibi->isRed = true;

            if (this->parent->isRed) {
                this->parent->isRed = false;
                return;
            }

            return this->parent->solve_lack_black();
        }

        // LL
        if (sibi == this->parent->left && sibi->left->isRed) {
            std::cout << "LL adjust" << std::endl;
            sibi->left->isRed = false;
            RBNode::swap_value(sibi, this->parent);
            this->parent->left = sibi->left;
            sibi->left->parent = this->parent;
            sibi->left = sibi->right;
            sibi->right = this;
            this->parent->right = sibi;
            this->parent = sibi;
            return;
        }

        // LR
        else if (sibi == this->parent->left && sibi->right->isRed) {
            std::cout << "LR adjust" << std::endl;
            RBNode* nephew = sibi->right;
            swap_value(nephew, this->parent);
            nephew->isRed = false;

            sibi->right = nephew->left;
            nephew->left->parent = sibi;

            nephew->left = nephew->right;
            nephew->right = this;
            this->parent->right = nephew;
            nephew->parent = this->parent;
            this->parent = nephew;
            return;
        }

        // RL
        else if (sibi == this->parent->right && sibi->left->isRed) {
            std::cout << "RL adjust" << std::endl;
            RBNode* nephew = sibi->left;
            swap_value(nephew, this->parent);
            nephew->isRed = false;

            sibi->left = nephew->right;
            nephew->right->parent = sibi;

            nephew->right = nephew->left;
            nephew->left = this;
            this->parent->left = nephew;
            nephew->parent = this->parent;
            this->parent = nephew;
            return;
        }

        // RR
        else if (sibi == this->parent->right && sibi->right->isRed) {
            std::cout << "RR adjust" << std::endl;
            sibi->right->isRed = false;
            RBNode::swap_value(sibi, this->parent);
            this->parent->right = sibi->right;
            sibi->right->parent = this->parent;
            sibi->right = sibi->left;
            sibi->left = this;
            this->parent->left = sibi;
            this->parent = sibi;
            return;
        }
    }

    void solve_color_conflict() {
        if (this->parent == nullptr && this->isRed) {
            std::cout << "root color adjust" << std::endl;
            this->isRed = false;
            return;
        }

        if (!(this->isRed && this->parent->isRed)) {
            return;
        }

        RBNode* uncle = this->find_uncle();
        RBNode* grandpa = this->parent->parent;

        // uncle is red
        if (uncle->isRed) {
            std::cout << "uncle red adjust" << std::endl;
            this->parent->isRed = false;
            uncle->isRed = false;
            grandpa->isRed = true;
            grandpa->solve_color_conflict();
            return;
        }

        // uncle is black
        // LL
        if (this->parent->left == this && grandpa->left == this->parent) {
            std::cout << "LL adjust" << std::endl;
            RBNode* oldparent = this->parent;

            T temp = grandpa->data;
            grandpa->data = oldparent->data;
            oldparent->data = temp;
            grandpa->left = this;
            this->parent = grandpa;
            oldparent->left = oldparent->right;
            oldparent->right = grandpa->right;
            if (oldparent->right != nullptr) oldparent->right->parent = oldparent;
            grandpa->right = oldparent;
            return;
        }

        // LR
        if (this->parent->right == this && grandpa->left == this->parent) {
            std::cout << "LR adjust" << std::endl;
            RBNode::swap_value(grandpa, this);
            delete this->right;
            this->right = grandpa->right;
            grandpa->right->parent = this;
            grandpa->right = this;
            this->parent->right = RBNode<T>::new_null(this->parent);
            this->parent = grandpa;
            return;
        }

        // RL
        if (this->parent->left == this && grandpa->right == this->parent) {
            std::cout << "RL adjust" << std::endl;
            RBNode::swap_value(grandpa, this);
            delete this->left;
            this->left = grandpa->left;
            grandpa->left->parent = this;
            grandpa->left = this;
            this->parent->left = RBNode<T>::new_null(this->parent);
            this->parent = grandpa;
            return;
        }

        // RR
        if (this->parent->right == this && grandpa->right == this->parent) {
            std::cout << "RR adjust" << std::endl;
            RBNode* oldparent = this->parent;
            grandpa->right = this;
            this->parent = grandpa;
            oldparent->right = oldparent->left;
            oldparent->left = grandpa->left;
            if (oldparent->left != nullptr) oldparent->left->parent = oldparent;
            grandpa->left = oldparent;
            T temp = grandpa->data;
            grandpa->data = oldparent->data;
            oldparent->data = temp;
            return;
        }
    }

    RBNode* insert(T value) {
        RBNode* newnode = this->direct_insert(value);
        if (newnode == nullptr) return nullptr;
        newnode->solve_color_conflict();
        return newnode;
    }

    bool remove(T value) {
        std::pair<RBNode*, bool> result = this->direct_remove(value);
        std::cout << "------after direct_remove-----\n";
        this->debug("");
        std::cout << "------                   -----\n";

        if (result.second) {
            std::cout << "need to solve the child of" << result.first->parent->data << "\n";
            result.first->solve_lack_black();
        }

        return true;
    }

    void delete_all() {
        if (this->left != nullptr) {
            this->left->delete_all();
        }
        if (this->right != nullptr) {
            this->right->delete_all();
        }
        delete this;
    }

    // **************************
    // below for unit test
    // **************************

    bool has_double_red() {
        if (this->left != nullptr) {
            if (this->isRed && this->left->isRed) return true;
            if (this->left->has_double_red()) return true;
        }

        if (this->right != nullptr) {
            if (this->isRed && this->right->isRed) return true;
            if (this->right->has_double_red()) return true;
        }

        return false;
    }

    // -1 means the tree can statisfy same black rule
    // +N  means the node has N black node in every longest path
    int check_black_same() {
        if (this->isNull) return 1;

        int leftNum = this->left->check_black_same();
        if (leftNum == -1) return -1;

        int rightNum = this->right->check_black_same();
        if (leftNum == -1) return -1;

        if (leftNum == rightNum) return this->isRed ? leftNum : leftNum + 1;

        return -1;
    }

    bool has_bare_null() {
        if (this->isNull) {
            return false;
        }

        if (this->left == nullptr) return true;
        if (this->left->has_bare_null()) return true;

        if (this->right == nullptr) return true;
        if (this->right->has_bare_null()) return true;

        return false;
    }

    bool has_paren_child_err() {
        if (this->isNull) {
            if (this->left != nullptr || this->right != nullptr) return true;
            return false;
        }

        if (this->left->parent != this) return true;
        if (this->left->has_paren_child_err()) return true;

        if (this->right->parent != this) return true;
        if (this->right->has_paren_child_err()) return true;

        return false;
    }

    void debug(std::string prefix) {
        if (this->parent != nullptr) {
            std::cout << "P:" << this->parent->data << " ";
        } else {
            std::cout << "root:";
        }

        if (this->isNull) {
            std::cout << prefix << "NULL" << std::endl;
            return;
        }
        std::string path = prefix + std::to_string(this->data) + "(" + std::to_string(this->isRed) + ")";

        std::cout << path;

        // leaf node
        if (this->right->isNull && this->left->isNull) std::cout << "$";

        std::cout << std::endl;
        this->left->debug(path + "/");
        this->right->debug(path + "/");
    }
};

template <typename T>
class RBTree {
   private:
    int len;
    RBNode<T>* root;

   public:
    RBTree() {
        this->root = nullptr;
        this->len = 0;
    }

    RBNode<T>* insert(T value) {
        if (this->root == nullptr) {
            this->len = 1;
            this->root = RBNode<T>::new_black(nullptr, value);
            this->root->left = RBNode<T>::new_null(this->root);
            this->root->right = RBNode<T>::new_null(this->root);

            return this->root;
        }

        return this->root->insert(value);
    }

    void debug() {
        if (this->root != nullptr) {
            this->root->debug("");
        }
    }

    bool remove(T value) {
        if (this->root != nullptr) {
            if (this->root->data == value && this->root->is_leaf()) {
                this->root->delete_all();
                this->root = nullptr;
                return true;
            }
            return this->root->remove(value);
        }
        return false;
    }

    // for tell the tree if has error with the red black tree rule;
    enum class Error {
        NO_ERR,
        RED_ROOT,
        DOUBLE_RED,
        DIFF_BLACK,
        BARE_NULL,
        WRONG_PAREN_CHILD,
    };

    static std::string err_to_str(Error err) {
        switch (err) {
        case Error::NO_ERR:
            return "NO_ERR";
        case Error::DOUBLE_RED:
            return "DOUBLE_RED";
        case Error::DIFF_BLACK:
            return "DIFF_BLACK";
        case Error::RED_ROOT:
            return "RED_ROOT";
        case Error::BARE_NULL:
            return "BARE_NULL";
        case Error::WRONG_PAREN_CHILD:
            return "WRONG_PAREN_CHILD";
        }
        return "some error occured in err_to_str";
    }

    Error check_tree_rules() {
        if (this->root == nullptr) return Error::NO_ERR;
        if (this->root->has_bare_null()) return Error::BARE_NULL;
        if (this->root->isRed) return Error::RED_ROOT;
        if (this->root->has_double_red()) return Error::DOUBLE_RED;
        if (this->root->check_black_same() < 0) return Error::DIFF_BLACK;
        if (this->root->has_paren_child_err()) return Error::WRONG_PAREN_CHILD;
        return Error::NO_ERR;
    }
};
