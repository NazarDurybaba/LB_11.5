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

// ������� ��� ������ ����� � ����
void writeToFile(const string& filename, const vector<Record>& records) {
    ofstream file(filename, ios::binary);
    if (!file) {
        cerr << "�� ������� ������� ���� ��� ������.\n";
        return;
    }
    for (const auto& record : records) {
        file.write(reinterpret_cast<const char*>(&record), sizeof(Record));
    }
    file.close();
}

// ������� ��� ���������� ����� � �����
vector<Record> readFromFile(const string& filename) {
    ifstream file(filename, ios::binary);
    if (!file) {
        cerr << "�� ������� ������� ���� ��� �������.\n";
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

// ������� ��� ��������� ������ �� �����
void appendToFile(const string& filename, const Record& record) {
    ofstream file(filename, ios::binary | ios::app);
    if (!file) {
        cerr << "�� ������� ������� ���� ��� ���������.\n";
        return;
    }
    file.write(reinterpret_cast<const char*>(&record), sizeof(Record));
    file.close();
}

// ������� ��� ����� ������ � ����
void replaceInFile(const string& filename, const Record& record, int index) {
    fstream file(filename, ios::binary | ios::in | ios::out);
    if (!file) {
        cerr << "�� ������� ������� ���� ��� �����.\n";
        return;
    }
    file.seekp(index * sizeof(Record));
    file.write(reinterpret_cast<const char*>(&record), sizeof(Record));
    file.close();
}

// ������� ��� ��������� ����� � �����
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

    // ³�������� ���� ��� ������, �������� ���� ���� �������� �� ������ ������� filteredRecords
    ofstream file(filename, ios::binary);
    if (!file) {
        cerr << "�� ������� ������� ���� ��� ������.\n";
        return;
    }
    for (const auto& record : filteredRecords) {
        file.write(reinterpret_cast<const char*>(&record), sizeof(Record));
    }
    file.close();
}

// ������� ��� ����������� ������ �������� � ��������� ������� ����� �� ��������� ���� �� ����
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

// ������� ��� ����������� ���������� �� ���� � ��������� ������� ��������
void findFacultiesWithTopStudents(const vector<Record>& records) {
    map<string, int> facultyCounts;
    map<string, int> groupCounts;

    for (const auto& record : records) {
        const auto& facultyName = record.institute.name + " - " + record.student.surname;
        const auto& groupName = record.institute.name + " - " + record.student.group;

        // ��������, �� ������� � ��������� (������� ��� 5)
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

    cout << "���������� � ��������� ������� ��������:" << endl;
    for (const auto& pair : facultyCounts) {
        if (pair.second == maxFacultyCount) {
            cout << pair.first << ": " << pair.second << " ��������" << endl;
        }
    }

    cout << "����� � ��������� ������� ��������:" << endl;
    for (const auto& pair : groupCounts) {
        if (pair.second == maxGroupCount) {
            cout << pair.first << ": " << pair.second << " ��������" << endl;
        }
    }
}

int main() {
    SetConsoleOutputCP(1251);
    vector<Record> records; 

    while (true) {
        cout << "1. ������ ��� �� �����\n";
        cout << "2. ������� ��� � ����\n";
        cout << "3. �������� ��� � �����\n";
        cout << "4. ������ ���������� �� ����� � ��������� ������� ��������\n";
        cout << "5. ������ ������� �������� � ��������� ������� ����� �� ��������� ���� �� ����\n";
        cout << "6. �����\n";
        cout << "������� �����: ";

        int choice;
        cin >> choice;

        switch (choice) {
        case 1: {
            Record newRecord;
            cout << "������ ��� ��� ������ ������:" << endl;
            cout << "��'� ���������: ";
            cin >> newRecord.institute.name;
            cout << "��'� ����������: ";
            cin >> newRecord.institute.faculties[0].name; // ������ ���
            cout << "������� ��������: ";
            cin >> newRecord.student.surname;
            cout << "����: ";
            cin >> newRecord.student.course;
            cout << "�����: ";
            cin >> newRecord.student.group;
            cout << "������� ���: ";
            cin >> newRecord.student.average_grade;
            records.push_back(newRecord);
            appendToFile("data.bin", newRecord);
            break;
        }
        case 2: {
            Record newData;
            cout << "������ ��� ��� �����:" << endl;
            cout << "��'� ���������: ";
            cin >> newData.institute.name;
            cout << "��'� ����������: ";
            cin >> newData.institute.faculties[0].name; // ������ ���
            cout << "������� ��������: ";
            cin >> newData.student.surname;
            cout << "����: ";
            cin >> newData.student.course;
            cout << "�����: ";
            cin >> newData.student.group;
            cout << "������� ���: ";
            cin >> newData.student.average_grade;
            records.push_back(newData);
            replaceInFile("data.bin", newData, 0);
            break;
        }
        case 3: {
            string instituteName, facultyName, surname;
            cout << "������ ��'� ���������: ";
            cin >> instituteName;
            cout << "������ ��'� ���������� (��� ������� �������, ���� ������ �������� ��������): ";
            cin >> facultyName;
            cout << "������ ������� �������� (��� ������� �������, ���� ������ �������� ���������): ";
            cin >> surname;
            deleteFromFile("data.bin", instituteName, facultyName, surname);
            break;
        }
        case 4: {
            findFacultiesWithTopStudents(records); // ����� ���������� �� ���� � ��������� ������� ��������
            break;
        }
        case 5: {
            int course;
            string group;
            double minGrade = DBL_MAX; 
            string minSurname;
            cout << "������ ����: ";
            cin >> course;
            cout << "������ �����: ";
            cin >> group;
            vector<pair<string, double>> students = findStudentsWithMinGrade(records, course, group, minGrade);
            if (!students.empty()) {
                cout << "�������(�) � ��������� ������� ����� �� ���� " << course << " �� ���� " << group << ":" << endl;
                for (const auto& student : students) {
                    cout << student.first << " (" << student.second << ")" << endl;
                }
            }
            else {
                cout << "�������� � ������ ����������� �� ��������." << endl;
            }
            break;
        }
        case 6:
            return 0; 
        default:
            cout << "������������ ����. ��������� �����.\n";
        }
    }

    return 0;
}
