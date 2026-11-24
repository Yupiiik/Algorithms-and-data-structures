#include <iostream>
#include <vector>
#include <list>
#include <string>
#include <algorithm>
#include <chrono>
#include <iomanip>

using namespace std;
using namespace chrono;

// Итеративный двоичный поиск: возвращает индекс первого элемента >= key.
int lowerBound(const vector<int>& a, int key) {
    int left = 0;
    int right = static_cast<int>(a.size());

    while (left < right) {
        int mid = left + (right - left) / 2;

        if (a[mid] < key) {
            left = mid + 1;
        } else {
            right = mid;
        }
    }

    return left;
}

// Возвращает индекс первого элемента > key.
int upperBound(const vector<int>& a, int key) {
    int left = 0;
    int right = static_cast<int>(a.size());

    while (left < right) {
        int mid = left + (right - left) / 2;

        if (a[mid] <= key) {
            left = mid + 1;
        } else {
            right = mid;
        }
    }

    return left;
}

// Хеш-таблица: разрешение коллизий методом цепочек.
class HashTable {
private:
    vector<list<pair<string, int>>> table;

    // Вычисление номера корзины для ключа.
    size_t getIndex(const string& key) const {
        return hash<string>{}(key) % table.size();
    }

public:
    explicit HashTable(size_t capacity) : table(capacity) {}

    // Добавление или обновление пары ключ-значение.
    void insert(const string& key, int value) {
        size_t index = getIndex(key);

        for (auto& item : table[index]) {
            if (item.first == key) {
                item.second = value;
                return;
            }
        }

        table[index].push_back({key, value});
    }

    // Поиск значения по ключу.
    bool find(const string& key, int& value) const {
        size_t index = getIndex(key);

        for (const auto& item : table[index]) {
            if (item.first == key) {
                value = item.second;
                return true;
            }
        }

        return false;
    }

    // Удаление элемента по ключу.
    bool remove(const string& key) {
        size_t index = getIndex(key);

        for (auto it = table[index].begin(); it != table[index].end(); ++it) {
            if (it->first == key) {
                table[index].erase(it);
                return true;
            }
        }

        return false;
    }
};

int main() {
    cout << "Лабораторная работа №8. Двоичный поиск и хеш-таблицы\n";
    cout << "Вариант 5\n\n";

    // Проверка двоичного поиска границ на массиве с повторениями.
    vector<int> example = {1, 2, 2, 2, 4, 5, 7};

    int key = 2;
    int leftPosition = lowerBound(example, key);
    int right = upperBound(example, key) - 1;

    cout << "Проверка двоичного поиска:\n";
    cout << "Массив: ";
    for (int x : example) {
        cout << x << ' ';
    }
    cout << "\nЭлемент " << key << ": левая позиция = " << leftPosition
         << ", правая позиция = " << right << "\n\n";

    // Проверка обязательных операций хеш-таблицы.
    HashTable testTable(10);
    testTable.insert("apple", 10);
    testTable.insert("banana", 20);

    int foundValue = 0;
    cout << "Проверка хеш-таблицы:\n";
    if (testTable.find("apple", foundValue)) {
        cout << "Найден apple: " << foundValue << '\n';
    }

    testTable.remove("apple");
    cout << "После удаления apple: "
         << (testTable.find("apple", foundValue) ? "найден" : "не найден")
         << "\n\n";

    // Сравнение времени поиска для N = 1000, 10000, 100000.
    const int searchCount = 100000;

    cout << "Сравнение времени поиска (" << searchCount
         << " повторений):\n";
    cout << "N | Бинарный поиск, мс | Хеш-таблица, мс\n";

    for (int n : {1000, 10000, 100000}) {
        vector<int> numbers(n);
        HashTable hashTable(static_cast<size_t>(n * 2));

        // Заполняем обе структуры одинаковыми логическими ключами.
        for (int i = 0; i < n; ++i) {
            numbers[i] = i;
            hashTable.insert(to_string(i), i);
        }

        // Подготавливаем одинаковые поисковые значения до замера времени.
        vector<int> intQueries(searchCount);
        vector<string> stringQueries(searchCount);

        for (int i = 0; i < searchCount; ++i) {
            int query = (i * 37) % n;
            intQueries[i] = query;
            stringQueries[i] = to_string(query);
        }

        // Измеряем только поиск в отсортированном массиве.
        volatile int binaryResult = 0;
        auto binaryStart = high_resolution_clock::now();

        for (int query : intQueries) {
            int position = lowerBound(numbers, query);

            if (position < n && numbers[position] == query) {
                binaryResult += numbers[position];
            }
        }

        auto binaryFinish = high_resolution_clock::now();

        // Измеряем только поиск в хеш-таблице.
        volatile int hashResult = 0;
        auto hashStart = high_resolution_clock::now();

        for (const string& query : stringQueries) {
            int value = 0;

            if (hashTable.find(query, value)) {
                hashResult += value;
            }
        }

        auto hashFinish = high_resolution_clock::now();

        double binaryTime =
            duration<double, milli>(binaryFinish - binaryStart).count();
        double hashTime =
            duration<double, milli>(hashFinish - hashStart).count();

        cout << n << " | " << fixed << setprecision(3) << binaryTime
             << " | " << hashTime << '\n';
    }

    return 0;
}
