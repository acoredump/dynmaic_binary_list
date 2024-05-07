//
// Created by phaeton on 2024/4/7.
//
#include "binary_list.h"

DYN_BINARY_LIST::DYN_BINARY_LIST(binary_compare bin_cmp, binary_print bin_print) {
    _bin_cmp = bin_cmp;
    _bin_print = bin_print;
}

DYN_BINARY_LIST::~DYN_BINARY_LIST() = default;

void DYN_BINARY_LIST::create() {
    _header = (dynamic_row *) calloc(sizeof(dynamic_row) + \
                                    sizeof(void *) * DYNAMIC_ROW_MIN_LENGTH, 1);
    _header->next = nullptr;
    _header->prev = nullptr;
    _header->header.used_size = 0;
    _header->header.row_size = DYNAMIC_ROW_MIN_LENGTH;

    _rows_num = 1;
    _row_index_no = 0;
    _rebuild_index_count = 0;
    memset(_row_index_pro, 0, sizeof(_row_index_pro));
}

bool DYN_BINARY_LIST::search_row_index(void *node, dynamic_row_pos *at_row) {

    bool match_flag = false;
    //第一步 利用 _row_index_pro 搜寻(使用二分的方式，在dynamic_row_index数组中进行快速查找)
    int cmp_res = 0;
    int max_pos = 0;
    dynamic_row *iter = _header;

    int beg = 0;
    int end = (int) _row_index_no;
    int index_middle_no = 0;
    dynamic_row *index = nullptr;
    if (_row_index_no > 0) {
        while (beg < end) {
            index_middle_no = (beg + end) / 2;
            index = _row_index_pro[index_middle_no].start;
            if (index->header.used_size > 0) {
                max_pos = index->header.used_size - 1;
                cmp_res = _bin_cmp(node, index->buf[max_pos]);
                if (cmp_res < 0) {
                    if (_bin_cmp(node, index->buf[0]) >= 0)
                        end = index_middle_no;
                    else
                        end = index_middle_no - 1;
                } else if (cmp_res > 0)
                    beg = index_middle_no + 1;
                else {
                    match_flag = true;
                    at_row->row = index;
                    at_row->pos = max_pos;
                    return match_flag;
                }
            }else
            {
                assert(0);
            }
        }

        if (beg > 0 && (beg == _row_index_no || beg >= end))
            beg -= 1;

        if (beg == 0)
            iter = _row_index_pro[beg].start;
        else
            iter = _row_index_pro[beg].start->next;
        index = _row_index_pro[beg].middle;

        if (index != nullptr) {
            max_pos = index->header.used_size - 1;
            cmp_res = _bin_cmp(node, index->buf[max_pos]);
            if (cmp_res > 0) {
                iter = _row_index_pro[beg].middle->next;
            } else if (cmp_res == 0) {
                match_flag = true;
                at_row->row = index;
                at_row->pos = max_pos;
                return match_flag;
            }
        }
    } else {
        for (uint32_t cnt = 0; cnt < _row_index_no; cnt++) {
            index = _row_index_pro[cnt].start;
            if (index->header.used_size > 0) {
                max_pos = index->header.used_size - 1;
                cmp_res = _bin_cmp(node, index->buf[max_pos]);
                if (cmp_res < 0) {
                    if (cnt == 0)
                        iter = _header;
                    else
                        iter = (_row_index_pro[cnt - 1].start)->next;
                    break;
                } else if (cmp_res == 0) {
                    match_flag = true;
                    at_row->row = index;
                    at_row->pos = max_pos;
                    return match_flag;
                }
            }
        }
    }
    //目标是定位到 所在的dynamic_row_index对象
    at_row->row = iter;
    at_row->pos = -1;
    return match_flag;
}

bool DYN_BINARY_LIST::search(void *node, dynamic_row_pos *at_row) {
    int max_pos,cmp_res = 0;
    dynamic_row *iter = nullptr;

    bool match_flag = false;
    dynamic_row_pos at_row_t;
    match_flag = search_row_index(node, &at_row_t);

    if (match_flag)
    {
        at_row->row = at_row_t.row;
        at_row->pos = at_row_t.pos;
    }else
    {
        //search in the dynamic_row_index to find the dynamic row
        iter = at_row_t.row;
        while (iter != nullptr) {
            max_pos = iter->header.used_size - 1;
            //直接比较最大值
            cmp_res = _bin_cmp(node, iter->buf[max_pos]);
            if (cmp_res > 0) {
                iter = iter->next;
            } else if (cmp_res == 0) {
                at_row->row = iter;
                at_row->pos = max_pos;
                return true;
            } else {
                break;
            }
        }
        int pos = 0;
        //search and find in the dynamic_row
        if (iter != nullptr && iter->header.used_size > 0) {
            //binary search the dynamic_row
            pos = binary_row_search(iter, 0, iter->header.used_size, node);
            cmp_res = _bin_cmp(node, iter->buf[pos]);
            if (cmp_res == 0) {
                at_row->row = iter;
                at_row->pos = pos;
                return true;
            }
        }
    }
    return match_flag;
}

bool DYN_BINARY_LIST::insert(void *node) {

    int max_pos,cmp_res = 0;
    dynamic_row *iter,*prev = nullptr;

    bool match_flag = false;
    dynamic_row_pos at_row_t;
    search_row_index(node, &at_row_t);

    iter = at_row_t.row;prev = iter->prev;

    while (iter != nullptr && iter->header.used_size > 0) {
        max_pos = iter->header.used_size - 1;
        //直接比较最大值
        if (_bin_cmp(node, iter->buf[max_pos]) > 0) {
            prev = iter;
            iter = iter->next;
        } else {
            break;
        }
    }

    if (iter == nullptr) {
        if (prev->header.used_size < prev->header.row_size) {
            max_pos = prev->header.used_size;
            prev->buf[max_pos] = node;
            prev->header.used_size += 1;
        } else {
            auto *new_row = (dynamic_row *) malloc(sizeof(dynamic_row) + \
                            sizeof(void *) * DYNAMIC_ROW_MIN_LENGTH);
            prev->next = new_row;
            new_row->prev = prev;
            new_row->next = nullptr;
            //将iter中的元素 转移一个最大值到最新的new_row中
            new_row->buf[0] = node;
            new_row->header.used_size = 1;
            new_row->header.row_size = DYNAMIC_ROW_MIN_LENGTH;
            _rows_num += 1;
        }
        return true;
    }

    //确定是否需要申请一个全新的 dynamic_row（挂载在row list的最后位置）
    if (iter->next != nullptr) {
        dynamic_row_search_and_insert(iter, node);
    } else {
        if (iter->header.used_size >= DYNAMIC_ROW_MIN_LENGTH) {
            //申请一个全新的 dynamic_row
            auto *new_row = (dynamic_row *) malloc(sizeof(dynamic_row) + \
                            sizeof(void *) * DYNAMIC_ROW_MIN_LENGTH);
            iter->next = new_row;
            new_row->prev = iter;
            //将iter中的元素 转移一个最大值到最新的new_row中
            new_row->buf[0] = iter->buf[max_pos];
            new_row->header.used_size = 1;
            new_row->header.row_size = DYNAMIC_ROW_MIN_LENGTH;
            new_row->next = nullptr;
            iter->header.used_size -= 1;
            _rows_num += 1;
        }
        dynamic_row_search_and_insert(iter, node);
    }
    return true;
}

bool DYN_BINARY_LIST::remove(void *node) {
    dynamic_row_pos at_row;
    if (search(node, &at_row)) {
        return binary_row_delete(at_row.row, at_row.pos);
    }
    return false;
}

int DYN_BINARY_LIST::binary_row_search(dynamic_row *row, int low, int high, void *target) {
    int cmp_res;
    int middle = 0;
    void **buf = row->buf;
    while (low <= high) {
        middle = (low + high) / 2;
        cmp_res = _bin_cmp(target, buf[middle]);
        if (cmp_res == 0) {
            return middle;
        } else if (cmp_res > 0) {
            low = middle + 1;
        } else {
            high = middle - 1;
        }
    }
    return middle;
}

bool DYN_BINARY_LIST::binary_row_insert(dynamic_row *row, int position, int length, void *target) {

    if (row->header.used_size >= row->header.row_size) {

        if (row->header.row_size >= DYNAMIC_ROW_MAX_LENGTH) {
            int unit_row_size = DYNAMIC_ROW_MAX_LENGTH / 2;
            int part1_row_buf_size = row->header.row_size - unit_row_size;
            auto *part1_row = (dynamic_row *) malloc(sizeof(dynamic_row) + \
                                sizeof(void *) * (part1_row_buf_size + DYNAMIC_ROW_UPGRADE_UINT));
            part1_row->next = row->next;
            part1_row->prev = row;
            part1_row->next->prev = part1_row;
            row->next = part1_row;
            part1_row->header.row_size = part1_row_buf_size + DYNAMIC_ROW_UPGRADE_UINT;
            part1_row->header.used_size = part1_row_buf_size;
            memcpy(part1_row->buf, row->buf + unit_row_size, part1_row_buf_size * sizeof(void *));

            //复制老的row对象
            auto *part2_row = (dynamic_row *) malloc(sizeof(dynamic_row) + \
                                sizeof(void *) * (unit_row_size + DYNAMIC_ROW_UPGRADE_UINT));
            memcpy(part2_row, row, sizeof(dynamic_row) + sizeof(void *) * (unit_row_size));
            part2_row->header.row_size = unit_row_size + DYNAMIC_ROW_UPGRADE_UINT;
            part2_row->header.used_size = unit_row_size;

            //维护链接指向
            part1_row->prev = part2_row;
            if (row != _header)
                row->prev->next = part2_row;
            else
                _header = part2_row;

            _rebuild_index_count += 1;
            if (_rebuild_index_count > (DYNAMIC_ROW_INDEX_UNIT - 2)) {
                build_row_index_points(nullptr, nullptr);
                _rebuild_index_count = 0;
            } else {
                build_row_index_points(row, part2_row);
            }

            free(row);
            if (position >= unit_row_size) {
                row = part1_row;
                position = position - unit_row_size;
                length = part1_row->header.used_size;
            } else {
                row = part2_row;
                length = part2_row->header.used_size;
            }
            _rows_num += 1;

        } else {
            //当前的dynamic_row需要进行扩容, 否则会引起后续节点的级联调整
            int new_row_buf_size = row->header.row_size + DYNAMIC_ROW_UPGRADE_UINT;
            auto *new_row = (dynamic_row *) malloc(sizeof(dynamic_row) + sizeof(void *) * new_row_buf_size);
            if (new_row == nullptr)
                return false;
            memcpy(new_row, row, sizeof(dynamic_row) + sizeof(void *) * row->header.row_size);
            //维护链接指向
            new_row->header.row_size = new_row_buf_size;
            row->next->prev = new_row;
            if (row != _header) {
                row->prev->next = new_row;
            } else {
                _header = new_row;
            }

            build_row_index_points(row, new_row);
            free(row);
            row = new_row;
        }
    }

    void **buf = row->buf;
    if (_bin_cmp(buf[position], target) >= 0) {
        position = position - 1;
        while (position >= 0) {
            if (_bin_cmp(buf[position], target) < 0) {
                memcpy(_tmp, buf + position + 1, (length - position - 1) * sizeof(void *));
                buf[position + 1] = target;
                memcpy(buf + position + 2, _tmp, (length - position - 1) * sizeof(void *));
                row->header.used_size++;
                return true;
            }
            position = position - 1;
        }
        memcpy(_tmp, buf, length * sizeof(void *));
        buf[0] = target;
        memcpy(buf + 1, _tmp, (length) * sizeof(void *));
    } else {
        position = position + 1;
        while (position < length) {
            if (_bin_cmp(buf[position], target) > 0) {
                memcpy(_tmp, buf + position, (length - position) * sizeof(void *));
                buf[position] = target;
                memcpy(buf + position + 1, _tmp, (length - position) * sizeof(void *));
                row->header.used_size++;
                return true;
            }
            position = position + 1;
        }
        buf[length] = target;
    }

    row->header.used_size++;
    return true;
}

bool DYN_BINARY_LIST::binary_row_delete(dynamic_row *row, int position) {
    int length = row->header.used_size;
    void **buf = row->buf;
    if (position >= length)
        return false;

    free(buf[position]);
    if (position < (length - 1))
    {
        memcpy(_tmp, buf + position + 1, (length - position - 1)*sizeof(void *));
        memcpy(buf + position, _tmp, (length - position - 1)*sizeof(void *));
    }
    row->header.used_size -= 1;
    buf[row->header.used_size] = nullptr;
    if (row->header.used_size <= 0)
    {
        if (row == _header)
        {
            _header = _header->next;
            _header->prev = nullptr;
        }else
        {
            //当前的整个dynamic_row中的元素 全部被删除
            if (row->prev != nullptr)
                (row->prev)->next = row->next;
            if (row->next != nullptr)
                (row->next)->prev = row->prev;
        }
        _rows_num --;
        free(row);
        build_row_index_points(nullptr, nullptr);
        return true;
    }

    if (row->header.row_size - row->header.used_size >= (DYNAMIC_ROW_UPGRADE_UINT + 13)) {
        int new_row_buf_size = row->header.row_size - DYNAMIC_ROW_UPGRADE_UINT;
        auto *new_row = (dynamic_row *) malloc(sizeof(dynamic_row) + sizeof(void *) * new_row_buf_size);
        if (new_row == nullptr)
            return false;
        memcpy(new_row, row, sizeof(dynamic_row) + sizeof(void *) * new_row_buf_size);
        new_row->header.row_size = new_row_buf_size;

        if (row->prev != nullptr)
            (row->prev)->next = new_row;
        else
        {
            //_header头指针指向的realloc 发生空间收缩
            _header = new_row;
        }

        if (row->next != nullptr)
            (row->next)->prev = new_row;
        build_row_index_points(row, new_row);
        free(row);
    }
    return true;
}

bool DYN_BINARY_LIST::dynamic_row_search_and_insert(dynamic_row *row, void *target) {
    bool success_flag = false;
    if ((row->header).used_size == 0) {
        row->buf[0] = target;
        row->header.used_size += 1;
    } else {
        int position = binary_row_search(row, 0, (row->header).used_size, target);
        success_flag = binary_row_insert(row, position, (row->header).used_size, target);
    }
    return success_flag;
}

void DYN_BINARY_LIST::print_detail_list() {
    int row_no = 0;
    dynamic_row *iter = _header;
    while (iter != nullptr) {
        row_no += 1;
        printf("binary dynamic row number %d; row size is %d; row used is %d \n", row_no, iter->header.row_size,
               iter->header.used_size);
        printf("************************************************************ \n");
        void **node_buf = iter->buf;
        for (int cnt = 0; cnt < iter->header.used_size; cnt++) {
            _bin_print(node_buf[cnt]);
            if (cnt % 100 == 0 && cnt != 0)
                printf("\n");
        }
        iter = iter->next;
        printf(" \n");
    }
    printf(" \n");
}

void DYN_BINARY_LIST::print_list_info() {
    int row_no = 0;
    uint32_t total_cap_nums = 0;
    uint32_t total_ele_nums = 0;
    dynamic_row *iter = _header;

    for (int cnt = 0; cnt < _rows_num; cnt++) {
        row_no += 1;
        //printf("binary dynamic row number %d; row size is %d; row used is %d \n", row_no, iter->header.row_size,
               //iter->header.used_size);
        total_cap_nums += iter->header.row_size;
        total_ele_nums += iter->header.used_size;
        iter = iter->next;
    }

    printf("************************************************\n");
    printf("dynamic_rows index num is %d\n", _row_index_no);
    printf("total dynamic_rows num is %d\n", _rows_num);
    printf("total dynamic_rows cached elements num is %d\n", total_ele_nums);
    double result = static_cast<double>(total_ele_nums) / total_cap_nums;
    result = result * 100;
    printf("dynamic rows memory usage is %%%f\n", result);
    printf("************************************************\n");
}

void DYN_BINARY_LIST::build_row_index_points(dynamic_row *row, dynamic_row *new_row) {
    //第一个 DYNAMIC_ROW_INDEX_UNIT 单元不构建索引
    if (_rows_num > DYNAMIC_ROW_INDEX_UNIT) {

        if (row != nullptr && new_row != nullptr) {
            for (int cnt = 0; cnt < _row_index_no; cnt++) {
                if (_row_index_pro[cnt].start == row) {
                    //跟换索引row为最新的索引row
                    _row_index_pro[cnt].start = new_row;
                    return;
                } else if (_row_index_pro[cnt].middle == row) {
                    _row_index_pro[cnt].middle = new_row;
                    return;
                }
            }
            return;
        }

        _row_index_no = 1;
        uint32_t row_ele_count = 0;
        uint32_t row_index_middle = 0;
        uint32_t row_index_middle_pos = 0;
        uint32_t row_index_middle_unit = DYNAMIC_ROW_INDEX_UNIT / 2;
        dynamic_row *iter = _header;
        if (_rows_num >= 2 * DYNAMIC_ROW_INDEX_UNIT) {
            for (int cnt = 0; cnt < _rows_num - row_index_middle_unit; cnt++) {
                row_ele_count++;
                if (_row_index_no >= DYNAMIC_MAX_ROW_INDEX_NUM)
                    break;
                if ((row_ele_count >= DYNAMIC_ROW_INDEX_UNIT) && cnt != 0) {
                    _row_index_pro[_row_index_no].start = iter;
                    row_index_middle_pos = cnt + row_index_middle_unit;
                    row_index_middle = _row_index_no;
                    _row_index_no++;
                    row_ele_count = 0;
                } else if (cnt == row_index_middle_pos && cnt != 0) {
                    _row_index_pro[row_index_middle].middle = iter;
                }
                iter = iter->next;
            }
        }

        //构建dynamic_row_index索引数组 0号位置的索引对象
        iter = _header;
        _row_index_pro[0].start = iter;
        for (int cnt = 0; cnt < row_index_middle_unit; cnt++) {
            iter = iter->next;
        }
        _row_index_pro[0].middle = iter;
    }
}

void DYN_BINARY_LIST::check_list_continuity() {
    int row_no = 0;
    int total_ele_nums = 0;
    dynamic_row *iter = _header;
    for (int cnt = 0; cnt < _rows_num; cnt++) {
        assert(iter != nullptr);
        iter = iter->next;
    }
}






