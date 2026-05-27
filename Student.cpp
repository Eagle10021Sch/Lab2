#include <iostream>
#include <fstream>
#include "Student.h"

// :: -> Scope resolution operator
// Loop through and prompt the user for each student's details
void inputStudents(Student* students, int n) {
    for (int i = 0; i < n; ++i) {
        std::cout << "Enter data for student " << (i + 1) << "\n";
        std::cout << "Enter ID: ";
        std::cin >> students[i].id;
        std::cin.ignore(); // Clear newline character from the buffer

        std::cout << "Enter Name: ";
        std::getline(std::cin, students[i].name);

        for (int j = 0; j < 3; ++j) {
            std::cout << "Enter marks for subject " << (j + 1) << ": ";
            std::cin >> students[i].marks[j];
        }
    }
}

// Compute the total and average marks for all students
void calculateGrades(Student* students, int n) {
    for (int i = 0; i < n; ++i) {
        students[i].total = 0;
        for (int j = 0; j < 3; ++j) {
            students[i].total += students[i].marks[j];
        }
        students[i].average = students[i].total / 3.0f;
    }
}

// Output student records and performance status to the console 
void displayStudents(const Student* students, int n) {
    for (int i = 0; i < n; ++i) {
        std::cout << "ID: " << students[i].id << ", Name: " << students[i].name << "\n";
        std::cout << "Marks: " << students[i].marks[0] << " " 
                  << students[i].marks[1] << " " 
                  << students[i].marks[2] << "\n";
        std::cout << "Total: " << students[i].total << ", Average: " << students[i].average << "\n";
        std::cout << "Status: " << (students[i].average >= 50.0f ? "PASS" : "FAIL") << "\n";
    }
}

// Compare student scores to find the top performer
void findHighestScorer(const Student* students, int n) {
    if (n <= 0) return;
    
    int highestIndex = 0;
    for (int i = 1; i < n; ++i) {
        if (students[i].total > students[highestIndex].total) {
            highestIndex = i;
        }
    }

    std::cout << "\nHighest Scorer:\n";
    std::cout << "ID: " << students[highestIndex].id << ", Name: " << students[highestIndex].name << "\n";
    std::cout << "Marks: " << students[highestIndex].marks[0] << " " 
              << students[highestIndex].marks[1] << " " 
              << students[highestIndex].marks[2] << "\n";
    std::cout << "Total: " << students[highestIndex].total << ", Average: " << students[highestIndex].average << "\n";
    std::cout << "Status: " << (students[highestIndex].average >= 50.0f ? "PASS" : "FAIL") << "\n";
}

// Write the current student data records out to a text file
void saveToFile(const Student* students, int n, const std::string& filename) {
    std::ofstream outFile(filename);
    if (!outFile) {
        std::cerr << "Error opening file for writing!\n";
        return;
    }

    for (int i = 0; i < n; ++i) {
        outFile << students[i].id << "\n";
        outFile << students[i].name << "\n";
        outFile << students[i].marks[0] << " " << students[i].marks[1] << " " << students[i].marks[2] << "\n";
        outFile << students[i].total << "\n";
        outFile << students[i].average << "\n";
    }
    outFile.close();
    std::cout << "Data saved to " << filename << "\n";
}

// Open the text file, read the stored contents, and print them out
void loadAndDisplayFromFile(const std::string& filename) {
    std::ifstream inFile(filename);
    if (!inFile) {
        std::cerr << "Error opening file for reading!\n";
        return;
    }

    Student temp;
    // Loops automatically and stops once it reaches the end of the file
    while (inFile >> temp.id) {
        inFile.ignore(); // Clear newline

        std::getline(inFile, temp.name);
        inFile >> temp.marks[0] >> temp.marks[1] >> temp.marks[2];
        inFile >> temp.total;
        inFile >> temp.average;
        inFile.ignore(); // Added this line here to clear the newline after the average so that the next iteration can read the next ID correctly

        std::cout << "ID: " << temp.id << ", Name: " << temp.name << "\n";
        std::cout << "Marks: " << temp.marks[0] << " " << temp.marks[1] << " " << temp.marks[2] << "\n";
        std::cout << "Total: " << temp.total << ", Average: " << temp.average << "\n";
        std::cout << "Status: " << (temp.average >= 50.0f ? "PASS" : "FAIL") << "\n";
    }
    inFile.close();
}