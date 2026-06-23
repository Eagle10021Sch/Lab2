#ifndef STUDENT_H
#define STUDENT_H

#include <string>
#include <memory>

// Unscoped enum for pass/fail decisions
enum Status { FAIL, PASS };

// Scoped enum for letter-grade representation
enum class Grade { A, B, C, D, F };

// Constants
constexpr int SUBJECT_COUNT = 3;
constexpr float PASS_THRESHOLD = 50.0f;
constexpr int MAX_STUDENTS = 1000;

// Structure to store student information
struct Student {
    int id;
    std::string name;
    float marks[SUBJECT_COUNT];
    float total;
    float average;
    Grade grade;
    Status status;
};

// Helper function prototypes for Enums
std::string statusToString(Status status);
std::string gradeToString(Grade grade);

// Function prototypes for Menu Options
void addStudents(std::unique_ptr<Student[]>& students, int& count);
void displayStudents(const std::unique_ptr<Student[]>& students, int count);
void searchStudentById(const std::unique_ptr<Student[]>& students, int count);
void editStudent(std::unique_ptr<Student[]>& students, int count);
void deleteStudent(std::unique_ptr<Student[]>& students, int& count);
void findHighestScorer(const std::unique_ptr<Student[]>& students, int count);
void displayClassSummary(const std::unique_ptr<Student[]>& students, int count);
void sortStudents(std::unique_ptr<Student[]>& students, int count);
void saveToFile(const std::unique_ptr<Student[]>& students, int count);
void loadFromFile(std::unique_ptr<Student[]>& students, int& count);

// Validation helper functions
int getValidatedInt(const std::string& prompt, int min, int max);
float getValidatedMark(const std::string& prompt);
std::string getValidatedName(const std::string& prompt);
bool isDuplicateId(const std::unique_ptr<Student[]>& students, int count, int id);

// Record processing rules helper functions
float calculateTotal(const Student& student);
float calculateAverage(float total);
Status determineStatus(float average);
Grade determineGrade(float average);
void recalculateStudent(Student& student);

// Additional utility helper functions
std::string trim(const std::string& text);
bool isBlank(const std::string& text);
int findStudentIndexById(const std::unique_ptr<Student[]>& students, int count, int id);
void copyStudent(Student& destination, const Student& source);

#endif