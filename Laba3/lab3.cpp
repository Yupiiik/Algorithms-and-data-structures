// Лабораторная работа №3. Реализация динамического массива. Итераторы
// Продвинутый уровень, вариант 5:
// Vector<T> + оператор конкатенации (+) + умножение на число (*)

#include <iostream>
#include <stdexcept>
#include <algorithm>
#include <iomanip>
#include <clocale>
#include <memory>

using namespace std;

// ============================================================
// Шаблонный класс Vector - динамический массив.
// Реализованы правило пяти, итераторы и операторы варианта 5.
// ============================================================
template <typename T>
class Vector {
private:
    T* data;             // указатель на динамический массив
    int sz;              // текущее количество элементов
    int cap;             // выделенная ёмкость
    long long copyCount; // количество копирований при расширении

public:
    // Конструктор по умолчанию
    Vector() : data(nullptr), sz(0), cap(0), copyCount(0) {}

    // Конструктор размера
    explicit Vector(int n) : data(nullptr), sz(0), cap(0), copyCount(0) {
        if (n < 0) {
            throw invalid_argument("Размер не может быть отрицательным");
        }
        if (n > 0) {
            data = new T[n]();
            sz = cap = n;
        }
    }

    // Конструктор размера с заполнением значением
    Vector(int n, const T& value) : data(nullptr), sz(0), cap(0), copyCount(0) {
        if (n < 0) {
            throw invalid_argument("Размер не может быть отрицательным");
        }
        if (n > 0) {
            unique_ptr<T[]> temp(new T[n]);
            for (int i = 0; i < n; i++) {
                temp[i] = value;
            }
            data = temp.release();
            sz = cap = n;
        }
    }

    // Конструктор копирования (глубокое копирование)
    Vector(const Vector& other)
        : data(nullptr), sz(other.sz), cap(other.cap), copyCount(other.copyCount) {
        if (cap > 0) {
            unique_ptr<T[]> temp(new T[cap]);
            for (int i = 0; i < sz; i++) {
                temp[i] = other.data[i];
            }
            data = temp.release();
        }
    }

    // Конструктор перемещения
    Vector(Vector&& other) noexcept
        : data(other.data), sz(other.sz), cap(other.cap),
          copyCount(other.copyCount) {
        other.data = nullptr;
        other.sz = 0;
        other.cap = 0;
        other.copyCount = 0;
    }

    // Деструктор
    ~Vector() {
        delete[] data;
    }

    // Оператор присваивания копированием
    Vector& operator=(const Vector& other) {
        if (this != &other) {                 // защита от самоприсваивания
            Vector temp(other);
            swap(temp);
        }
        return *this;
    }

    // Оператор присваивания перемещением
    Vector& operator=(Vector&& other) noexcept {
        if (this != &other) {
            delete[] data;
            data = other.data;
            sz = other.sz;
            cap = other.cap;
            copyCount = other.copyCount;

            other.data = nullptr;
            other.sz = 0;
            other.cap = 0;
            other.copyCount = 0;
        }
        return *this;
    }

    // Обмен содержимым двух векторов
    void swap(Vector& other) noexcept {
        std::swap(data, other.data);
        std::swap(sz, other.sz);
        std::swap(cap, other.cap);
        std::swap(copyCount, other.copyCount);
    }

    // Доступ по индексу с проверкой границ
    T& operator[](int index) {
        if (index < 0 || index >= sz) {
            throw out_of_range("Индекс выходит за границы Vector");
        }
        return data[index];
    }

    const T& operator[](int index) const {
        if (index < 0 || index >= sz) {
            throw out_of_range("Индекс выходит за границы Vector");
        }
        return data[index];
    }

    // Методы доступа
    int size() const { return sz; }
    int capacity() const { return cap; }
    bool empty() const { return sz == 0; }
    long long getCopyCount() const { return copyCount; }
    void resetCopyCount() { copyCount = 0; }

    // Резервирование памяти
    void reserve(int newCap) {
        if (newCap <= cap) return;
        unique_ptr<T[]> newData(new T[newCap]);
        for (int i = 0; i < sz; i++) {
            newData[i] = data[i];
            copyCount++;
        }
        delete[] data;
        data = newData.release();
        cap = newCap;
    }

    // Добавление элемента в конец (ёмкость удваивается)
    void push_back(const T& value) {
        T valueCopy = value; // безопасно, даже если value ссылается на элемент Vector
        if (sz >= cap) {
            int newCap = (cap == 0) ? 1 : cap * 2;
            reserve(newCap);
        }
        data[sz++] = valueCopy;
    }

    // Удаление последнего элемента
    void pop_back() {
        if (empty()) {
            throw underflow_error("Нельзя удалить элемент из пустого Vector");
        }
        sz--;
    }

    // Изменение размера
    void resize(int newSize, const T& defaultValue = T()) {
        if (newSize < 0) {
            throw invalid_argument("Размер не может быть отрицательным");
        }
        if (newSize > cap) {
            reserve(newSize);
        }
        if (newSize > sz) {
            for (int i = sz; i < newSize; i++) {
                data[i] = defaultValue;
            }
        }
        sz = newSize;
    }

    // Очистка без освобождения зарезервированной памяти
    void clear() {
        sz = 0;
    }

    // Итераторы (указатели на первый элемент и позицию после последнего)
    T* begin() { return data; }
    T* end() { return data ? data + sz : data; }
    const T* begin() const { return data; }
    const T* end() const { return data ? data + sz : data; }

    // Оператор + : конкатенация двух векторов
    Vector operator+(const Vector& other) const {
        Vector result;
        result.reserve(sz + other.sz);
        for (int i = 0; i < sz; i++) result.push_back(data[i]);
        for (int i = 0; i < other.sz; i++) result.push_back(other.data[i]);
        return result;
    }

    // Оператор * : умножение каждого элемента на число
    Vector operator*(const T& number) const {
        Vector result(sz);
        for (int i = 0; i < sz; i++) {
            result.data[i] = data[i] * number;
        }
        return result;
    }

    // Поиск элемента. Возвращает указатель на найденный элемент или nullptr.
    T* find(const T& value) {
        for (int i = 0; i < sz; i++) {
            if (data[i] == value) return &data[i];
        }
        return nullptr;
    }

    const T* find(const T& value) const {
        for (int i = 0; i < sz; i++) {
            if (data[i] == value) return &data[i];
        }
        return nullptr;
    }

    // Проверка наличия элемента
    bool contains(const T& value) const {
        return find(value) != nullptr;
    }

    // Вывод содержимого
    void print() const {
        cout << "[";
        for (int i = 0; i < sz; i++) {
            cout << data[i];
            if (i < sz - 1) cout << ", ";
        }
        cout << "]";
    }
};

// ============================================================
// Демонстрация работы Vector
// ============================================================
int main() {
    setlocale(LC_ALL, "ru");

    try {
        cout << "=== 1. Создание динамического массива ===" << endl;
        Vector<int> v;
        cout << "Начальный размер: " << v.size()
             << ", ёмкость: " << v.capacity() << endl;

        cout << "\n=== 2. Добавление элементов и расширение ===" << endl;
        for (int i = 1; i <= 8; i++) {
            v.push_back(i);
            cout << "push_back(" << i << "): размер=" << v.size()
                 << ", ёмкость=" << v.capacity() << endl;
        }

        cout << "Содержимое: ";
        v.print();
        cout << endl;

        cout << "\n=== 3. Итераторы ===" << endl;
        cout << "Перебор через range-based for: ";
        for (int x : v) cout << x << " ";
        cout << endl;

        cout << "Перебор через begin/end: ";
        for (auto it = v.begin(); it != v.end(); ++it) cout << *it << " ";
        cout << endl;

        cout << "\n=== 4. Операции доступа и изменения ===" << endl;
        cout << "v[3] = " << v[3] << endl;
        cout << "find(5): " << (v.find(5) ? "элемент найден" : "не найден") << endl;
        cout << "contains(20): " << (v.contains(20) ? "true" : "false") << endl;
        v.pop_back();
        cout << "После pop_back: ";
        v.print();
        cout << ", размер=" << v.size() << endl;

        v.resize(8, 10);
        cout << "После resize(8, 10): ";
        v.print();
        cout << endl;

        cout << "\n=== 5. Правило пяти ===" << endl;
        Vector<int> copied(v); // конструктор копирования
        cout << "Копия: ";
        copied.print();
        cout << endl;

        Vector<int> assigned;
        assigned = v; // присваивание копированием
        cout << "После копирующего присваивания: ";
        assigned.print();
        cout << endl;

        Vector<int> moved(std::move(copied)); // конструктор перемещения
        cout << "Перемещённый вектор: ";
        moved.print();
        cout << "; исходный размер после перемещения=" << copied.size() << endl;

        Vector<int> moveAssigned;
        moveAssigned = std::move(assigned); // присваивание перемещением
        cout << "После перемещающего присваивания: ";
        moveAssigned.print();
        cout << endl;

        cout << "\n=== 6. Вариант 5: оператор + (конкатенация) ===" << endl;
        Vector<int> a(3, 2);
        Vector<int> b(2, 7);
        Vector<int> joined = a + b;
        cout << "Первый вектор: "; a.print(); cout << endl;
        cout << "Второй вектор: "; b.print(); cout << endl;
        cout << "Результат a + b: "; joined.print(); cout << endl;

        cout << "\n=== 7. Вариант 5: оператор * (умножение на число) ===" << endl;
        Vector<int> multiplied = joined * 3;
        cout << "Результат (a + b) * 3: ";
        multiplied.print();
        cout << endl;

        cout << "\n=== 8. Амортизированный анализ push_back ===" << endl;
        cout << setw(10) << "n"
             << setw(18) << "Копирований"
             << setw(18) << "Всего операций"
             << setw(15) << "Среднее" << endl;
        cout << string(61, '-') << endl;

        for (int n = 1000; n <= 100000; n *= 10) {
            Vector<int> test;
            for (int i = 0; i < n; i++) test.push_back(i);
            long long totalOps = n + test.getCopyCount();
            double average = static_cast<double>(totalOps) / n;
            cout << setw(10) << n
                 << setw(18) << test.getCopyCount()
                 << setw(18) << totalOps
                 << setw(15) << fixed << setprecision(2) << average << endl;
        }

        cout << "\n=== 9. Обработка исключений ===" << endl;
        try {
            cout << v[100] << endl;
        } catch (const out_of_range& e) {
            cout << "Перехвачено исключение: " << e.what() << endl;
        }

        try {
            Vector<int> emptyVector;
            emptyVector.pop_back();
        } catch (const underflow_error& e) {
            cout << "Перехвачено исключение: " << e.what() << endl;
        }

        cout << "\n=== 10. Очистка ===" << endl;
        v.clear();
        cout << "После clear: размер=" << v.size()
             << ", ёмкость=" << v.capacity()
             << ", empty=" << (v.empty() ? "true" : "false") << endl;

    } catch (const exception& e) {
        cerr << "Ошибка: " << e.what() << endl;
        return 1;
    }

    return 0;
}
