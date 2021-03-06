#ifndef RBT_H
#define RBT_H
#include <iostream>
#include <queue>
#include <stack>
#include <cassert>
// #define assert(x) (void)0
template <typename Key, typename Value>
class RBTNode
{
public:
    Key key;
    Value val;
    RBTNode<Key, Value> *left, *right;

public:
    RBTNode(Key k, Value v) : key(k), val(v), is_red(true) { left = right = nullptr; }
    bool is_red;
};
template <typename Key, typename Value>
class RBTree
{
private:
    using Node = RBTNode<Key, Value>;
    Node *root;

public:
    class Iterator
    {
    private:
        std::stack<std::pair<Node *, bool>> _stateful_stack;
        bool finished;

    public:
        Iterator(const Iterator &it)
        {
            this->finished = it.finished;
            this->_stateful_stack = it._stateful_stack;
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
        }
        Node *get()
        {
            if (this->_stateful_stack.top().second == false)
            {
                expand();
            }
            return this->_stateful_stack.top().first;
        }
        bool has_next()
        {
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
            if ((this->_stateful_stack.size() == 1 && this->_stateful_stack.top().first->right == nullptr))
            {
                this->finished = true;
            }
        }

    private:
        void expand()
        {
            assert(this->_stateful_stack.top().second == false);
            while (this->_stateful_stack.top().second == false)
            {
                this->_stateful_stack.top().second = true;
                if (this->_stateful_stack.top().first->left != nullptr)
                {
                    this->_stateful_stack.push(std::make_pair(this->_stateful_stack.top().first->left, false));
                }
            }
        }
    };

public:
    void del(const Key &k)
    {
        _del(k);
    }
    bool sanity_check()
    {
        if (check(root))
        {
            int64_t black_height = black_balanced(root);
            if (black_height >= 0)
            {
                // std::cout << "Tree black height is: " << black_height << std::endl;
                return true;
            }
            return false;
        }
        return false;
    }
    Iterator iterator()
    {
        return Iterator(this->root);
    }
    RBTree() : root(nullptr)
    {
    }
    ~RBTree() { this->clear(); }
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
    void add(const Key &k, Value v)
    {
        if (root == nullptr)
        {
            root = new Node(k, v);
            root->is_red = false;
            return;
        }
        std::stack<Node *> path_memo;
        path_memo.push(root);
        Node *cur = root;
        while (true)
        {
            if (cur->key == k)
            {
                // std::cout << "Duplicated key: " << k << " cover it with value: " << v << std::endl;
                cur->val = v;
                return;
            }
            else if (cur->key > k)
            {
                if (cur->left == nullptr)
                {
                    cur->left = new Node(k, v);
                    // do the adjust
                    adjust(path_memo);
                    return;
                }
                else
                {
                    cur = cur->left;
                    path_memo.push(cur);
                }
            }
            else
            {
                if (cur->right == nullptr)
                {
                    cur->right = new Node(k, v);
                    // do the adjust
                    adjust(path_memo);
                    return;
                }
                else
                {
                    cur = cur->right;
                    path_memo.push(cur);
                }
            }
        }
    }

private:
    void adjust(std::stack<Node *> &path_memo)
    {
        while (!path_memo.empty())
        {
            Node *cur = path_memo.top();
            path_memo.pop();
            Node *cur_left = cur->left, *cur_right = cur->right;
            Node *parent = path_memo.empty() ? nullptr : path_memo.top();
            if (cur->is_red == false)
            {
                // current node is a black node
                if (cur_right && cur_right->is_red)
                {
                    if (cur_left && cur_left->is_red)
                    {
                        // left and right children are all red in a black node
                        cur_left->is_red = false;
                        cur_right->is_red = false;
                        cur->is_red = true;
                    }
                    else
                    {
                        // only right child is red in a black node
                        cur->right = cur_right->left;
                        cur_right->left = cur;
                        cur->is_red = true;
                        cur_right->is_red = false;
                        if (parent)
                        {
                            if (parent->left == cur)
                            {
                                parent->left = cur_right;
                            }
                            else
                            {
                                parent->right = cur_right;
                            }
                            assert(parent->left || parent->right);
                        }
                        else
                        {
                            root = cur_right;
                        }
                    }
                }
                else
                {
                    break;
                }
            }
            else
            {
                // current node is a red node
                assert(!(parent && parent->is_red));
                // current node is a red node
                if (cur_left && cur_left->is_red)
                {
                    assert(parent);
                    // assert there aren't three red links, which is against the rbtree rule
                    assert(!(cur_right && cur_right->is_red));
                    parent->left = cur_right;
                    cur->right = parent;
                    cur_left->is_red = false;
                    if (path_memo.size() >= 2)
                    {
                        path_memo.pop();
                        Node *pp = path_memo.top();
                        if (pp->left == parent)
                        {
                            pp->left = cur;
                        }
                        else
                        {
                            pp->right = cur;
                        }
                        assert(pp->left || pp->right);
                    }
                    else
                    {
                        // up rising to the top, parent is root
                        cur->is_red = false;
                        root = cur;
                        break;
                    }
                }
                else if (cur_right && cur_right->is_red)
                {
                    assert(parent);
                    assert(!(cur_left && cur_left->is_red));
                    parent->left = cur_right->right;
                    cur->right = cur_right->left;
                    cur_right->left = cur;
                    cur_right->right = parent;
                    cur->is_red = false;
                    if (path_memo.size() >= 2)
                    {
                        path_memo.pop();
                        Node *pp = path_memo.top();
                        if (pp->left == parent)
                        {
                            pp->left = cur_right;
                        }
                        else
                        {
                            pp->right = cur_right;
                        }
                        assert(pp->left || pp->right);
                    }
                    else
                    {
                        // up rising to the top, parent is root
                        cur_right->is_red = false;
                        root = cur_right;
                        break;
                    }
                }
                else
                {
                    // no double red links
                    break;
                }
            }
        }
        if (root)
        {
            root->is_red = false;
        }
    }
private:
    bool check(Node *r)
    {
        if (!r)
        {
            return true;
        }
        if (r->is_red)
        {
            return false;
        }
        Node *left = r->left, *right = r->right;
        Node *ll = left ? left->left : nullptr, *lr = left ? left->right : nullptr;
        if (right && right->is_red)
        {
            return false;
        }
        if (left && left->is_red)
        {
            if ((ll && ll->is_red) || (lr && lr->is_red))
            {
                return false;
            }
            return check(ll) && check(lr) && check(right);
        }
        return check(left) && check(right);
    }
    int64_t black_balanced(Node *r)
    {
        if (!r)
        {
            return 0;
        }
        Node *left = r->left, *right = r->right;
        if (left == nullptr && right == nullptr)
        {
            return 1;
        }
        int64_t rt = black_balanced(right);
        if (left && left->is_red)
        {
            int64_t ll = black_balanced(left->left), lr = black_balanced(left->right);
            if (ll >= 0 && lr >= 0 && rt >= 0 && ll == lr && ll == rt)
            {
                return ll + 1;
            }
            else
            {
                return -1;
            }
        }
        int64_t lt = black_balanced(left);
        if (rt >= 0 && lt >= 0 && rt == lt)
        {
            return lt + 1;
        }
        else
        {
            return -1;
        }
    }
    Node *find_min(Node *r, std::stack<Node *> &path_memo)
    {
        assert(r);
        assert(path_memo.top()->left == r || path_memo.top()->right == r);
        Node *cur = r;
        while (cur->left)
        {
            path_memo.push(cur);
            cur = cur->left;
        }
        return cur;
    }
    Node *find_max(Node *r, std::stack<Node *> &path_memo)
    {
        assert(r);
        assert(path_memo.top()->left == r || path_memo.top()->right == r);
        Node *cur = r;
        while (cur->right)
        {
            path_memo.push(cur);
            cur = cur->right;
        }
        return cur;
    }
    void del_leaf(Node *leaf, std::stack<Node *> &path_memo)
    {
        // when you are a leaf, you can't have right child
        assert(!leaf->right);
        Node *to_del = leaf, *ptr = leaf->left;
        while (!path_memo.empty())
        {
            Node *parent = path_memo.top();
            assert(parent->left == to_del || parent->right == to_del);
            path_memo.pop();
            Node *sib;
            if (parent->left == to_del)
            {
                sib = parent->right;
                if (to_del->is_red)
                {
                    to_del->is_red = false;
                    parent->left = ptr;
                    break;
                }
                assert(!sib->is_red);
                if (sib->left && sib->left->is_red)
                {
                    sib->left->is_red = false;
                    parent->left = sib->left;
                    sib->left = parent->left->right;
                    parent->left->right = parent->left->left;
                    parent->left->left = ptr;
                    std::swap(parent->key, parent->left->key);
                    std::swap(parent->val, parent->left->val);
                    break;
                }
                else
                {
                    parent->left = sib;
                    parent->right = sib->right;
                    sib->right = sib->left;
                    sib->left = ptr;
                    sib->is_red = true;
                    std::swap(parent->key, parent->left->key);
                    std::swap(parent->val, parent->left->val);
                    to_del = parent;
                    ptr = parent;
                }
            }
            else
            {
                sib = parent->left;
                // In this case sibling is not a sib but a parent part
                if (sib->is_red)
                {
                    Node *parent_lp = sib;
                    sib = parent_lp->right;
                    if (sib->left && sib->left->is_red)
                    {
                        sib->left->is_red = false;
                        parent_lp->right = sib->left;
                        parent->right = sib;
                        sib->left = sib->right;
                        sib->right = ptr;
                        std::swap(parent->key, parent->right->key);
                        std::swap(parent->val, parent->right->val);
                    }
                    else
                    {
                        parent->left = parent_lp->left;
                        parent->right = parent_lp;
                        parent_lp->left = parent_lp->right;
                        parent_lp->right = ptr;
                        parent_lp->is_red = false;
                        parent_lp->left->is_red = true;
                        std::swap(parent->key, parent->right->key);
                        std::swap(parent->val, parent->right->val);
                    }
                    break;
                }
                else
                {
                    if (sib->left && sib->left->is_red)
                    {
                        sib->left->is_red = false;
                        parent->left = sib->left;
                        parent->right = sib;
                        sib->left = sib->right;
                        sib->right = ptr;
                        std::swap(parent->key, parent->right->key);
                        std::swap(parent->val, parent->right->val);
                        break;
                    }
                    else
                    {
                        sib->is_red = true;
                        parent->right = ptr;
                        to_del = parent;
                        ptr = parent;
                    }
                }
            }
        }
        delete leaf;
    }
    // first transform node to be deleted to leaf node then do the rebalance
    void _del(const Key &k)
    {
        if (root == nullptr)
        {
            return;
        }

        if (root->key == k && root->left == nullptr && root->right == nullptr)
        {
            delete root;
            root = nullptr;
            return;
        }
        std::stack<Node *> path_memo;
        Node *cur = root;
        while (true)
        {
            if (!cur)
            {
                break;
            }
            if (cur->key == k)
            {
                if (cur->left == nullptr && cur->right == nullptr)
                {
                    del_leaf(cur, path_memo);
                }
                else if (cur->right)
                {
                    path_memo.push(cur);
                    Node *leaf_to_del = find_min(cur->right, path_memo);
                    cur->key = leaf_to_del->key;
                    cur->val = leaf_to_del->val;
                    del_leaf(leaf_to_del, path_memo);
                }
                else
                {
                    path_memo.push(cur);
                    Node *leaf_to_del = find_max(cur->left, path_memo);
                    cur->key = leaf_to_del->key;
                    cur->val = leaf_to_del->val;
                    del_leaf(leaf_to_del, path_memo);
                }
                break;
            }
            else if (cur->key > k)
            {
                path_memo.push(cur);
                cur = cur->left;
            }
            else
            {
                path_memo.push(cur);
                cur = cur->right;
            }
        }
    }
};
#endif