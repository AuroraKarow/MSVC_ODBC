BAGRT_BEGIN

uint64_t num_cnt(uint64_t from, uint64_t to)
{
    if(from <= to) return to - from + 1;
    else return 0;
}

template<typename CT, typename PT> const std::shared_ptr<CT> derive_instance_ptr(std::shared_ptr<PT> &pBaseInstance) {return std::dynamic_pointer_cast<CT>(pBaseInstance);}

template<typename _T> void quick_sort(std::unique_ptr<_T[]> &seq_val, uint64_t begin, uint64_t end, bool asc = true, std::function<bool(_T&, _T&)>func_comp = [](_T &_first, _T &_second){return _first > _second;})
{
    if(end == begin) return;
    else
    {
        auto pivot = begin, slide = end;
        while(slide != pivot)
            if(pivot<slide)
            {
                if(!func_comp(seq_val[slide], seq_val[pivot]))
                {
                    std::swap(seq_val[slide], seq_val[pivot]);
                    std::swap(slide, pivot);
                    slide ++;
                }
                else -- slide;
            }
            else
            {
                if(func_comp(seq_val[slide], seq_val[pivot]))
                {
                    std::swap(seq_val[slide], seq_val[pivot]);
                    std::swap(slide, pivot);
                    -- slide;
                }
                else ++ slide;
            }
        if(pivot != begin) quick_sort(seq_val, begin, pivot-1, asc, func_comp);
        if(pivot != end) quick_sort(seq_val, pivot+1, end, asc, func_comp);
    }
}

template<typename _Ty> class net_queue
{
protected:
    std::unique_ptr<_Ty[]> _ptr;
    uint64_t len = 0;
    void realloc_inc_ptr(uint64_t _size)
    {
        if(_size > len)
        {
            auto p_tool = std::make_unique<_Ty[]>(_size);
            for(auto i=0; i<len; ++i) p_tool[i] = std::move(_ptr[i]);
            len = _size;
            _ptr = std::move(p_tool);
        }
    }
    template <typename ... Args> net_queue<_Ty> realloc_dec_ptr(Args &&...idx)
    {
        uint64_t idx_arr[] = {idx ...};
        auto cut_len = sizeof(idx_arr)/sizeof(uint64_t);
        for(auto i=0; i<cut_len; ++i) if(idx_arr[i] < len) continue;
        else return net_queue<_Ty>::blank_queue();
        auto uniq_idx = std::make_unique<uint64_t[]>(cut_len);
        for(auto i=0; i<cut_len; ++i) uniq_idx[i] = idx_arr[i];
        net_queue<_Ty> out_temp(cut_len);
        quick_sort(uniq_idx, 0, cut_len - 1);
        auto p_tool = std::make_unique<_Ty[]>(len - cut_len);
        for(auto i=0,re_loc=0,idx_cnt=0; i<len; ++i)
            if(i==uniq_idx[idx_cnt])
            {
                out_temp[idx_cnt++] = _ptr[i];
                continue;
            }
            else p_tool[re_loc++] = _ptr[i];
        _ptr = std::move(p_tool);
        len -= cut_len;
        return out_temp;
    }
    _Ty temp;
public:
    net_queue(uint64_t _size = 0) {realloc_inc_ptr(_size);}
    net_queue(net_queue &cpy_val)
    {
        if(cpy_val.len)
        {
            if(len != cpy_val.len)
            {
                len = cpy_val.len;
                _ptr = std::make_unique<_Ty []>(len);
            }
            for(auto i=0; i<len; ++i) _ptr[i] = cpy_val._ptr[i];
        }
    }
    net_queue(net_queue &&mov_val)
    {
        _ptr = std::move(mov_val._ptr);
        len = mov_val.len;
        mov_val.len = 0;
    }
    bool init(uint64_t _size = 1)
    {
        if(_size > 0)
        {
            _ptr = std::make_unique<_Ty[]>(_size);
            len = _size;
            return true;
        }
        else return false;
    }
    static net_queue blank_queue() {return net_queue();}
    uint64_t size() {return len;}
    template<typename...Args> bool insert(uint64_t idx, Args &&...args)
    {
        if(idx > len) return false;
        else
        {
            realloc_inc_ptr(len + 1);
            for(auto i=len-1; i>idx; ++i) _ptr[i] = _ptr[i-1];
            _ptr[idx] = std::move(_Ty(std::move(std::forward<Args>(args)...)));
            return true;
        }
    }
    template<typename...Args> bool emplace_back(Args &&...args) {return insert(len, args...);}
    bool push_back(_Ty val) {return insert(len, val);}
    template<typename... Args> net_queue<_Ty> erase(Args &&...idx) {return realloc_dec_ptr(idx...);}
    net_queue sub_queue(uint64_t idx_first, uint64_t idx_second)
    {
        net_queue sub_out;
        if(idx_first<len && idx_second<len)
        {
            if(idx_second < idx_first) std::swap(idx_first, idx_second);
            auto mem_size = num_cnt(idx_first, idx_second);
            if(mem_size == len) sub_out = *this;
            else
            {
                sub_out.init(mem_size);
                for(auto i=idx_first; i<=idx_second; ++i) sub_out._ptr[i] = _ptr[i];
            }
        }
        return sub_out;
    }
    void sort(bool asc = true, std::function<bool(_Ty&, _Ty&)> _func = [](_Ty &_first, _Ty &_second){return _first > _second;}) {quick_sort(_ptr, 0, len - 1, asc, _func);}
    net_queue unit(net_queue &val)
    {
        net_queue u_set(val.len + len);
        for(auto i=0; i<u_set.len; ++i)
        {
            if(i<len) u_set._ptr[i] = _ptr[i];
            else u_set._ptr[i] = val._ptr[i-len];
        }
        return u_set;
    }
    net_queue unit_union(net_queue &val)
    {
        auto src_val = val;
        for(auto i=0; i<len; ++i) for(auto j=0; j<src_val.len; ++j)
            if(_ptr[i] == src_val._ptr[j])
            {
                src_val.erase(j);
                break;
            }
        return unit(src_val);
    }
    net_queue unit_intersect(net_queue &val)
    {
        net_queue itrs;
        auto src_val = val;
        for(auto i=0; i<len; ++i) for(auto j=0; j<src_val.len; ++j)
            if(_ptr[i] == src_val._ptr[j])
            {
                itrs.push_back(src_val._ptr[j]);
                src_val.erase(j);
                break;
            }
        return itrs;
    }
    net_queue<uint64_t> find(_Ty &&target, uint64_t range_first = 0, uint64_t range_second = 0)
    {
        net_queue<uint64_t> idx_set;
        if(range_first<len && range_second<len && range_second!=range_first)
        {
            if(range_first < range_second) std::swap(range_second, range_first);
            for(auto i=range_first; i<=range_second; ++i)
                if(_ptr[i] == target) idx_set.push_back(i);
        }
        return idx_set;
    }
    net_queue<uint64_t> find(_Ty &target, uint64_t range_first = 0, uint64_t range_second = 0) {return find(std::move(target), range_first, range_second);}
    _Ty sum(std::function<_Ty(_Ty&, _Ty&)> add_func = [](_Ty &first, _Ty &second){return first + second;})
    {
        auto rtn_val = _ptr[IDX_ZERO];
        for(auto i=1; i<len; ++i) rtn_val = std::move(add_func(rtn_val, _ptr[i]));
        return rtn_val;
    }
    _Ty &operator[](uint64_t idx)
    {
        if(idx < len) return _ptr[idx];
        else return temp;
    }
    bool operator==(net_queue &val)
    {
        if(len == val.len)
        {
            for(auto i=0; i<len; ++i) if(val._ptr[i] == _ptr[i]) continue;
            else return false;
            return true;
        }
        else return false;
    }
    bool operator!=(net_queue &val) {return !(val == *this);}
    void operator=(net_queue &val) {new(this)net_queue(val);}
    void operator=(net_queue &&val) {new(this)net_queue(std::move(val));}
    friend std::ostream& operator<<(std::ostream &output, net_queue &val)
    {
        for(auto i=0; i<val.len; ++i)
        {
            output << '[' << i << "][" << std::endl << val._ptr[i] << std::endl << ']';
            if(i+1 < val.len) output << std::endl;
        }
        return output;
    }
    void reset()
    {
        len = 0;
        _ptr.reset();
        _ptr.release();
        _ptr = nullptr;
    }
    // ~net_queue() {len = 0;}
};

template<typename T> class net_seq
{
protected:
    struct node {node *prev = nullptr; std::unique_ptr<node> next = nullptr /* node *next */; T elem;};
    std::unique_ptr<node> head;
    node *tail = nullptr, *itr = nullptr;
    uint64_t len = 0, itr_idx = 0;
    T temp;
    std::unique_ptr<node> create_node() { return std::make_unique<node>(); }
    node *idx_node(uint64_t idx)
    {
        if(idx)
            if(idx+1 == len) itr = tail;
            else
            {
                auto tml_cnt = len - 1 - idx,
                    fnt_cnt = idx;
                uint64_t itr_mov = std::abs((int)itr_idx - (int)idx);
                if(itr && itr_mov<tml_cnt && itr_mov<fnt_cnt) while(itr_mov)
                {
                    if(itr_idx < idx) itr = (itr->next).get();
                    else itr = itr -> prev;
                    -- itr_mov;
                }
                else if(fnt_cnt < itr_mov)
                {
                    itr = head.get();
                    while(fnt_cnt)
                    {
                        itr = (itr->next).get();
                        -- fnt_cnt;
                    }
                }
                else
                {
                    itr = tail;
                    while(tml_cnt)
                    {
                        itr = itr -> prev;
                        -- tml_cnt;
                    }
                }
            }
        else itr = head.get();
        itr_idx = idx;
        return itr;
    }
public:
    uint64_t size() { return len; }
    net_seq() {}
    net_seq(net_seq &src) : len(src.len)
    {
        if(len)
        {
            head = create_node();
            auto src_ptr = src.head.get(),
                temp_ptr = head.get();
            temp_ptr -> elem = src_ptr -> elem;
            tail = temp_ptr;
            for(auto i=1; i<len; ++i)
            {
                (temp_ptr -> next) = create_node();
                (temp_ptr -> next).get() -> prev = temp_ptr;
                (temp_ptr -> next).get() -> elem = src_ptr -> next -> elem;
                temp_ptr = (temp_ptr -> next).get();
                tail = temp_ptr;
                src_ptr = (src_ptr -> next).get();
            }
        }
    }
    net_seq(net_seq &&src) : head(std::move(src.head)), len(src.len), tail(src.tail)
    {
        src.tail = nullptr;
        src.len = 0;
    }
    template<typename...args>bool insert(uint64_t idx, args &&...src)
    {
        if(idx > len) return false;
        else 
        {
            auto temp_ptr = create_node();
            auto temp_node = temp_ptr.get();
            temp_node -> elem = std::move(T(std::forward<args>(src)...));
            if(idx)
                if(idx == len)
                {
                    if(tail)
                    {
                        tail -> next = std::move(temp_ptr);
                        temp_node -> prev = tail; 
                        tail = temp_node;
                    }
                    else
                    {
                        head = std::move(temp_ptr);
                        tail = head.get();
                    }
                    itr = tail;
                }
                else
                {
                    auto tgt_node = idx_node(idx);
                    temp_node -> prev = tgt_node -> prev;
                    tgt_node -> prev = temp_node;
                    temp_node -> next = std::move(temp_node -> prev -> next);
                    temp_node -> prev -> next = std::move(temp_ptr);
                    itr = temp_node;
                }
            else
            {
                temp_node -> next = std::move(head);
                head = std::move(temp_ptr);
                if(temp_node -> next) (temp_node->next).get() -> prev = temp_node;
                else tail = temp_node;
                itr = head.get();
            }
            ++ len;
            itr_idx = idx;
            return true;
        }
    }
    T &erase(uint64_t idx)
    {
        if(idx < len)
        {
            auto tgt_node = idx_node(idx);
            temp = std::move(tgt_node -> elem);
            if(tgt_node -> prev)
            {
                tgt_node = tgt_node -> prev;
                tgt_node -> next = std::move((tgt_node->next).get()->next);
                if(tgt_node->next) (tgt_node->next).get()->prev = tgt_node;
                else tail = tgt_node;
                -- itr_idx;
                
            }
            else if(tgt_node -> next)
            {
                head = std::move(head.get()->next);
                tgt_node = head.get();
                tgt_node -> prev = nullptr;
            }
            else
            {
                tgt_node = nullptr;
                head.reset();
            }
            itr = tgt_node;
            -- len;
        }
        return temp;
    }
    net_seq unit(net_seq &src)
    {
        net_seq tool_cpy = *this;
        if(src.len)
        {
            net_seq src_cpy = src;
            tool_cpy.len += src_cpy.len;
            src_cpy.head.get() -> prev = tool_cpy.tail;
            tool_cpy.tail -> next = std::move(src_cpy.head);
            tool_cpy.tail = src_cpy.tail;
        }
        return tool_cpy;
    }
    net_seq colabo_unit(net_seq &src)
    {
        net_seq src_cpy = src, tool_cpy = *this;
        for(auto i=0; i<len; ++i) for(auto j=0; j<src_cpy.len; ++j) if(src_cpy[j] == tool_cpy[i])
        {
            src_cpy.erase(j);
            break;
        }
        return tool_cpy.unit(src_cpy);
    }
    net_seq inters_unit(net_seq &src)
    {
        net_seq src_cpy = src, ls_temp;
        for(auto i=0; i<len; ++i) for(auto j=0; j<src_cpy.len; ++j) if(src_cpy[j] == (*this)[i])
        {
            ls_temp.insert(ls_temp.len, src_cpy.erase(j));
            break;
        }
        return ls_temp;
    }
    T &operator[](uint64_t idx)
    {
        if(idx < len) return idx_node(idx) -> elem;
        else return temp;
    }
    void operator=(net_seq &src) {new (this)net_seq(src);}
    void operator=(net_seq &&src) {new (this)net_seq(std::move(src));}
    bool operator==(net_seq &src)
    {
        if(len == src.len)
        {
            auto tool_ptr = head.get(),
                src_ptr = src.head.get();
            for(auto i=0; i<len; ++i)
                if(tool_ptr->elem != src_ptr->elem) return false;
                else
                {
                    tool_ptr = (tool_ptr -> next).get();
                    src_ptr = (src_ptr -> next).get();
                }
            return true;
        }
        else return false;
    }
    bool operator!=(net_seq &src) {return !(*this == src);}
    friend std::ostream &operator<<(std::ostream &output, net_seq &src)
    {
        auto tool_ptr = src.head.get();
        for(auto i=0; i<src.len; ++i)
        {
            output << '[' << i << "][" << std::endl << tool_ptr -> elem << std::endl << ']';
            if(i+1 != src.len) output << std::endl;
            tool_ptr = (tool_ptr -> next).get();
        }
        return output;
    }
    // ~net_seq() {}
};

template<typename _Ty> class net_list
{
public:
    struct _node
    {
        _Ty data;
        _node *prev_node = nullptr;
        std::unique_ptr<_node> next_node = nullptr;
        _node *next() {return next_node.get();}
        _node *prev() {return prev_node;}
    };
protected:
    std::unique_ptr<_node> head = nullptr;
    _node *tail = nullptr;
    uint64_t len = 0;
    _Ty temp;
    std::unique_ptr<_node> create_node() {return std::make_unique<_node>();}
    _node *idx_node = head.get();
    uint64_t pre_idx = 0;
public:
    net_list() {}
    void reset()
    {
        head.reset();
        head = nullptr;
        len = 0;
    }
    uint64_t size() {return len;}
    bool empty() {return !len;}
    net_list(net_list &cpy_val)
    {
        if(cpy_val.len)
        {
            if(!len)
            {
                head = create_node();
                ++ len;
            }
            auto p_tool = head.get(),
                src_tool = cpy_val.head.get();
            p_tool -> data = src_tool -> data;
            auto cpy_len = 0;
            if(len < cpy_val.len) cpy_len = len;
            else cpy_len = cpy_val.len;
            len = cpy_val.len;
            for(auto i=1; i<cpy_len; ++i)
            {
                p_tool = p_tool -> next_node.get();
                src_tool = src_tool -> next_node.get();
                p_tool -> data = src_tool -> data;
            }
            if(cpy_val.len == cpy_len) p_tool -> next_node = nullptr;
            else for(auto i=cpy_len; i<cpy_val.len; ++i)
            {
                p_tool -> next_node = create_node();
                p_tool -> next_node.get() -> prev_node = p_tool;
                p_tool = p_tool -> next_node.get();
                src_tool = src_tool->next_node.get();
                p_tool->data = src_tool->data;
            }
        }
    }
    net_list(net_list &&cpy_val)
    {
        len = cpy_val.len;
        head = std::move(cpy_val.head);
        cpy_val.len = 0;
    }
    template<typename ... Args> bool insert(uint64_t idx, Args &&...args)
    {
        if(idx > len) return false;
        else
        {
            auto _temp = create_node();
            _temp.get() -> data = std::move(_Ty(std::move(std::forward<Args>(args)...)));
            if(idx)
            {
                auto p_tool = head.get();
                for(auto i=1; i<idx; ++i) p_tool = (p_tool -> next_node).get();
                _temp.get() -> next_node = std::move(p_tool -> next_node);
                if(_temp.get() -> next_node) (_temp.get() -> next_node).get() -> prev_node = _temp.get();
                else tail = _temp.get();
                p_tool -> next_node = std::move(_temp);
                (p_tool -> next_node).get() -> prev_node = p_tool;
            }
            else
            {
                _temp.get() -> next_node = std::move(head);
                head = std::move(_temp);
                if(len) (head.get() -> next_node).get() -> prev_node = head.get();
            }
            if(idx < pre_idx) ++ pre_idx;
            ++ len;
            return true;
        }
    }
    _node *head_node()
    {
        auto p_tool = head.get();
        return p_tool;
    }
    _node *tail_node() {return tail;}
    _Ty &erase(uint64_t idx)
    {
        if(idx < len)
        {
            if(idx)
            {
                auto p_tool = head.get();
                for(auto i=1; i<idx; ++i) p_tool = (p_tool -> next_node).get();
                temp = std::move(p_tool -> next_node -> data);
                p_tool -> next_node = std::move(p_tool->next_node->next_node);
                if(p_tool -> next_node) (p_tool -> next_node).get() -> prev_node = p_tool;
                else tail = p_tool;
            }
            else
            {
                temp = std::move(head.get() -> data);
                head = std::move(head.get() -> next_node);
                if(head) head.get() -> prev_node = nullptr;
                else tail = head.get();
            }
            if(idx < pre_idx) -- pre_idx;
            -- len;
        }
        return temp;
    }
    template<typename ... Args> bool emplace_back(Args &&...args) {return insert(len, args ...);}
    bool push_back(_Ty val) {return insert(len, val);}
    friend std::ostream &operator<<(std::ostream &output, net_list &out_val)
    {
        auto p_tool = out_val.head.get();
        for(auto i=0; i<out_val.len; ++i)
        {
            output << '[' << i << "][" << std::endl;
            output << p_tool -> data << std::endl << ']' << std::endl;
            if(p_tool -> next_node) p_tool = (p_tool -> next_node).get();
        }
        return output;
    }
    void operator=(net_list &&val) {new (this)net_list(std::move(val));}
    _Ty &operator[](uint64_t idx)
    {
        if(idx < len)
        {
            _node *p_tool;
            int mov_cnt = idx - pre_idx;
            mov_cnt = std::abs(mov_cnt);
            if(pre_idx && mov_cnt<idx && pre_idx<len)
            {
                p_tool = idx_node;
                for(auto i=0; i<mov_cnt; ++i)
                    if(idx > pre_idx) p_tool = (p_tool -> next_node).get();
                    else p_tool = p_tool -> prev_node;
            }
            else
            {
                p_tool = head.get();
                for(auto i=0; i<idx; ++i) p_tool = (p_tool -> next_node).get();
            }
            idx_node = p_tool;
            pre_idx = idx;
            return p_tool -> data;
        }
        else return temp;
    }
    void operator=(net_list &val) {new(this)net_list(val);}
    bool operator==(net_list &src_val)
    {
        if(len == src_val.len)
        {
            auto p_tool = head.get(),
                p_src = src_val.head.get();
            for(auto i=0; i<len; ++i) if(p_tool->data == p_src->data)
            {
                p_tool = (p_tool -> next_node).get();
                p_src = (p_src -> next_node).get();
            }
            else return false;
            return true;
        }
        else return false;
    }
    bool operator!=(net_list &src_val) {return !(*this == src_val);}
};

template<typename _K, typename _V>class net_map
{
public:
    struct kv
    {
        _K key; _V value;
        void operator=(kv &val)
        {
            key = val.key;
            value = val.value;
        }
        void operator==(kv &val) {return ((key==val.key) && (value==val.value));}
        bool operator!=(kv &val) {return !(*this == val);}
        friend std::ostream &operator<<(std::ostream &output, kv &out_val)
        {
            output << "[Key]" << std::endl << out_val.key << std::endl << "[Value]" << std::endl << out_val.value;
            return output;
        }
    };
protected:
    net_list<kv> val;
    _V v_temp;
    kv kv_temp;
public:
    net_map() {}
    net_map(net_map &src) {val = src.val;}
    net_map(net_map &&src) {val = std::move(src.val);}
    uint64_t size() {return val.size();}
    uint64_t find_idx(_K &&key)
    {
        for(auto i=0; i<size(); ++i) if(val[i].key == key) return i;
        return 0;
    }
    uint64_t find_idx(_K &key) {return find_idx(std::move(key));}
    net_list<_K> find_key(_V &&value)
    {
        net_list<_K> key_set;
        for(auto i=0; i<size(); ++i) if(val[i].value == value) key_set.emplace_back(val[i].key);
        return key_set;
    }
    net_list<_K> find_key(_V &value) {return find_key(std::move(value));}
    bool insert(_K &&key, _V &&value)
    {
        if(this->find_idx(key) || val[IDX_ZERO].key==key) return false;
        else
        {
            kv in_temp;
            in_temp.key = key;
            in_temp.value = value;
            val.insert(size(), std::move(in_temp));
            return true;
        }
    }
    bool insert(_K &key, _V &value) {return insert(std::move(key), std::move(value));}
    kv erase(_K &&key)
    {
        for(auto i=0; i<size(); ++i) if(val[i].key == key) kv_temp = val.erase(i);
        return kv_temp;
    }
    kv erase(_K &key) {return erase(std::move(key));}
    kv &index(uint64_t idx)
    {
        if(idx < size()) return val[idx];
        else return kv_temp;
    }
    _V &operator[](_K &&key) 
    {
        auto tar_idx = find_idx(key);
        return val[tar_idx].value;
    }
    _V &operator[](_K &key) {return this->operator[](std::move(key));}
    void operator=(net_map &src) {new(this)net_map(src);}
    void operator=(net_map &&src) {new(this)net_map(std::move(src));}
    bool operator==(net_map &val) {return val == val.val;}
    bool operator!=(net_map &val) {return val != val.val;}
    friend std::ostream &operator<<(std::ostream &output, net_map &out_val)
    {
        for(auto i=0; i<out_val.val.size(); ++i)
        {
            output << '[' << i << ']' << out_val.val[i];
            if(i + 1 < out_val.val.size()) output << std::endl;
        }
        return output;
    }
    // ~net_map() {}
};

bool acc_valid(double acc)
{
    while(acc < 1) acc *= 10;
    return acc == 1;
}

double get_rand_base(long long seed, double acc = 1e-5)
{
    if(acc_valid(acc))
    {
        uint64_t expon = 10;
        while(expon * acc < 1) expon *= 10;
        auto base = seed % expon;
        return base * acc;
    }
    else return 0;
}

double random_number(double boundry_first, double boundry_second, bool to_sleep = false, double acc = 1e-5)
{
    if(boundry_first == boundry_second) return boundry_second;
    else if(acc_valid(acc))
    {
        if(to_sleep) _sleep(1);
        long long raw_pt = std::chrono::system_clock::now().time_since_epoch().count();
        auto seed_pt = get_rand_base(raw_pt, acc);
        return (boundry_first<boundry_second) ? (seed_pt*(boundry_second-boundry_first)+boundry_first) : ((seed_pt*(boundry_first-boundry_second))+boundry_second);
    }
    else return 0;
}

uint32_t swap_endian(uint32_t val)
{
	val = ((val << 8) & 0xFF00FF00) | ((val >> 8) & 0xFF00FF);
	return (val << 16) | (val >> 16);
}

std::pair<uint64_t, uint64_t> sci_num(double dec_part)
{
    dec_part = std::abs(dec_part);
    if(dec_part > 1) return std::make_pair(0, 0);
    else
    {
        uint64_t sum = 0, expon = 1, i_temp = 0;
        auto cnt = 0;
        do
        {
            auto d_temp = (float)dec_part;
            i_temp = (int)(d_temp * 10);
            dec_part *= 10;
            dec_part -= i_temp;
            expon *= 10;
            sum *= 10;
            sum += i_temp;
            ++ cnt;
        } while (i_temp != 0);
        return std::make_pair(sum/10, expon/10);
    }
}

std::string input_paragraph(uint64_t buffer_length = 2000)
{
    std::fflush(stdin);
    std::cout << "Please end submit with double enter." << std::endl;
    std::cout << std::endl; 
    auto cmtr = std::make_unique<char[]>(buffer_length);
    int i = 0;
    char temp, enter; 
    while(i < buffer_length){
        temp = std::getchar();
        if(temp == '\n'){
            enter = std::getchar();
            if(enter == '\n') break;
            else {
                cmtr[i] = '\n';
                cmtr[++i] = enter;
            }
        }else cmtr[i] = temp; i ++;
    }cmtr[i] = '\0';
    auto len = i + 1;
    std::string init_str = "";
    for(i=0; i<len; ++i) init_str += cmtr[i];
    std::fflush(stdin);
    return init_str;
}

std::unique_ptr<double[]> extract_number(std::string &num_str)
{
    int i = 0, cnt = 0, len = num_str.length(), amt = 0;
    // get the amount of the number from string
    while(i < len){
        if(num_str[i]=='.' || num_str[i]=='-' || num_str[i]=='+' || (num_str[i]>='0' && num_str[i]<='9'))
            if(i == 0 || num_str[i-1]==' '|| num_str[i-1]=='\t' || num_str[i-1]=='\n') cnt ++;
        i ++;
    } std::unique_ptr<double[]>num_seq = std::make_unique<double[]>(cnt);
    amt = cnt;
    cnt = 0;
    for(int j=0; j<amt; ++j){
        num_seq[j] = 0.0;
    } i = 0;
    while(i < len){
        double temp = 0.0, div = 0.0, dec = 0.0;
        bool neg = false, di = false;
        /*
        When iterate to a new number, go into the loop
        */
        if(num_str[i]=='.' || num_str[i]=='-' || num_str[i]=='+' || (num_str[i]>='0' && num_str[i]<='9')){
            // looping when meet a punctuated symbol
            bool de = false;
            int de_num = 0;
            while(num_str[i]!=' ' && num_str[i]!='\t' && num_str[i]!='\n' && num_str[i]!='\0'){
                if(num_str[i] == '-'){
                    // when meet a negative symbol sign convert to true
                    neg = true;
                }if(num_str[i]>='0' && num_str[i]<='9'){
                    // if meet decimal sign is true
                    if(de){
                        dec += (num_str[i]-'0')*1.0/std::pow(10, de_num);
                        de_num ++;
                    }else{
                        // if the fractional sign is true
                        if(di) div = div*10.0+(num_str[i]-'0')*1.0;
                        else temp = temp*10.0+(num_str[i]-'0')*1.0;
                    }
                }if(num_str[i] == '.'){
                    // this one is a decimal symbol
                    de = true;
                    de_num ++;
                }if(num_str[i] == '/'){
                    // this one is a fractional symbol
                    di = true;
                    temp += dec;
                    dec = 0.0;
                    de_num = 0;
                    de = false;
                }i ++;
            }if(di){
                div += dec;
                if(neg) div *= -1.0;
                temp /= div;
            }else{
                temp += dec;
                if(neg)temp *= -1.0;
            }
        } num_seq[cnt ++] = temp;
        i ++;
    }
    return num_seq;
}

std::string charset_exchange(std::wstring &str_src){
    auto nLen = str_src.length();
    char *psBuf = new char[nLen + 1];
    wcstombs(psBuf, str_src.c_str(), nLen);
    psBuf[nLen] = '\0';
    std::string strDest(psBuf);
    delete []psBuf;
    psBuf = nullptr;
    if(strDest.length() == nLen) return strDest;
    else return "";
}

std::wstring charset_exchange(std::string str_src){
    // setlocale(LC_ALL, "zh_CN.UTF-8");
    auto nLen = str_src.length();
    wchar_t *pwsBuf = new wchar_t[nLen + 1];
    mbstowcs(pwsBuf, str_src.c_str(), nLen);
    pwsBuf[nLen] = L'\0';
    std::wstring wstrDest(pwsBuf);
    delete []pwsBuf;
    pwsBuf = nullptr;
    if(wstrDest.length() == nLen) return wstrDest;
    else return L"";
}

net_queue<uint64_t> primes(uint64_t upper)
{
    net_queue<uint64_t> prm_set;
    prm_set.emplace_back(2);
    if(upper > 2) for(auto i=3; i<=upper; ++i)
    {
        bool p_flag = true;
        for(auto j=0; j<prm_set.size(); ++j) if(i%prm_set[j] == 0)
        {
            p_flag = false;
            break;
        }
        if(p_flag) prm_set.emplace_back(i);  
    }
    return prm_set;
}

net_queue<uint64_t> primes_fact(uint64_t val)
{
    auto prm_set = primes(val);
    auto itr_cnt = 0;
    net_queue<uint64_t> prm_fact;
    while(val > 1)
    {
        if(val%prm_set[itr_cnt] == 0)
        {
            val /= prm_set[itr_cnt];
            prm_fact.push_back(prm_set[itr_cnt]);
        }
        else ++ itr_cnt;
    }
    return prm_fact;
}

uint64_t greatest_common_divisor(uint64_t l_val, uint64_t r_val)
{
    auto itr_prm = primes_fact(l_val).unit_intersect(primes_fact(r_val));
    auto res = 1;
    for(auto i=0; i<itr_prm.size(); ++i) res *= itr_prm[i];
    return res;
}

uint64_t least_common_multiple(uint64_t l_val, uint64_t r_val)
{
    auto itr_prm = primes_fact(l_val).unit_union(primes_fact(r_val));
    auto res = 1;
    for(auto i=0; i<itr_prm.size(); ++i) res *= itr_prm[i];
    return res;
}

BAGRT_END