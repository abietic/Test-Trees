#include "bst.h"
#include "rbt.h"
#include <iostream>
#include <fstream>
#include <cstdio>
int main(int argc, char **argv)
{
    auto tree_test_data = fopen("/home/abietic/trees/test_tree.txt", "r");
    uint64_t id;
    char buf[100];
    int ret;
    RBTree<uint64_t, std::string> bst;
    RBTree<std::string, uint64_t> rev_bst;
    int count = 0;
    while (((ret = fscanf(tree_test_data, "%ld %s\n", &id, buf)) != 0) && (ret != EOF))
    {
        // printf("%ld %s \n", id, buf);
        bst.add(id, buf);
        rev_bst.add(buf, id);
        count++;
    }
    std::cout << count << " data inserted." << std::endl;
    for(auto it = rev_bst.iterator(); it.has_next();){
        it.next();
        // std::cout << it.get()->key << ' ' << it.get()->val << std::endl;
        // bst.del(it.get()->val);
        count--;
    }
    std::cout << "rev_bst size: " << 10000-count << std::endl;
    for(auto it = bst.iterator(); it.has_next();){
        it.next();
        // std::cout << it.get()->key << ' ' << it.get()->val << std::endl;
    }
    std::cout << bst.sanity_check() << ' ' << rev_bst.sanity_check() << std::endl;
    return 0;
}