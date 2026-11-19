// Лабораторная работа №6. Рекурсивные алгоритмы. Простые сортировки
// Продвинутый уровень, вариант 5
// Рекурсивные задачи: факториал, Фибоначчи, сумма массива, Ханойские башни (4 диска).
// Сортировки: пузырьковая, выбором и вставками; для пузырьковой и выбором
// считаются сравнения и обмены (дополнительное задание варианта 5).

#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <clocale>
using namespace std;

// 1. Рекурсивный факториал
long long factorial(int n) {
    if (n <= 1) return 1;              // базовый случай
    return n * factorial(n - 1);       // шаг рекурсии
}

// 2. Рекурсивное число Фибоначчи (для небольших n)
long long fibonacci(int n) {
    if (n <= 1) return n;              // базовые случаи
    return fibonacci(n - 1) + fibonacci(n - 2);
}

// 3. Рекурсивная сумма элементов массива
int sumArray(const vector<int>& arr, int index) {
    if (index == static_cast<int>(arr.size())) return 0; // массив закончился
    return arr[index] + sumArray(arr, index + 1);
}

// 4. Ханойские башни для четырёх дисков
void hanoi(int n, char from, char to, char auxiliary, int& moves) {
    if (n == 1) {
        ++moves;
        cout << "Ход " << moves << ": диск 1 с " << from << " на " << to << '\n';
        return;
    }
    hanoi(n - 1, from, auxiliary, to, moves);
    ++moves;
    cout << "Ход " << moves << ": диск " << n << " с " << from << " на " << to << '\n';
    hanoi(n - 1, auxiliary, to, from, moves);
}

struct SortStats {
    long long comparisons = 0;
    long long swaps = 0;
};

// Пузырьковая сортировка с подсчётом сравнений и обменов
void bubbleSort(vector<int>& a, SortStats& stats) {
    for (size_t i = 0; i + 1 < a.size(); ++i) {
        bool swapped = false;
        for (size_t j = 0; j + 1 < a.size() - i; ++j) {
            ++stats.comparisons;
            if (a[j] > a[j + 1]) {
                swap(a[j], a[j + 1]);
                ++stats.swaps;
                swapped = true;
            }
        }
        if (!swapped) break; // если обменов не было, массив уже отсортирован
    }
}

// Сортировка выбором с подсчётом сравнений и обменов
void selectionSort(vector<int>& a, SortStats& stats) {
    for (size_t i = 0; i + 1 < a.size(); ++i) {
        size_t minIndex = i;
        for (size_t j = i + 1; j < a.size(); ++j) {
            ++stats.comparisons;
            if (a[j] < a[minIndex]) minIndex = j;
        }
        if (minIndex != i) {
            swap(a[i], a[minIndex]);
            ++stats.swaps;
        }
    }
}

// Сортировка вставками — включена также для выполнения общего перечня требований методички
void insertionSort(vector<int>& a, SortStats& stats) {
    for (size_t i = 1; i < a.size(); ++i) {
        int key = a[i];
        int j = static_cast<int>(i) - 1;
        while (j >= 0) {
            ++stats.comparisons;
            if (a[j] <= key) break;
            a[j + 1] = a[j];
            ++stats.swaps; // учитываем перемещение элемента как операцию перестановки
            --j;
        }
        a[j + 1] = key;
    }
}

void printArray(const vector<int>& a) {
    for (int value : a) cout << value << ' ';
    cout << '\n';
}

int main() {
    setlocale(LC_ALL, "");

    cout << "=== Лабораторная работа №6, вариант 5 ===\n";
    cout << "\n1. Рекурсивный факториал\n";
    cout << "5! = " << factorial(5) << '\n';

    cout << "\n2. Рекурсивные числа Фибоначчи\n";
    cout << "F(10) = " << fibonacci(10) << '\n';

    cout << "\n3. Рекурсивная сумма элементов массива\n";
    vector<int> values = {2, 4, 6, 8, 10};
    cout << "Массив: "; printArray(values);
    cout << "Сумма = " << sumArray(values, 0) << '\n';

    cout << "\n4. Ханойские башни (4 диска)\n";
    int moves = 0;
    hanoi(4, 'A', 'C', 'B', moves);
    cout << "Всего ходов: " << moves << " (ожидается 2^4 - 1 = 15)\n";

    cout << "\n5. Сравнение простых сортировок\n";
    vector<int> original = {7, 3, 9, 1, 5, 2, 8, 4, 6};
    cout << "Исходный массив: "; printArray(original);

    vector<int> bubble = original;
    SortStats bubbleStats;
    bubbleSort(bubble, bubbleStats);
    cout << "Пузырьковая:    "; printArray(bubble);
    cout << "  Сравнений: " << bubbleStats.comparisons << ", обменов: " << bubbleStats.swaps << '\n';

    vector<int> selection = original;
    SortStats selectionStats;
    selectionSort(selection, selectionStats);
    cout << "Выбором:        "; printArray(selection);
    cout << "  Сравнений: " << selectionStats.comparisons << ", обменов: " << selectionStats.swaps << '\n';

    vector<int> insertion = original;
    SortStats insertionStats;
    insertionSort(insertion, insertionStats);
    cout << "Вставками:      "; printArray(insertion);
    cout << "  Сравнений: " << insertionStats.comparisons << ", перемещений: " << insertionStats.swaps << '\n';

    return 0;
}
