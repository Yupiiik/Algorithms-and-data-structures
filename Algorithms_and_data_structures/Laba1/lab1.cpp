// Лабораторная работа №1. Композиция и агрегация классов. Статические члены
// Продвинутый уровень, вариант 5:
// Система управления курсами: TrainingCenter (одиночка)
//   + Course (агрегация) + Student (агрегация в Course)
 
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <clocale>
 
using namespace std;
 
// ============================================================
// Класс Student - "часть" в агрегации с Course.
// Студент существует независимо от курса и может быть
// записан сразу на несколько курсов.
// ============================================================
class Student {
private:
    string name;   // ФИО студента
    string group;  // номер группы
 
    // Статическое поле: общий счётчик живых объектов Student
    static int studentCount;
 
public:
    Student(const string& n, const string& g) : name(n), group(g) {
        studentCount++;
        cout << "  [+] Создан студент: " << name << " (" << group << ")" << endl;
    }
 
    // Копирование запрещено: иначе счётчик студентов был бы неверным
    // (копия не увеличивала бы studentCount, а деструктор уменьшал бы).
    Student(const Student&) = delete;
    Student& operator=(const Student&) = delete;
 
    ~Student() {
        studentCount--;
        cout << "  [-] Удалён студент: " << name << endl;
    }
 
    const string& getName() const { return name; }
    const string& getGroup() const { return group; }
 
    void print() const {
        cout << "      * " << name << ", группа " << group << endl;
    }
 
    // Статический метод для работы со статическим полем
    static int getStudentCount() { return studentCount; }
};
 
// Определение статического поля (вне класса)
int Student::studentCount = 0;
 
// ============================================================
// Класс Course - "целое" для Student (агрегация)
// и "часть" для TrainingCenter (агрегация).
// Курс хранит только УКАЗАТЕЛИ на студентов и не удаляет их.
// ============================================================
class Course {
private:
    string title;                 // название курса
    int maxStudents;              // максимальное число студентов
    vector<Student*> students;    // агрегация: студенты живут независимо
 
    // Статическое поле: общий счётчик живых объектов Course
    static int courseCount;
 
public:
    Course(const string& t, int maxS) : title(t), maxStudents(maxS > 0 ? maxS : 1) {
        courseCount++;
        cout << "  [+] Создан курс: \"" << title << "\" (мест: " << maxStudents << ")" << endl;
    }
 
    Course(const Course&) = delete;
    Course& operator=(const Course&) = delete;
 
    ~Course() {
        courseCount--;
        cout << "  [-] Удалён курс: \"" << title << "\"" << endl;
        // Студенты НЕ удаляются - курс ими не владеет
    }
 
    const string& getTitle() const { return title; }
    int getStudentsCount() const { return static_cast<int>(students.size()); }
 
    // Добавление студента. Возвращает true при успехе.
    bool addStudent(Student* student) {
        // Проверка на nullptr (обязательна при агрегации)
        if (student == nullptr) {
            cout << "  [!] Ошибка: передан нулевой указатель на студента" << endl;
            return false;
        }
        // Проверка на дубликат
        if (find(students.begin(), students.end(), student) != students.end()) {
            cout << "  [!] Студент " << student->getName()
                 << " уже записан на курс \"" << title << "\"" << endl;
            return false;
        }
        // Проверка на переполнение курса
        if (static_cast<int>(students.size()) >= maxStudents) {
            cout << "  [!] Курс \"" << title << "\" заполнен, студент "
                 << student->getName() << " не добавлен" << endl;
            return false;
        }
        students.push_back(student);
        cout << "  Студент " << student->getName()
             << " записан на курс \"" << title << "\"" << endl;
        return true;
    }
 
    // Удаление студента с курса (сам объект Student НЕ уничтожается)
    bool removeStudent(Student* student) {
        if (student == nullptr) {
            cout << "  [!] Ошибка: передан нулевой указатель на студента" << endl;
            return false;
        }
        auto it = find(students.begin(), students.end(), student);
        if (it == students.end()) {
            cout << "  [!] Студента " << student->getName()
                 << " нет на курсе \"" << title << "\"" << endl;
            return false;
        }
        students.erase(it);
        cout << "  Студент " << student->getName()
             << " отчислен с курса \"" << title << "\"" << endl;
        return true;
    }
 
    void print() const {
        cout << "    Курс \"" << title << "\" [" << students.size()
             << "/" << maxStudents << "]:" << endl;
        if (students.empty()) {
            cout << "      (студентов нет)" << endl;
            return;
        }
        for (const Student* s : students) {
            s->print();
        }
    }
 
    // Статический метод для работы со статическим полем
    static int getCourseCount() { return courseCount; }
};
 
int Course::courseCount = 0;
 
// ============================================================
// Класс TrainingCenter - паттерн "Одиночка" (Singleton).
// Агрегирует курсы (хранит указатели, но не удаляет их).
// ============================================================
class TrainingCenter {
private:
    string name;
    vector<Course*> courses;  // агрегация: курсы живут независимо
 
    // Единственный экземпляр класса
    static TrainingCenter* instance;
 
    // Приватный конструктор - создать объект извне нельзя
    TrainingCenter(const string& n) : name(n) {
        cout << "[+] Открыт учебный центр \"" << name << "\"" << endl;
    }
 
    ~TrainingCenter() {
        cout << "[-] Закрыт учебный центр \"" << name << "\"" << endl;
        // Курсы НЕ удаляются
    }
 
public:
    // Запрет копирования и присваивания
    TrainingCenter(const TrainingCenter&) = delete;
    TrainingCenter& operator=(const TrainingCenter&) = delete;
 
    // Получение единственного экземпляра (ленивая инициализация)
    static TrainingCenter* getInstance(const string& n = "Учебный центр") {
        if (instance == nullptr) {
            instance = new TrainingCenter(n);
        }
        return instance;
    }
 
    // Явное уничтожение одиночки (чтобы не было утечки памяти)
    static void destroyInstance() {
        delete instance;
        instance = nullptr;
    }
 
    bool addCourse(Course* course) {
        if (course == nullptr) {
            cout << "  [!] Ошибка: передан нулевой указатель на курс" << endl;
            return false;
        }
        if (find(courses.begin(), courses.end(), course) != courses.end()) {
            cout << "  [!] Курс \"" << course->getTitle()
                 << "\" уже есть в центре" << endl;
            return false;
        }
        courses.push_back(course);
        cout << "  Курс \"" << course->getTitle()
             << "\" добавлен в центр \"" << name << "\"" << endl;
        return true;
    }
 
    bool removeCourse(Course* course) {
        if (course == nullptr) {
            cout << "  [!] Ошибка: передан нулевой указатель на курс" << endl;
            return false;
        }
        auto it = find(courses.begin(), courses.end(), course);
        if (it == courses.end()) {
            cout << "  [!] Курса \"" << course->getTitle()
                 << "\" нет в центре" << endl;
            return false;
        }
        courses.erase(it);
        cout << "  Курс \"" << course->getTitle()
             << "\" исключён из центра \"" << name << "\"" << endl;
        return true;
    }
 
    void print() const {
        cout << "Учебный центр \"" << name << "\" (курсов в центре: "
             << courses.size() << "):" << endl;
        if (courses.empty()) {
            cout << "    (курсов нет)" << endl;
            return;
        }
        for (const Course* c : courses) {
            c->print();
        }
    }
};
 
TrainingCenter* TrainingCenter::instance = nullptr;
 
// ============================================================
// Вспомогательная функция для вывода статистики
// ============================================================
void printStatistics() {
    cout << ">>> Статистика: курсов существует = " << Course::getCourseCount()
         << ", студентов существует = " << Student::getStudentCount() << endl;
}
 
// ============================================================
// Демонстрация
// ============================================================
int main() {
    setlocale(LC_ALL, "ru");
 
    cout << "=== 1. Паттерн Одиночка ===" << endl;
    TrainingCenter* center1 = TrainingCenter::getInstance("IT-Академия");
    TrainingCenter* center2 = TrainingCenter::getInstance("Другое имя");
    cout << "center1 == center2: " << (center1 == center2 ? "true" : "false") << endl;
    // TrainingCenter c;                        // ОШИБКА: конструктор приватный
    // TrainingCenter c3(*center1);             // ОШИБКА: копирование удалено
 
    cout << "\n=== 2. Статические счётчики до создания объектов ===" << endl;
    printStatistics();
 
    cout << "\n=== 3. Создание студентов и курсов (независимо от центра) ===" << endl;
    Student s1("Иванов Иван", "ИС-21");
    Student s2("Петрова Мария", "ИС-21");
    Student s3("Сидоров Пётр", "ИС-22");
    Student s4("Кузнецова Анна", "ИС-22");
 
    Course cpp("Программирование на C++", 3);
    Course db("Базы данных", 2);
    printStatistics();
 
    cout << "\n=== 4. Агрегация: курсы -> центр ===" << endl;
    center1->addCourse(&cpp);
    center1->addCourse(&db);
 
    cout << "\n=== 5. Агрегация: студенты -> курсы ===" << endl;
    cpp.addStudent(&s1);
    cpp.addStudent(&s2);
    cpp.addStudent(&s3);
    db.addStudent(&s1);   // один студент может быть на нескольких курсах
    db.addStudent(&s4);
 
    cout << "\n=== 6. Граничные случаи ===" << endl;
    cpp.addStudent(&s4);          // курс заполнен (3 из 3)
    cpp.addStudent(&s1);          // дубликат
    cpp.addStudent(nullptr);      // нулевой указатель
    db.removeStudent(&s3);        // студента нет на курсе
    center1->addCourse(nullptr);  // нулевой указатель
    center1->addCourse(&cpp);     // дубликат курса
 
    cout << "\n=== 7. Текущее состояние центра ===" << endl;
    center1->print();
 
    cout << "\n=== 8. Отчисление студента с курса ===" << endl;
    cpp.removeStudent(&s2);
    cout << "Студент s2 по-прежнему существует:" << endl;
    s2.print();
    printStatistics();
 
    cout << "\n=== 9. Уничтожение целого: временный курс удаляется, студент остаётся ===" << endl;
    {
        Course temp("Временный курс", 5);
        center1->addCourse(&temp);
        temp.addStudent(&s2);
        printStatistics();
        center1->removeCourse(&temp);
    } // temp уничтожается, студент s2 остаётся
    cout << "Курс уничтожен, но студент s2 жив:" << endl;
    s2.print();
    printStatistics();
 
    cout << "\n=== 10. Уничтожение центра: курсы и студенты остаются ===" << endl;
    TrainingCenter::destroyInstance();
    center1 = center2 = nullptr;  // обнуляем "висячие" указатели
    printStatistics();
    cpp.print();
    db.print();
 
    cout << "\n=== 11. Выход из main: объекты удаляются автоматически ===" << endl;
    return 0;
}