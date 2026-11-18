// Лабораторная работа №2. Продвинутый уровень, вариант 5.
// Шаблонный контейнер Deque<T> — двусторонняя очередь.

#include <iostream>
#include <deque>
#include <chrono>
#include <iomanip>
#include <stdexcept>
#include <clocale>

using namespace std;

// Дек хранит элементы в циклическом динамическом массиве.
template <typename T>
class Deque {
private:
    T* data;                 // Массив с элементами
    size_t capacity;         // Вместимость массива
    size_t count;            // Количество элементов
    size_t head;             // Индекс первого элемента

    // Увеличение массива в два раза. Выполняется только когда массив заполнен.
    void grow() {
        size_t newCapacity = capacity * 2;
        T* newData = new T[newCapacity];

        // Переносим элементы в новый массив в правильном порядке.
        for (size_t i = 0; i < count; ++i) {
            newData[i] = data[(head + i) % capacity];
        }

        delete[] data;
        data = newData;
        capacity = newCapacity;
        head = 0;
    }

public:
    // Создание пустого дека.
    Deque() : data(new T[4]), capacity(4), count(0), head(0) {}

    // Освобождение памяти.
    ~Deque() {
        delete[] data;
    }

    // Добавить элемент в конец.
    void push_back(const T& value) {
        if (count == capacity) grow();
        data[(head + count) % capacity] = value;
        ++count;
    }

    // Добавить элемент в начало.
    void push_front(const T& value) {
        if (count == capacity) grow();
        head = (head + capacity - 1) % capacity;
        data[head] = value;
        ++count;
    }

    // Удалить последний элемент.
    void pop_back() {
        if (count == 0) throw out_of_range("Deque пуст");
        --count;
    }

    // Удалить первый элемент.
    void pop_front() {
        if (count == 0) throw out_of_range("Deque пуст");
        head = (head + 1) % capacity;
        --count;
        if (count == 0) head = 0;
    }

    // Получить элемент по логическому индексу.
    T& operator[](size_t index) {
        if (index >= count) throw out_of_range("Неверный индекс");
        return data[(head + index) % capacity];
    }

    // Количество элементов — вспомогательный метод для проверки и тестирования.
    size_t size() const {
        return count;
    }
};

// Измерение времени выполнения одинаковой последовательности операций.
template <typename Container>
long long testTime(size_t n) {
    Container values;
    auto start = chrono::high_resolution_clock::now();

    for (size_t i = 0; i < n; ++i) {
        if (i % 2 == 0) values.push_back(static_cast<int>(i));
        else values.push_front(static_cast<int>(i));
    }

    // Обращаемся к элементам по индексу.
    long long sum = 0;
    for (size_t i = 0; i < values.size(); i += (values.size() / 10 + 1)) {
        sum += values[i];
    }

    while (values.size() > 0) {
        if (values.size() % 2 == 0) values.pop_front();
        else values.pop_back();
    }

    auto finish = chrono::high_resolution_clock::now();
    (void)sum;
    return chrono::duration_cast<chrono::microseconds>(finish - start).count();
}

int main() {
    setlocale(LC_ALL, "Russian");

    cout << "Лабораторная работа №2. Вариант 5 — Deque<T>\n\n";

    Deque<int> numbers;
    numbers.push_back(20);
    numbers.push_front(10);
    numbers.push_back(30);

    cout << "После push_front и push_back: ";
    for (size_t i = 0; i < numbers.size(); ++i) cout << numbers[i] << " ";

    numbers.pop_front();
    numbers.pop_back();
    cout << "\nПосле pop_front и pop_back: ";
    for (size_t i = 0; i < numbers.size(); ++i) cout << numbers[i] << " ";

    cout << "\n\nСравнение с std::deque (время в микросекундах):\n";
    cout << "Элементов | Deque<T> | std::deque\n";

    const size_t testSizes[] = {10000, 100000, 500000};
    for (size_t n : testSizes) {
        cout << n << " | " << testTime<Deque<int>>(n)
             << " | " << testTime<deque<int>>(n) << '\n';
    }

    cout << "\nРабота программы завершена.\n";
    return 0;
}
