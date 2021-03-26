#ifndef BST_H
#define BST_H
#include <queue>
#include <stack>
#include <iostream>
#include <utility>
#include <cassert>
#include <vector>
template <typename Key, typename Value>
class BSTNode
{
public:
    Key key;
    Value val;
    BSTNode<Key, Value> *left, *right;
    BSTNode(Key k, Value v) : key(k), val(v) { left = right = nullptr; }
};
template <typename Key, typename Value>
class BSTree
{

private:
    using Node = BSTNode<Key, Value>;
    Node *_do_del(Node *to_be_del)
    {
        if (to_be_del == nullptr)
        {
            return nullptr;
        }
        Node *res = nullptr;
        if (to_be_del->left == nullptr)
        {
            res = to_be_del->right;
        }
        else if (to_be_del->right == nullptr)
        {
            res = to_be_del->left;
        }
        else
        {
            Node *temp = to_be_del->left;
            if (temp->right == nullptr)
            {
                res = temp;
                temp->right = to_be_del->right;
            }
            else
            {
                while (temp->right->right != nullptr)
                {
                    temp = temp->right;
                }
                res = temp->right;
                res->right = to_be_del->right;
                temp->right = res->left;
                res->left = to_be_del->left;
            }
        }
        delete to_be_del;
        return res;
    }

public:
    class Iterator
    {
    private:
        std::stack<std::pair<Node *, bool>> _stateful_stack;
        std::vector<Node *> snapshot;
        bool finished;
        int64_t cursor;

    public:
        Iterator(const Iterator &it)
        {
            this->finished = it.finished;
            this->_stateful_stack = it._stateful_stack;
            this->snapshot = it.snapshot;
            this->cursor = it.cursor;
        }
        Iterator(Node *root)
        {
            if (root != nullptr)
            {
                finished = false;
                this->_stateful_stack.push(std::make_pair(root, false));
            }
            else
            {
                finished = true;
            }
            cursor = -1;
            while (!_stateful_stack.empty())
            {
                auto &temp = _stateful_stack.top();
                if (temp.second == true)
                {
                    Node *t = temp.first;
                    snapshot.push_back(t);
                    _stateful_stack.pop();
                    if (t->right != nullptr)
                    {
                        _stateful_stack.push(std::make_pair(t->right, false));
                    }
                }
                else
                {
                    temp.second = true;
                    if (temp.first->left != nullptr)
                    {
                        _stateful_stack.push(std::make_pair(temp.first->left, false));
                    }
                }
            }
            _stateful_stack.push(std::make_pair(root, false));
        }
        Node *get()
        {
            // return this->_stateful_stack.top().second == false ? nullptr : this->_stateful_stack.top().first;
            // return snapshot[cursor];
            if (this->_stateful_stack.top().second == false) {
                expand();
            }
            return this->_stateful_stack.top().first;
        }
        bool has_next()
        {
            // if (this->_stateful_stack.empty())
            // {
            //     return false;
            // }
            // if (this->_stateful_stack.size() == 1 && this->_stateful_stack.top().second == true)
            // {
            //     return false;
            // }
            // return true;
            // return !this->finished;
            // std::cout << "cursor " << cursor << " snapshot size " << snapshot.size() << " " << (cursor < ((int64_t)(snapshot.size()))) << std::endl;
            // if (snapshot.empty())
            //     return false;
            // return (cursor < ((int64_t)(snapshot.size())) - 1);
            return !(this->finished);
        }
        void next()
        {
            if (this->_stateful_stack.top().second == true)
            {
                Node *temp = this->_stateful_stack.top().first;
                this->_stateful_stack.pop();
                if (temp->right != nullptr)
                {
                    this->_stateful_stack.push(std::make_pair(temp->right, false));
                }
            }
            if (this->_stateful_stack.top().second == false)
            {
                expand();
            }
            if ((this->_stateful_stack.size() == 1 && this->_stateful_stack.top().first->right == nullptr)) {
                this->finished = true;
            }
            // cursor++;
        }

    private:
        void expand()
        {
            assert(this->_stateful_stack.top().second == false);
            while (this->_stateful_stack.top().second == false) {
                this->_stateful_stack.top().second = true;
                if (this->_stateful_stack.top().first->left != nullptr) {
                    this->_stateful_stack.push(std::make_pair(this->_stateful_stack.top().first->left, false));
                }
            }
        }
    };

public:
    Iterator iterator()
    {
        return Iterator(this->root);
    }
    void add(Key k, Value v)
    {
        if (root == nullptr)
        {
            root = new Node(k, v);
            return;
        }
        Node *cur = root;
        while (true)
        {
            if (cur->key == k)
            {
                std::cout << "Duplicated key: " << k << " cover it with value: " << v << std::endl;
                cur->val = v;
                return;
            }
            else if (cur->key > k)
            {
                if (cur->left == nullptr)
                {
                    cur->left = new Node(k, v);
                    return;
                }
                else
                {
                    cur = cur->left;
                }
            }
            else
            {
                if (cur->right == nullptr)
                {
                    cur->right = new Node(k, v);
                    return;
                }
                else
                {
                    cur = cur->right;
                }
            }
        }
    }
    void del(const Key &k)
    {
        if (root == nullptr)
        {
            return;
        }
        if (root->key == k)
        {
            root = _do_del(root);
            return;
        }
        Node *cur = root;
        while (true)
        {
            Node *temp = nullptr;
            if (cur->key > k)
            {
                temp = cur->left;
                if (temp == nullptr)
                {
                    break;
                }
                else if (temp->key == k)
                {
                    cur->left = _do_del(temp);
                    break;
                }
                else
                {
                    cur = temp;
                }
            }
            else
            {
                temp = cur->right;
                if (temp == nullptr)
                {
                    break;
                }
                else if (temp->key == k)
                {
                    cur->right = _do_del(temp);
                    break;
                }
                else
                {
                    cur = temp;
                }
            }
        }
    }
    Value *find(const Key &k)
    {
        Node *cur = root;
        while (cur != nullptr)
        {
            if (cur->key == k)
            {
                break;
            }
            else if (cur->key > k)
            {
                cur = cur->left;
            }
            else
            {
                cur = cur->right;
            }
        }
        return cur == nullptr ? nullptr : &(cur->val);
    }
    void clear()
    {
        int count = 0;
        if (root == nullptr)
        {
            std::cout << "clearing a empty bst" << std::endl;
            return;
        }
        std::queue<Node *> cleaning_q;
        cleaning_q.push(root);
        while (!cleaning_q.empty())
        {
            Node *temp = cleaning_q.front();
            cleaning_q.pop();
            if (temp->left != nullptr)
            {
                cleaning_q.push(temp->left);
            }
            if (temp->right != nullptr)
            {
                cleaning_q.push(temp->right);
            }
            // std::cout << temp->key << ' ' << temp->val << std::endl;
            delete temp;
            count++;
        }
        root = nullptr;
        std::cout << count << std::endl;
    }
    BSTree()
    {
        root = nullptr;
    }
    ~BSTree()
    {
        this->clear();
    }

private:
    Node *root;
};
#endif