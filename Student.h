#ifndef STUDENT_H
#define STUDENT_H

#include <string>

// Structure to store student information
struct Student {
    std::string name;
    int id;
    float marks[3];
    float total;
    float average;
};

// Function declarations
void inputStudents(Student* students, int n);
void calculateGrades(Student* students, int n);
void displayStudents(const Student* students, int n);
void findHighestScorer(const Student* students, int n);
void saveToFile(const Student* students, int n, const std::string& filename);
void loadAndDisplayFromFile(Student* students, int n, const std::string& filename);

#endif