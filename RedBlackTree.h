#include <iostream>
#include <string>
template <typename T>
class RedBlackNode {
   private:
    RedBlackNode<T>* parent;
    RedBlackNode<T>* left;
    RedBlackNode<T>* right;
    bool isred;
    T data;

    RedBlackNode* direct_insert(T value) {
        if (value < this->data) {
            if (this->left == nullptr) {
                this->left = new RedBlackNode<T>(value, this, true);
                return this->left;
            } else {
                return this->left->insert(value);
            }
        }

        if (value > this->data) {
            if (this->right == nullptr) {
                this->right = new RedBlackNode<T>(value, this, true);
                return this->right;
            } else {
                return this->right->insert(value);
            }
        }

        return nullptr;
    }

    RedBlackNode* uncle() {
        if (this->parent == nullptr) return nullptr;
        if (this->parent->parent == nullptr) return nullptr;
        return this->parent->parent->left == this->parent ? this->parent->parent->right : this->parent->parent->left;
    }

    void adjust() {
        if (this->parent == nullptr && this->isred) {
            std::cout << "root color adjust" << std::endl;
            this->isred = false;
            return;
        }

        if (!(this->isred && this->parent->isred)) {
            return;
        }

        RedBlackNode* uncle = this->uncle();
        RedBlackNode* grandpa = this->parent->parent;

        // uncle is red
        if (uncle != nullptr && uncle->isred) {
            std::cout << "uncle red adjust" << std::endl;
            this->parent->isred = false;
            uncle->isred = false;
            grandpa->isred = true;
            grandpa->adjust();
            return;
        }

        // uncle is black
        // LL
        if (this->parent->left == this && grandpa->left == this->parent) {
            std::cout << "LL adjust" << std::endl;
            RedBlackNode* oldparent = this->parent;

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
            T temp = grandpa->data;
            grandpa->data = this->data;
            this->data = temp;
            this->right = grandpa->right;
            this->parent->right = nullptr;
            this->parent = grandpa;
            grandpa->right = this;
            return;
        }

        // RL
        if (this->parent->left == this && grandpa->right == this->parent) {
            std::cout << "RL adjust" << std::endl;
            T temp = grandpa->data;
            grandpa->data = this->data;
            this->data = temp;
            this->left = grandpa->left;
            grandpa->left = this;
            this->parent->left = nullptr;
            this->parent = grandpa;
            return;
        }

        // RR
        if (this->parent->right == this && grandpa->right == this->parent) {
            std::cout << "RR adjust" << std::endl;
            RedBlackNode* oldparent = this->parent;
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

   public:
    RedBlackNode(T value, RedBlackNode* parent, bool isred) {
        this->parent = parent;
        this->left = nullptr;
        this->right = nullptr;
        this->isred = isred;
        this->data = value;
    };

    RedBlackNode* insert(T value) {
        RedBlackNode* newnode = this->direct_insert(value);
        if (newnode == nullptr) return nullptr;
        newnode->adjust();
        return newnode;
    }

    void debug(std::string prefix) {
        std::string path = prefix + std::to_string(this->data) + "(" + std::to_string(this->isred) + ")";

        std::cout << path;

        // leaf node
        if (this->right == nullptr && this->left == nullptr) {
            std::cout << "$";
        };

        std::cout << std::endl;

        if (this->left != nullptr) {
            this->left->debug(path + "/");
        }

        if (this->right != nullptr) {
            this->right->debug(path + "/");
        }
    }
};

template <typename T>
class RedBlackTree {
   private:
    RedBlackNode<T>* root;

   public:
    RedBlackTree(T v) {
        this->root = new RedBlackNode<T>(v, nullptr, false);
    }
    RedBlackNode<T>* insert(T value) {
        return this->root->insert(value);
    }

    void debug() {
        if (this->root != nullptr) {
            this->root->debug("");
        }
    }
};
