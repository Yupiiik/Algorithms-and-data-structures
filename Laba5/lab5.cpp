// Лабораторная работа №5. Стек, очередь, дек, приоритетная очередь
// Продвинутый уровень, вариант 5

#include <iostream>
#include <string>
#include <stdexcept>
#include <sstream>
#include <clocale>

using namespace std;

// ============================================================
// Стек на динамическом массиве (LIFO)
// ============================================================
template <typename T>
class Stack {
private:
    T* data;        // динамический массив элементов
    int topIndex;   // индекс вершины (-1, если пусто)
    int capacity;   // текущая вместимость

    // Увеличение массива вдвое при переполнении
    void resize() {
        int newCapacity = capacity * 2;
        T* newData = new T[newCapacity];
        for (int i = 0; i <= topIndex; i++)
            newData[i] = data[i];   // копируем элементы
        delete[] data;
        data = newData;
        capacity = newCapacity;
    }

public:
    Stack(int initialCapacity = 10)
        : capacity(initialCapacity), topIndex(-1) {
        data = new T[capacity];
    }

    ~Stack() { delete[] data; }

    // Копирование запрещено: класс владеет сырым массивом
    Stack(const Stack&) = delete;
    Stack& operator=(const Stack&) = delete;

    void push(const T& value) {
        if (topIndex >= capacity - 1) resize();
        data[++topIndex] = value;   // кладём на вершину
    }

    T pop() {
        if (isEmpty()) throw underflow_error("Stack is empty");
        return data[topIndex--];    // снимаем с вершины
    }

    T top() const {
        if (isEmpty()) throw underflow_error("Stack is empty");
        return data[topIndex];
    }

    bool isEmpty() const { return topIndex == -1; }
    int size() const { return topIndex + 1; }
};

// ============================================================
// Очередь на связном списке (FIFO)
// ============================================================
template <typename T>
class Queue {
private:
    struct Node {           // узел списка
        T value;
        Node* next;
        Node(const T& v) : value(v), next(nullptr) {}
    };
    Node* head;             // начало (откуда забираем)
    Node* tail;             // конец (куда добавляем)
    int count;

public:
    Queue() : head(nullptr), tail(nullptr), count(0) {}
    ~Queue() {
        while (head) {      // освобождаем все узлы
            Node* tmp = head;
            head = head->next;
            delete tmp;
        }
    }

    Queue(const Queue&) = delete;
    Queue& operator=(const Queue&) = delete;

    void enqueue(const T& value) {
        Node* node = new Node(value);
        if (tail) tail->next = node;   // привязываем к хвосту
        else      head = node;         // очередь была пуста
        tail = node;
        count++;
    }

    T dequeue() {
        if (isEmpty()) throw underflow_error("Очередь пуста");
        Node* tmp = head;
        T value = head->value;
        head = head->next;
        if (!head) tail = nullptr;     // очередь опустела
        delete tmp;
        count--;
        return value;
    }

    T front() const {
        if (isEmpty()) throw underflow_error("Очередь пуста");
        return head->value;
    }

    bool isEmpty() const { return head == nullptr; }
    int size() const { return count; }
};

// ============================================================
// Дек на кольцевом динамическом массиве
// ============================================================
template <typename T>
class Deque {
private:
    T* data;
    int capacity;
    int frontIdx;   // индекс первого элемента
    int count;      // число элементов

    // Расширение кольцевого буфера вдвое
    void resize() {
        int newCapacity = capacity * 2;
        T* newData = new T[newCapacity];
        for (int i = 0; i < count; i++)
            newData[i] = data[(frontIdx + i) % capacity]; // сохраняем порядок
        delete[] data;
        data = newData;
        capacity = newCapacity;
        frontIdx = 0;
    }

public:
    Deque(int initialCapacity = 8)
        : capacity(initialCapacity), frontIdx(0), count(0) {
        data = new T[capacity];
    }

    ~Deque() { delete[] data; }

    Deque(const Deque&) = delete;
    Deque& operator=(const Deque&) = delete;

    void push_front(const T& value) {
        if (count == capacity) resize();
        frontIdx = (frontIdx - 1 + capacity) % capacity; // шаг назад по кольцу
        data[frontIdx] = value;
        count++;
    }

    void push_back(const T& value) {
        if (count == capacity) resize();
        int backIdx = (frontIdx + count) % capacity;
        data[backIdx] = value;
        count++;
    }

    T pop_front() {
        if (isEmpty()) throw underflow_error("Дека пуста");
        T value = data[frontIdx];
        frontIdx = (frontIdx + 1) % capacity;
        count--;
        return value;
    }

    T pop_back() {
        if (isEmpty()) throw underflow_error("Дека пуста");
        int backIdx = (frontIdx + count - 1) % capacity;
        T value = data[backIdx];
        count--;
        return value;
    }

    T front() const {
        if (isEmpty()) throw underflow_error("Дека пуста");
        return data[frontIdx];
    }

    T back() const {
        if (isEmpty()) throw underflow_error("Дека пуста");
        return data[(frontIdx + count - 1) % capacity];
    }

    bool isEmpty() const { return count == 0; }
    int size() const { return count; }
};

// ============================================================
// Приоритетная очередь на двоичной куче (max-heap)
// ============================================================
template <typename T>
class PriorityQueue {
private:
    T* heap;        // массив-куча
    int capacity;
    int count;

    void resize() {
        int newCapacity = capacity * 2;
        T* newHeap = new T[newCapacity];
        for (int i = 0; i < count; i++) newHeap[i] = heap[i];
        delete[] heap;
        heap = newHeap;
        capacity = newCapacity;
    }

    // Просеивание вверх после добавления
    void siftUp(int i) {
        while (i > 0) {
            int parent = (i - 1) / 2;
            if (heap[parent] >= heap[i]) break;   // свойство кучи выполнено
            swap(heap[parent], heap[i]);
            i = parent;
        }
    }

    // Просеивание вниз после извлечения максимума
    void siftDown(int i) {
        while (true) {
            int left = 2 * i + 1, right = 2 * i + 2, largest = i;
            if (left  < count && heap[left]  > heap[largest]) largest = left;
            if (right < count && heap[right] > heap[largest]) largest = right;
            if (largest == i) break;
            swap(heap[i], heap[largest]);
            i = largest;
        }
    }

public:
    PriorityQueue(int initialCapacity = 8)
        : capacity(initialCapacity), count(0) {
        heap = new T[capacity];
    }

    ~PriorityQueue() { delete[] heap; }

    PriorityQueue(const PriorityQueue&) = delete;
    PriorityQueue& operator=(const PriorityQueue&) = delete;

    void push(const T& value) {
        if (count == capacity) resize();
        heap[count] = value;
        siftUp(count);      // восстанавливаем свойство кучи
        count++;
    }

    T pop() {
        if (isEmpty()) throw underflow_error("PriorityQueue пуст");
        T topValue = heap[0];
        heap[0] = heap[--count];   // последний элемент — на корень
        siftDown(0);               // просеиваем вниз
        return topValue;
    }

    T top() const {
        if (isEmpty()) throw underflow_error("PriorityQueue пуст");
        return heap[0];
    }

    bool isEmpty() const { return count == 0; }
    int size() const { return count; }
};

// ============================================================
// Применение стека №1: проверка сбалансированности скобок
// ============================================================
bool checkBrackets(const string& expr) {
    Stack<char> stack;
    for (char ch : expr) {
        // Открывающая скобка — кладём в стек
        if (ch == '(' || ch == '[' || ch == '{') {
            stack.push(ch);
        }
        // Закрывающая — проверяем пару с вершиной
        else if (ch == ')' || ch == ']' || ch == '}') {
            if (stack.isEmpty()) return false;
            char top = stack.pop();
            if ((ch == ')' && top != '(') ||
                (ch == ']' && top != '[') ||
                (ch == '}' && top != '{')) return false;
        }
    }
    return stack.isEmpty();   // в конце стек должен быть пуст
}

// ============================================================
// Применение стека №2: вычисление обратной польской записи
// ============================================================
double evalRPN(const string& expr) {
    Stack<double> stack;
    istringstream iss(expr);
    string token;
    while (iss >> token) {
        // Операнд — число, кладём в стек
        if (token != "+" && token != "-" && token != "*" && token != "/") {
            stack.push(stod(token));
            continue;
        }
        // Оператор — снимаем два операнда (порядок важен!)
        if (stack.size() < 2) throw runtime_error("Недостаточно операндов");
        double b = stack.pop();
        double a = stack.pop();
        if (token == "+")      stack.push(a + b);
        else if (token == "-") stack.push(a - b);
        else if (token == "*") stack.push(a * b);
        else {
            if (b == 0) throw runtime_error("Деление на ноль");
            stack.push(a / b);
        }
    }
    if (stack.size() != 1) throw runtime_error("Некорректное выражение");
    return stack.pop();
}

// ============================================================
// Демонстрация
// ============================================================
int main() {
    setlocale(LC_ALL, "ru");

    cout << "=== 1. Стек (LIFO) ===" << endl;
    Stack<int> st;
    st.push(1); st.push(2); st.push(3);
    cout << "top = " << st.top() << ", size = " << st.size() << endl;
    cout << "pop = " << st.pop() << ", pop = " << st.pop() << endl;

    cout << "\n=== 2. Очередь (FIFO) ===" << endl;
    Queue<int> q;
    q.enqueue(1); q.enqueue(2); q.enqueue(3);
    cout << "front = " << q.front() << ", size = " << q.size() << endl;
    cout << "dequeue = " << q.dequeue() << ", dequeue = " << q.dequeue() << endl;

    cout << "\n=== 3. Дек ===" << endl;
    Deque<int> dq;
    dq.push_back(2); dq.push_back(3);
    dq.push_front(1);
    cout << "front = " << dq.front() << ", back = " << dq.back()
        << ", size = " << dq.size() << endl;
    cout << "pop_front = " << dq.pop_front()
        << ", pop_back = " << dq.pop_back() << endl;

    cout << "\n=== 4. Приоритетная очередь (max-heap) ===" << endl;
    PriorityQueue<int> pq;
    pq.push(5); pq.push(1); pq.push(9); pq.push(3);
    cout << "top = " << pq.top() << endl;
    cout << "pop = " << pq.pop() << ", pop = " << pq.pop()
        << ", pop = " << pq.pop() << endl;

    cout << "\n=== 5. Проверка скобок ===" << endl;
    cout << "\"(a[b]{c})\" -> " << (checkBrackets("(a[b]{c})") ? "true" : "false") << endl;
    cout << "\"(a[b)}c\"   -> " << (checkBrackets("(a[b)}c)") ? "true" : "false") << endl;
    cout << "\"((()))\"    -> " << (checkBrackets("((()))") ? "true" : "false") << endl;

    cout << "\n=== 6. Обратная польская запись ===" << endl;
    cout << "\"3 4 + 2 *\"   = " << evalRPN("3 4 + 2 *") << endl;   // (3+4)*2 = 14
    cout << "\"5 1 2 + 4 * + 3 -\" = " << evalRPN("5 1 2 + 4 * + 3 -") << endl;

    cout << "\n=== 7. Граничные случаи ===" << endl;
    try { st.pop(); st.pop(); st.pop(); }   // стек пуст
    catch (const exception& e) { cout << "  [!] " << e.what() << endl; }

    try { evalRPN("1 0 /"); }                // деление на ноль
    catch (const exception& e) { cout << "  [!] " << e.what() << endl; }

    try { evalRPN("1 +"); }                  // недостаточно операндов
    catch (const exception& e) { cout << "  [!] " << e.what() << endl; }

    return 0;
}