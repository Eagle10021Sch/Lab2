#include "Student.h"
#include <iostream>

// Track file write progress as requested via volatile keyword
volatile bool fileWriteInProgress = false;

int main() {
    std::unique_ptr<Student[]> students(nullptr);
    int count = 0;
    
    while (true) {
        std::cout << "\n===== Enhanced Student Management System =====\n";
        std::cout << "1. Add student records\n";
        std::cout << "2. Display all students\n";
        std::cout << "3. Search student by ID\n";
        std::cout << "4. Edit student record\n";
        std::cout << "5. Delete student record\n";
        std::cout << "6. Find highest scoring student\n";
        std::cout << "7. Display class summary\n";
        std::cout << "8. Sort students\n";
        std::cout << "9. Save records to file\n";
        std::cout << "10. Load records from file\n";
        std::cout << "11. Exit\n";
        
        int choice = getValidatedInt("Enter your choice: ", 1, 11);
        
        switch (choice) {
            case 1:
                addStudents(students, count);
                break;
            case 2:
                displayStudents(students, count);
                break;
            case 3:
                searchStudentById(students, count);
                break;
            case 4:
                editStudent(students, count);
                break;
            case 5:
                deleteStudent(students, count);
                break;
            case 6:
                findHighestScorer(students, count);
                break;
            case 7:
                displayClassSummary(students, count);
                break;
            case 8:
                sortStudents(students, count);
                break;
            case 9:
                saveToFile(students, count);
                break;
            case 10:
                loadFromFile(students, count);
                break;
            case 11:
                // No raw delete needed, smart pointer handles cleanup. Program exits cleanly.
                return 0;
            default:
                std::cout << "Invalid choice. Please try again.\n";
                break;
        }
    }
}