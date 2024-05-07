//
// Created by phaeton on 2024/3/27.
//
#include "skip_list.h"

SKIP_LIST::~SKIP_LIST() {

}

SKIP_LIST::SKIP_LIST(sk_compare sk_cmp) {
    _sk_cmp = sk_cmp;
}

bool SKIP_LIST::create() {
    _level = 1;
    _length = 0;
    _header = alloc_node(SKIPLIST_MAXLEVEL, nullptr);
    if (_header == nullptr)
        return false;
    for (int cnt = 0; cnt < SKIPLIST_MAXLEVEL; cnt++)
    {
        _header->level[cnt].forward = nullptr;
        _header->level[cnt].span = 0;
    }

    _header->backward = nullptr;
    _tail = nullptr;
    return true;
}

void SKIP_LIST::free_node(sk_node *Node) {
    free(Node->ele);
    free(Node);
}

void SKIP_LIST::destroy() {
    sk_node *node = _header->level[0].forward, *next;
    free(_header);
    while(node) {
        next = node->level[0].forward;
        free_node(node);
        node = next;
    }
    _level = 1;
    _length = 0;
}

bool SKIP_LIST::insert(void *Node, double score) {
    int i, level;
    uint32_t rank[SKIPLIST_MAXLEVEL];
    sk_node *update[SKIPLIST_MAXLEVEL], *x;

    //从header开始寻找insert pos
    x = _header;
    for (i = _level-1; i >= 0; i--) {
        //skiplist的高层开始，一层一层的往下查找 每经过一层 查找范围都会收缩一部分
        //搜集每一层的合适位置信息，存放于update[SKIPLIST_MAXLEVEL]中，实际也就记录着当前结点需要插入在每一层的什么位置
        //最低一层即0层，决定了最终的插入位置，除了0层外其他层级主要是搜集对应的层级位置信息
        /* store rank that is crossed to reach the insert position */
        rank[i] = i == (_level-1) ? 0 : rank[i+1];
        /* 按照score 从小到大排列*/
        while (x->level[i].forward &&
               (x->level[i].forward->score < score ||
                (x->level[i].forward->score == score &&
                 _sk_cmp((sk_node*)x->level[i].forward, Node) < 0)))
        {
            //获取每一层的插入位置x
            rank[i] += x->level[i].span;
            x = x->level[i].forward;
        }
        //记录当前层级i适合插入的位置(在当前层级i上，位置是在x之后)
        update[i] = x;
    }
    /* we assume the element is not already inside, since we allow duplicated
     * scores, reinserting the same element should never happen since the
     * caller of zslInsert() should test in the hash table if the element is
     * already inside or not. */
    level = RandomLevel();  //获取当前insert节点的层级
    if (level > _level) {
        for (i = _level; i < level; i++) {
            rank[i] = 0;
            //层级大于当前skiplist建立的最高层级_level时，设置为头节点层级；
            //无法与现有的层级指针适配，所以回指_header指针，设置span为当前skiplist的_length
            update[i] = _header;
            update[i]->level[i].span = _length;
        }
        //层级大于当前skiplist的最高层级时，需要提升skiplist的层级
        _level = level;
    }
    //创建层级是level的zskiplistNode， 并插入到update中记录并重新梳理每一层的合适位置
    x = alloc_node(level,Node ,score);
    for (i = 0; i < level; i++) {
        x->level[i].forward = update[i]->level[i].forward;
        update[i]->level[i].forward = x;

        /* update span covered by update[i] as x is inserted here */
        /* 更新插入节点当前的跨度信息span*/
        x->level[i].span = update[i]->level[i].span - (rank[0] - rank[i]);
        //rank记录0号层级的位置下标编号（最终插入在0号level的位置pos编号）
        update[i]->level[i].span = (rank[0] - rank[i]) + 1;
    }

    /* increment span for untouched levels */
    for (i = level; i < _level; i++) {
        update[i]->level[i].span++;
    }

    //backward后向指针 属于0号层级的指针
    x->backward = (update[0] == _header) ? nullptr : update[0];
    if (x->level[0].forward)
        x->level[0].forward->backward = x;
    else
        _tail = x;
    _length++;
    return true;
}

//解除节点链接，从skiplist上摘除一个sk_node节点（不释放节点空间）；
void SKIP_LIST::unlink_node(sk_node *x, sk_node **update)
{
/*update是当前需要删除的节点x，在skiplist中从高到低的每一层中的位置信息*/
    int i;
    for (i = 0; i < _level; i++) {
        if (update[i]->level[i].forward == x) {
            update[i]->level[i].span += x->level[i].span - 1;
            update[i]->level[i].forward = x->level[i].forward;
        } else {
            update[i]->level[i].span -= 1;
        }
    }
    if (x->level[0].forward) {
        x->level[0].forward->backward = x->backward;
    } else {
        _tail = x->backward;
    }
    while(_level > 1 && _header->level[_level-1].forward == nullptr)
        _level--;
    _length--;
}

/*
 * if the param (sk_node  **node) is null
 * the SKIP_LIST::remove will release the memory that match the Node
 * if the (sk_node  **node) is not null, it will store the sk_nodes that match the Node
 * it is up to you for release the memory or not according the (sk_node  **node)
 * */
bool SKIP_LIST::remove(void *Node, sk_node  **node, double score) {
    sk_node *update[SKIPLIST_MAXLEVEL], *x;
    int i;

    x = _header;
    for (i = _level-1; i >= 0; i--) {
        while (x->level[i].forward &&
               (x->level[i].forward->score < score ||
                (x->level[i].forward->score == score &&
                _sk_cmp((sk_node*)x->level[i].forward, Node) < 0)))
        {
            x = x->level[i].forward;
        }
        //记录待删除的节点在每一层中的位置信息
        update[i] = x;
    }
    /* We may have multiple elements with the same score, what we need
     * is to find the element with both the right score and object. */
    x = x->level[0].forward;
    if (x && score == x->score && _sk_cmp(x, Node) == 0) {
        unlink_node(x, update);
        if (!node)
            free_node(x);
        else
            *node = x;
        return true;
    }
    return false;
}

int SKIP_LIST::RandomLevel() {
    int level = 1;
    /*当前节点属于n层节点的概率是（n>1）: 1/4 * 1/4 * 1/4 .......即n-1个1/4相乘的概率
    *
    *----------|------------------------------
    *
    * (random()&0xFFFF) < (SKIPLIST_P * 0xFFFF)的条件每次只有4分之1的概率会满足，满足
    * 后会导致 level +=1 被执行，对应就往高层爬升，但是每次都有4分之3的概率不满足条件
    * 导致level自加结束，此时的level的值相对ZSKIPLIST_MAXLEVEL应该相对比较小
    */
    while ((random()&0xFFFF) < (SKIPLIST_P * 0xFFFF))
        level += 1;
    return (level<SKIPLIST_MAXLEVEL) ? level : SKIPLIST_MAXLEVEL;
}

sk_node * SKIP_LIST::alloc_node(uint8_t level ,void *ele, double socre) {
    auto * node = (sk_node*)malloc(sizeof(sk_node) + level*sizeof(sk_level));
    node->ele = ele;
    node->score = socre;
    return node;
}

void SKIP_LIST::print_list(sk_print print_node) {
    sk_node *node = (_header->level[0]).forward;
    for (int cnt = 0; cnt < _length; cnt++) {
        print_node(node);
        if (cnt % 100 == 0 && cnt != 0)
            printf("\n");
        node = node->level[0].forward;
    }
    printf(" \n");
}