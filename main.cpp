#include <iostream>
#include "Student.h"

int main() {
    int n = 0;

    // Get the total number of students from the user
    do {
        std::cout << "Enter number of students (Max 500): ";
        std::cin >> n;
        
        if (n <= 0 || n > 500) {
            std::cout << "Invalid number! Please enter a number between 1 and 500.\n";
        }
    } while (n <= 0 || n > 500); // Loops if input in too small or is larger than the 500 student limit

    // Create an array to hold the student data
    Student* studentArray =  new Student[n]; 

    // Gather input and process the grades
    inputStudents(studentArray, n);
    calculateGrades(studentArray, n);
    
    // Display all processed records
    std::cout << "\n--- Student Records ---\n";
    displayStudents(studentArray, n);

    // Find and print the student with the highest total marks
    findHighestScorer(studentArray, n);

    // Save the student records to a text file
    std::string filename = "students.txt";
    saveToFile(studentArray, n, filename);

    // Load the saved records back and display them
    std::cout << "\nLoading data from file:\n";
    loadAndDisplayFromFile(studentArray, n, filename);

    //Deallocate dynamic heap memory to prevent memory leaks
    delete[] studentArray;

    return 0;
}