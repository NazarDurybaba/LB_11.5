#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <Windows.h>
#include <limits> 
#include <map> 

using namespace std;

struct Student {
    int course;
    string group;
    string surname;
    double average_grade;
};

struct Faculty {
    string name;
    vector<Student> students;
};

struct Institute {
    string name;
    vector<Faculty> faculties;
};

struct Record {
    Institute institute;
    Student student;
};

// Функція для запису даних у файл
void writeToFile(const string& filename, const vector<Record>& records) {
    ofstream file(filename, ios::binary);
    if (!file) {
        cerr << "Не вдалося відкрити файл для запису.\n";
        return;
    }
    for (const auto& record : records) {
        file.write(reinterpret_cast<const char*>(&record), sizeof(Record));
    }
    file.close();
}

// Функція для зчитування даних з файлу
vector<Record> readFromFile(const string& filename) {
    ifstream file(filename, ios::binary);
    if (!file) {
        cerr << "Не вдалося відкрити файл для читання.\n";
        return {};
    }
    vector<Record> records;
    Record record;
    while (file.read(reinterpret_cast<char*>(&record), sizeof(Record))) {
        records.push_back(record);
    }
    file.close();
    return records;
}

// Функція для додавання запису до файлу
void appendToFile(const string& filename, const Record& record) {
    ofstream file(filename, ios::binary | ios::app);
    if (!file) {
        cerr << "Не вдалося відкрити файл для додавання.\n";
        return;
    }
    file.write(reinterpret_cast<const char*>(&record), sizeof(Record));
    file.close();
}

// Функція для заміни запису у файлі
void replaceInFile(const string& filename, const Record& record, int index) {
    fstream file(filename, ios::binary | ios::in | ios::out);
    if (!file) {
        cerr << "Не вдалося відкрити файл для заміни.\n";
        return;
    }
    file.seekp(index * sizeof(Record));
    file.write(reinterpret_cast<const char*>(&record), sizeof(Record));
    file.close();
}

// Функція для видалення даних з файлу
void deleteFromFile(const string& filename, const string& instituteName, const string& facultyName, const string& surname) {
    vector<Record> records = readFromFile(filename);
    vector<Record> filteredRecords;

    for (const auto& record : records) {
        if (record.institute.name == instituteName) {
            if (facultyName.empty()) {
                continue;
            }
            else if (record.student.surname.empty()) {
                if (record.student.group.empty()) {
                    continue;
                }
            }
            else if (record.student.surname == surname) {
                continue;
            }
        }
        filteredRecords.push_back(record);
    }

    // Відкриваємо файл для запису, замінюємо його вміст відповідно до нового вектора filteredRecords
    ofstream file(filename, ios::binary);
    if (!file) {
        cerr << "Не вдалося відкрити файл для запису.\n";
        return;
    }
    for (const auto& record : filteredRecords) {
        file.write(reinterpret_cast<const char*>(&record), sizeof(Record));
    }
    file.close();
}

// Функція для знаходження прізвищ студентів з мінімальним середнім балом на вказаному курсі та групі
vector<pair<string, double>> findStudentsWithMinGrade(const vector<Record>& records, int course, const string& group, double& minGrade) {
    vector<pair<string, double>> result;
    for (const auto& record : records) {
        const auto& student = record.student;
        if (student.course == course && student.group == group && student.average_grade < minGrade) {
            result.clear();
            result.push_back(make_pair(student.surname, student.average_grade));
            minGrade = student.average_grade;
        }
        else if (student.course == course && student.group == group && student.average_grade == minGrade) {
            result.push_back(make_pair(student.surname, student.average_grade));
        }
    }
    return result;
}

// Функція для знаходження факультетів та груп з найбільшою кількістю відмінників
void findFacultiesWithTopStudents(const vector<Record>& records) {
    map<string, int> facultyCounts;
    map<string, int> groupCounts;

    for (const auto& record : records) {
        const auto& facultyName = record.institute.name + " - " + record.student.surname;
        const auto& groupName = record.institute.name + " - " + record.student.group;

        // Перевірка, чи студент є відмінником (середній бал 5)
        if (record.student.average_grade == 5) {
            facultyCounts[facultyName]++;
            groupCounts[groupName]++;
        }
    }

    int maxFacultyCount = 0;
    for (const auto& pair : facultyCounts) {
        maxFacultyCount = max(maxFacultyCount, pair.second);
    }

    int maxGroupCount = 0;
    for (const auto& pair : groupCounts) {
        maxGroupCount = max(maxGroupCount, pair.second);
    }

    cout << "Факультети з найбільшою кількістю відмінників:" << endl;
    for (const auto& pair : facultyCounts) {
        if (pair.second == maxFacultyCount) {
            cout << pair.first << ": " << pair.second << " відмінників" << endl;
        }
    }

    cout << "Групи з найбільшою кількістю відмінників:" << endl;
    for (const auto& pair : groupCounts) {
        if (pair.second == maxGroupCount) {
            cout << pair.first << ": " << pair.second << " відмінників" << endl;
        }
    }
}

int main() {
    SetConsoleOutputCP(1251);
    vector<Record> records; 

    while (true) {
        cout << "1. Додати дані до файлу\n";
        cout << "2. Замінити дані у файлі\n";
        cout << "3. Видалити дані з файлу\n";
        cout << "4. Знайти факультети та групи з найбільшою кількістю відмінників\n";
        cout << "5. Знайти прізвища студентів з мінімальним середнім балом на вказаному курсі та групі\n";
        cout << "6. Вийти\n";
        cout << "Виберіть опцію: ";

        int choice;
        cin >> choice;

        switch (choice) {
        case 1: {
            Record newRecord;
            cout << "Введіть дані для нового запису:" << endl;
            cout << "Ім'я інституту: ";
            cin >> newRecord.institute.name;
            cout << "Ім'я факультету: ";
            cin >> newRecord.institute.faculties[0].name; // Змінено тут
            cout << "Прізвище студента: ";
            cin >> newRecord.student.surname;
            cout << "Курс: ";
            cin >> newRecord.student.course;
            cout << "Група: ";
            cin >> newRecord.student.group;
            cout << "Середній бал: ";
            cin >> newRecord.student.average_grade;
            records.push_back(newRecord);
            appendToFile("data.bin", newRecord);
            break;
        }
        case 2: {
            Record newData;
            cout << "Введіть дані для заміни:" << endl;
            cout << "Ім'я інституту: ";
            cin >> newData.institute.name;
            cout << "Ім'я факультету: ";
            cin >> newData.institute.faculties[0].name; // Змінено тут
            cout << "Прізвище студента: ";
            cin >> newData.student.surname;
            cout << "Курс: ";
            cin >> newData.student.course;
            cout << "Група: ";
            cin >> newData.student.group;
            cout << "Середній бал: ";
            cin >> newData.student.average_grade;
            records.push_back(newData);
            replaceInFile("data.bin", newData, 0);
            break;
        }
        case 3: {
            string instituteName, facultyName, surname;
            cout << "Введіть ім'я інституту: ";
            cin >> instituteName;
            cout << "Введіть ім'я факультету (або залиште порожнім, якщо хочете видалити інститут): ";
            cin >> facultyName;
            cout << "Введіть прізвище студента (або залиште порожнім, якщо хочете видалити факультет): ";
            cin >> surname;
            deleteFromFile("data.bin", instituteName, facultyName, surname);
            break;
        }
        case 4: {
            findFacultiesWithTopStudents(records); // Пошук факультетів та груп з найбільшою кількістю відмінників
            break;
        }
        case 5: {
            int course;
            string group;
            double minGrade = DBL_MAX; 
            string minSurname;
            cout << "Введіть курс: ";
            cin >> course;
            cout << "Введіть групу: ";
            cin >> group;
            vector<pair<string, double>> students = findStudentsWithMinGrade(records, course, group, minGrade);
            if (!students.empty()) {
                cout << "Студент(и) з мінімальним середнім балом на курсі " << course << " та групі " << group << ":" << endl;
                for (const auto& student : students) {
                    cout << student.first << " (" << student.second << ")" << endl;
                }
            }
            else {
                cout << "Студентів з такими параметрами не знайдено." << endl;
            }
            break;
        }
        case 6:
            return 0; 
        default:
            cout << "Неправильний вибір. Спробуйте знову.\n";
        }
    }

    return 0;
}
