#include "./base_struct/simple_queue.h"
#include "./base_struct/skip_list.h"
#include "./base_struct/binary_list.h"
#include <ctime>
#include <cstdlib>
#include <chrono>

struct dynamic_arr{
    int a;
    int b[0];
};

struct thread_param{
    simple_queue *queue;
    int  thread_id;
};

struct flist_node{
    BASE_NODE node;
    int32_t number;
};

#define MAX_ELEMENT_NUM 2000000
#define SEARCH_ELEMENT_NUM 1910000

struct test_node{
    int size;
    double score;
};

/*
 * if left euqal right return 0
 * if left greater than right return 1
 * if left smaller than right return -1
 * */
int sk_node_compare(sk_node* left, void* right)
{
    assert(left != nullptr);
    assert(right != nullptr);
    return ((test_node*)left->ele)->size - ((test_node*)right)->size;
}

void sk_node_print(sk_node* node)
{
    assert(node != nullptr);
    printf("%d ", ((test_node*)node->ele)->size);
}

int bin_node_compare(void* left, void* right)
{
    assert(left != nullptr);
    assert(right != nullptr);
    return ((test_node*)left)->size - ((test_node*)right)->size;
}

void bin_node_print(void* node)
{
    assert(node != nullptr);
    printf("%d ", ((test_node*)node)->size);
}

int main() {
    
    int size = 0;
    int search_array[SEARCH_ELEMENT_NUM] = {0};
    test_node *new_node = nullptr;

    /*SKIP_LIST my_list(sk_node_compare);
    my_list.create();
    auto beg1 = std::chrono::steady_clock::now();
    for (int cnt =0; cnt < MAX_ELEMENT_NUM; cnt ++)
    {
        size = random()%300741;
        if (cnt < SEARCH_ELEMENT_NUM)
        {
            search_array[cnt] = size;
        }
        new_node = (test_node *)calloc(sizeof(test_node), 1);
        new_node->size = size;
        my_list.insert(new_node);
    }

    test_node search_node{};

    for (long cnt : search_array)
    {
        search_node.size = cnt;
        if (!my_list.remove(&search_node, nullptr))
        {
            printf("error !!!! %d not found for delete.......\n", search_node.size);
        }

    }

    auto end1 = std::chrono::steady_clock::now();
    std::cerr << "SkipList Insert " << MAX_ELEMENT_NUM << " Nums and Search&Delete "<<SEARCH_ELEMENT_NUM <<" Nums cost:"
              << std::chrono::duration_cast<std::chrono::milliseconds>(end1 - beg1).count()
              << std::endl;*/
    //my_list.print_list(sk_node_print);
    //my_list.destroy();


    DYN_BINARY_LIST dyn_bin_list(bin_node_compare, bin_node_print);
    dyn_bin_list.create();
    new_node = nullptr;
    auto beg2 = std::chrono::steady_clock::now();
    for (int cnt =0; cnt < MAX_ELEMENT_NUM; cnt ++)
    {
        size = ((int)random()%700741);
        if (cnt < SEARCH_ELEMENT_NUM)
        {
            search_array[cnt] = size;
        }
        new_node = (test_node *)calloc(sizeof(test_node), 1);
        new_node->size = size;
        dyn_bin_list.insert(new_node);
    }
    //dyn_bin_list.print_list_info();

    test_node search_node;
    dynamic_row_pos at_row;

    for (long cnt : search_array)
    {
        search_node.size = cnt;
        if (!dyn_bin_list.remove(&search_node))
        {
            printf("error !!!! %d not found for delete.......\n", search_node.size);
        }

    }

    auto end2 = std::chrono::steady_clock::now();
    std::cerr << "BinaryList Insert " << MAX_ELEMENT_NUM << " Nums and Search&Delete "<<SEARCH_ELEMENT_NUM <<" Nums cost:"
              << std::chrono::duration_cast<std::chrono::milliseconds>(end2 - beg2).count()
              << std::endl;

    //dyn_bin_list.print_detail_list();
    //dyn_bin_list.print_list_info();
}