#include <iostream>
#include <stdexcept>
#include <chrono>
#include <utility>
#include <cstddef>

// Односвязный список: узел хранит данные и указатель next.
template<class T>
class SinglyLinkedList {
    struct Node {
        T data; Node* next;
        explicit Node(const T& v) : data(v), next(nullptr) {}
    };
    Node* head = nullptr;
    Node* tail = nullptr;
    std::size_t count = 0;

    void swap(SinglyLinkedList& x) noexcept {
        std::swap(head, x.head); std::swap(tail, x.tail); std::swap(count, x.count);
    }
    static Node* merge(Node* a, Node* b) {
        Node* result = nullptr;
        Node* last = nullptr;
        while (a && b) {
            Node* selected;
            if (a->data <= b->data) { selected = a; a = a->next; }
            else { selected = b; b = b->next; }
            if (!result) result = selected;
            else last->next = selected;
            last = selected;
        }
        if (last) last->next = a ? a : b;
        else result = a ? a : b;
        return result;
    }
    static Node* sortNodes(Node* first) {
        if (!first || !first->next) return first;
        Node* slow = first; Node* fast = first->next;
        while (fast && fast->next) { slow = slow->next; fast = fast->next->next; }
        Node* right = slow->next; slow->next = nullptr;
        return merge(sortNodes(first), sortNodes(right));
    }
public:
    class Iterator {
        Node* p;
    public:
        explicit Iterator(Node* n=nullptr) : p(n) {}
        T& operator*() const { return p->data; }
        T* operator->() const { return &p->data; }
        Iterator& operator++() { if (p) p=p->next; return *this; }
        Iterator operator++(int) { Iterator t=*this; ++*this; return t; }
        bool operator==(Iterator x) const { return p==x.p; }
        bool operator!=(Iterator x) const { return p!=x.p; }
    };

    SinglyLinkedList() = default;
    SinglyLinkedList(const SinglyLinkedList& x) {
        try { for (const T& v : x) push_back(v); } catch (...) { clear(); throw; }
    }
    SinglyLinkedList(SinglyLinkedList&& x) noexcept : head(x.head), tail(x.tail), count(x.count) {
        x.head=x.tail=nullptr; x.count=0;
    }
    SinglyLinkedList& operator=(const SinglyLinkedList& x) {
        if (this!=&x) { SinglyLinkedList t(x); swap(t); } return *this;
    }
    SinglyLinkedList& operator=(SinglyLinkedList&& x) noexcept {
        if (this!=&x) { clear(); head=x.head; tail=x.tail; count=x.count;
            x.head=x.tail=nullptr; x.count=0; } return *this;
    }
    ~SinglyLinkedList() { clear(); }

    void push_front(const T& v) {
        Node* n=new Node(v); n->next=head; head=n; if (!tail) tail=n; ++count;
    }
    void push_back(const T& v) {
        Node* n=new Node(v); if (!tail) head=tail=n; else { tail->next=n; tail=n; } ++count;
    }
    void pop_front() {
        if (!head) throw std::underflow_error("Нельзя удалить элемент из пустого односвязного списка");
        Node* n=head; head=head->next; delete n; --count; if (!head) tail=nullptr;
    }
    void pop_back() {
        if (!head) throw std::underflow_error("Нельзя удалить элемент из пустого односвязного списка");
        if (head==tail) { delete head; head=tail=nullptr; }
        else { Node* p=head; while (p->next!=tail) p=p->next;
            delete tail; tail=p; tail->next=nullptr; } --count;
    }
    Node* find(const T& v) const {
        for (Node* p=head; p; p=p->next) {
            if (p->data==v) return p;
        }
        return nullptr;
    }
    bool remove(const T& v) {
        if (!head) return false;
        if (head->data==v) { pop_front(); return true; }
        Node* p=head; while (p->next && !(p->next->data==v)) p=p->next;
        if (!p->next) return false;
        Node* n=p->next; p->next=n->next; if (n==tail) tail=p; delete n; --count; return true;
    }
    void mergeSort() { head=sortNodes(head); tail=head; while (tail && tail->next) tail=tail->next; }
    bool hasCycle() const {
        Node* slow=head; Node* fast=head;
        while (fast && fast->next) { slow=slow->next; fast=fast->next->next; if (slow==fast) return true; }
        return false;
    }
    void clear() noexcept {
        while (head) { Node* n=head->next; delete head; head=n; } tail=nullptr; count=0;
    }
    std::size_t size() const noexcept { return count; }
    bool empty() const noexcept { return count==0; }
    Iterator begin() const { return Iterator(head); }
    Iterator end() const { return Iterator(nullptr); }
    bool operator==(const SinglyLinkedList& x) const {
        if (count!=x.count) return false;
        Node* a=head; Node* b=x.head;
        while (a && b) { if (!(a->data==b->data)) return false; a=a->next; b=b->next; }
        return !a && !b;
    }
    bool operator!=(const SinglyLinkedList& x) const { return !(*this==x); }
    void print() const {
        std::cout << "["; Node* p=head;
        while (p) { std::cout << p->data; if (p->next) std::cout << ", "; p=p->next; }
        std::cout << "]";
    }
};

// Двусвязный список: каждый узел хранит prev и next.
template<class T>
class DoublyLinkedList {
    struct Node {
        T data; Node* prev; Node* next;
        explicit Node(const T& v) : data(v), prev(nullptr), next(nullptr) {}
    };
    Node* head=nullptr; Node* tail=nullptr; std::size_t count=0;
    void swap(DoublyLinkedList& x) noexcept {
        std::swap(head,x.head); std::swap(tail,x.tail); std::swap(count,x.count);
    }
public:
    class Iterator {
        Node* p;
    public:
        explicit Iterator(Node* n=nullptr):p(n){}
        T& operator*() const { return p->data; }
        T* operator->() const { return &p->data; }
        Iterator& operator++(){if(p)p=p->next;return *this;}
        Iterator operator++(int){Iterator t=*this;++*this;return t;}
        Iterator& operator--(){if(p)p=p->prev;return *this;}
        Iterator operator--(int){Iterator t=*this;--*this;return t;}
        bool operator==(Iterator x)const{return p==x.p;}
        bool operator!=(Iterator x)const{return p!=x.p;}
    };
    DoublyLinkedList()=default;
    DoublyLinkedList(const DoublyLinkedList& x) {
        try { for (const T& v:x) push_back(v); } catch (...) { clear(); throw; }
    }
    DoublyLinkedList(DoublyLinkedList&& x) noexcept:head(x.head),tail(x.tail),count(x.count){
        x.head=x.tail=nullptr;x.count=0;
    }
    DoublyLinkedList& operator=(const DoublyLinkedList& x){
        if(this!=&x){DoublyLinkedList t(x);swap(t);}return *this;
    }
    DoublyLinkedList& operator=(DoublyLinkedList&& x) noexcept{
        if(this!=&x){clear();head=x.head;tail=x.tail;count=x.count;x.head=x.tail=nullptr;x.count=0;}return *this;
    }
    ~DoublyLinkedList(){clear();}
    void push_front(const T& v){
        Node* n=new Node(v);n->next=head;if(head)head->prev=n;else tail=n;head=n;++count;
    }
    void push_back(const T& v){
        Node* n=new Node(v);n->prev=tail;if(tail)tail->next=n;else head=n;tail=n;++count;
    }
    void pop_front(){
        if(!head)throw std::underflow_error("Нельзя удалить элемент из пустого двусвязного списка");
        Node* n=head;head=head->next;if(head)head->prev=nullptr;else tail=nullptr;delete n;--count;
    }
    void pop_back(){
        if(!tail)throw std::underflow_error("Нельзя удалить элемент из пустого двусвязного списка");
        Node* n=tail;tail=tail->prev;if(tail)tail->next=nullptr;else head=nullptr;delete n;--count;
    }
    Node* find(const T& v)const{for(Node*p=head;p;p=p->next)if(p->data==v)return p;return nullptr;}
    // Проверка цикла алгоритмом Флойда (черепаха и заяц).
    bool hasCycle() const {
        Node* slow=head; Node* fast=head;
        while (fast && fast->next) {
            slow=slow->next; fast=fast->next->next;
            if (slow==fast) return true;
        }
        return false;
    }
    bool remove(const T& v){
        Node*n=find(v);if(!n)return false;
        if(n==head){pop_front();return true;}if(n==tail){pop_back();return true;}
        n->prev->next=n->next;n->next->prev=n->prev;delete n;--count;return true;
    }
    void insert_after(Node* pos,const T& v){
        if(!pos){push_front(v);return;}Node*n=new Node(v);n->prev=pos;n->next=pos->next;
        if(pos->next)pos->next->prev=n;else tail=n;pos->next=n;++count;
    }
    void clear()noexcept{while(head){Node*n=head->next;delete head;head=n;}tail=nullptr;count=0;}
    std::size_t size()const noexcept{return count;}
    bool empty()const noexcept{return count==0;}
    Iterator begin()const{return Iterator(head);}
    Iterator end()const{return Iterator(nullptr);}
    bool operator==(const DoublyLinkedList& x)const{
        if(count!=x.count) return false;
        Node* a=head; Node* b=x.head;
        while(a&&b){if(!(a->data==b->data))return false;a=a->next;b=b->next;}return !a&&!b;
    }
    bool operator!=(const DoublyLinkedList& x)const{return !(*this==x);}
    void print()const{
        std::cout<<"[";Node*p=head;while(p){std::cout<<p->data;if(p->next)std::cout<<", ";p=p->next;}std::cout<<"]";
    }
    void printReverse()const{
        std::cout<<"[";Node*p=tail;while(p){std::cout<<p->data;if(p->prev)std::cout<<", ";p=p->prev;}std::cout<<"]";
    }
};

int main(){
    std::cout<<"=== Лабораторная работа №4: связные списки ===\n\n";
    std::cout<<"1. Односвязный список\n";
    SinglyLinkedList<int> s;for(int x:{7,2,9,1,5,3})s.push_back(x);
    std::cout<<"Исходный список: ";s.print();std::cout<<"\nРазмер: "<<s.size()<<"\n";
    std::cout<<"find(5): "<<(s.find(5)?"элемент найден":"не найден")<<"\n";
    std::cout<<"hasCycle(): "<<(s.hasCycle()?"true":"false")<<"\n";
    s.mergeSort();std::cout<<"После mergeSort(): ";s.print();std::cout<<"\n";
    s.pop_front();s.pop_back();std::cout<<"После pop_front() и pop_back(): ";s.print();std::cout<<"\n";

    std::cout<<"\n2. Двусвязный список\n";
    DoublyLinkedList<int>d;for(int x:{10,20,30,40})d.push_back(x);d.push_front(5);
    std::cout<<"Прямой обход: ";d.print();std::cout<<"\nОбратный обход: ";d.printReverse();std::cout<<"\n";
    std::cout<<"find(30): "<<(d.find(30)?"элемент найден":"не найден")<<"\n";
    std::cout<<"hasCycle() двусвязного списка: "<<(d.hasCycle()?"true":"false")<<"\n";
    d.remove(20);std::cout<<"После remove(20): ";d.print();std::cout<<"\nРазмер: "<<d.size()<<"\n";

    std::cout<<"\n3. Итераторы и сравнение\nОбход односвязного списка: ";
    for(auto it=s.begin();it!=s.end();++it) std::cout<<*it<<" ";
    std::cout<<"\n";
    SinglyLinkedList<int> copy=s;
    std::cout<<"Копия равна оригиналу (==): "<<(copy==s?"true":"false")<<"\n";
    copy.push_back(100);std::cout<<"После изменения копии списки различны (!=): "<<(copy!=s?"true":"false")<<"\n";

    std::cout<<"\n4. Пустой список и очистка\n";
    DoublyLinkedList<int> empty;
    try{empty.pop_back();}catch(const std::underflow_error&e){std::cout<<"Перехвачено исключение: "<<e.what()<<"\n";}
    d.clear();std::cout<<"После clear(): size="<<d.size()<<", empty="<<(d.empty()?"true":"false")<<"\n";

    std::cout<<"\n5. Сравнение времени вставки в конец\n";
    const int N=50000;
    auto a=std::chrono::steady_clock::now();SinglyLinkedList<int> bs;for(int i=0;i<N;++i)bs.push_back(i);
    auto b=std::chrono::steady_clock::now();DoublyLinkedList<int> bd;for(int i=0;i<N;++i)bd.push_back(i);
    auto c=std::chrono::steady_clock::now();
    auto us1=std::chrono::duration_cast<std::chrono::microseconds>(b-a).count();
    auto us2=std::chrono::duration_cast<std::chrono::microseconds>(c-b).count();
    std::cout<<"Количество элементов: "<<N<<"\nОдносвязный список: "<<us1<<" мкс\nДвусвязный список: "<<us2<<" мкс\n";
    std::cout<<"В обоих списках хранится tail, поэтому push_back имеет O(1); время зависит от среды.\n";
}
