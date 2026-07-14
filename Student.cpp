#include "Student.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <limits>
#include <algorithm>
#include <sstream>

namespace {
    // Helper struct to make unique_ptr<Student[]> iterable for range-based loops.
    // This allows us to use standard range-based for loops for display formatting.
    struct ConstStudentRange {
        const Student* begin_ptr;
        const Student* end_ptr;
        const Student* begin() const { return begin_ptr; }
        const Student* end() const { return end_ptr; }
    };
}

// Enum Helpers
// Converts the Status enum (PASS/FAIL) to a printable string
std::string statusToString(Status status) {
    return (status == PASS) ? "PASS" : "FAIL";
}

// Converts the scoped Grade enum class to its letter grade string representation
std::string gradeToString(Grade grade) {
    switch (grade) {
        case Grade::A: return "A";
        case Grade::B: return "B";
        case Grade::C: return "C";
        case Grade::D: return "D";
        case Grade::F: return "F";
    }
    return "Unknown";
}

// Utility functions
// Removes leading and trailing whitespace from a string
std::string trim(const std::string& text) {
    auto start = text.find_first_not_of(" \t\n\r");
    if (start == std::string::npos) return ""; // String is empty or just whitespace
    auto end = text.find_last_not_of(" \t\n\r");
    return text.substr(start, end - start + 1);
}

// Checks if a string is empty or contains only whitespace
bool isBlank(const std::string& text) {
    return trim(text).empty();
}

// Searches for a student by ID and returns their index in the dynamic array, or -1 if not found
int findStudentIndexById(const std::unique_ptr<Student[]>& students, int count, int id) {
    for (int i = 0; i < count; ++i) {
        if (students[i].id == id) {
            return i;
        }
    }
    return -1;
}

// Safely copies student data from a source object to a destination object
void copyStudent(Student& destination, const Student& source) {
    destination.id = source.id;
    destination.name = source.name;
    for (int i = 0; i < SUBJECT_COUNT; ++i) {
        destination.marks[i] = source.marks[i];
    }
    destination.total = source.total;
    destination.average = source.average;
    destination.grade = source.grade;
    destination.status = source.status;
}

// Prompts the user for an integer and strictly rejects any non-integer input (like floats or text)
int getValidatedInt(const std::string& prompt, int min, int max) {
    std::string input;
    int value;
    while (true) {
        std::cout << prompt;
        std::getline(std::cin, input);
        input = trim(input);
        
        if (input.empty()) {
            std::cout << "Invalid input. Please enter an integer between " << min << " and " << max << ".\n";
            continue;
        }
        
        // Check if the entire string represents an integer
        bool isNumeric = true;
        for (size_t i = 0; i < input.length(); ++i) {
            if (!std::isdigit(input[i]) && !(i == 0 && input[i] == '-')) {
                isNumeric = false;
                break;
            }
        }
        
        if (isNumeric) {
            try {
                value = std::stoi(input);
                if (value >= min && value <= max) {
                    return value;
                }
            } catch (...) {
                // Handled below if stoi fails due to out-of-range values
            }
        }
        
        std::cout << "Invalid input. Please enter an integer between " << min << " and " << max << ".\n";
    }
}

// Prompts the user for a floating point mark between 0 and 100
float getValidatedMark(const std::string& prompt) {
    float value;
    while (true) {
        std::cout << prompt;
        if (std::cin >> value) {
            // Handles Edge Case: Marks below 0 or above 100
            if (value >= 0.0f && value <= 100.0f) {
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                return value;
            }
        }
        // Handles Edge Case: Non-numeric marks
        std::cout << "Invalid input. Marks must be between 0 and 100.\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

// Prompts the user for a name and ensures it's not empty or just spaces
std::string getValidatedName(const std::string& prompt) {
    std::string name;
    while (true) {
        std::cout << prompt;
        std::getline(std::cin, name);
        // Handles Edge Cases: Empty student name AND Name containing only spaces
        if (!isBlank(name)) {
            return trim(name);
        }
        std::cout << "Name cannot be empty or just whitespace. Please try again.\n";
    }
}

// Checks if an ID already exists in the system to prevent duplicates
bool isDuplicateId(const std::unique_ptr<Student[]>& students, int count, int id) {
    // Handles Edge Case: Duplicate student ID
    return findStudentIndexById(students, count, id) != -1;
}

// Record Processing Rules
// Calculates the total sum of the student's marks
float calculateTotal(const Student& student) {
    float total = 0;
    for (int i = 0; i < SUBJECT_COUNT; ++i) {
        total += student.marks[i];
    }
    return total;
}

// Calculates the average, using static_cast for safe numeric conversion
float calculateAverage(float total) {
    // using static_cast for division
    return total / static_cast<float>(SUBJECT_COUNT);
}

// Determines if a student passed based on the passing threshold constant
Status determineStatus(float average) {
    if (average >= PASS_THRESHOLD) {
        return PASS; // Equal to threshold is marked as PASS
    }
    return FAIL;
}

// Assigns a letter grade using the standard grade logic brackets
Grade determineGrade(float average) {
    if (average >= 80.0f) return Grade::A;
    if (average >= 70.0f) return Grade::B;
    if (average >= 60.0f) return Grade::C;
    if (average >= 50.0f) return Grade::D;
    return Grade::F;
}

// Updates the total, average, status, and grade based on the current marks
void recalculateStudent(Student& student) {
    student.total = calculateTotal(student);
    student.average = calculateAverage(student.total);
    student.status = determineStatus(student.average);
    student.grade = determineGrade(student.average);
}

// Main Functionality
// Adds new students and dynamically grows the smart pointer array
void addStudents(std::unique_ptr<Student[]>& students, int& count) {
    static int totalRecordsCreated = 0; // static requirement to track overall records made
    int maxToAdd = MAX_STUDENTS - count;
    if (maxToAdd <= 0) {
        std::cout << "Maximum safe student count reached. Cannot add more students.\n";
        return;
    }
    
    int numToAdd = getValidatedInt("Enter number of students: ", 1, maxToAdd);
    
    // Allocate new array with extra space for the new students
    std::unique_ptr<Student[]> newArray = std::make_unique<Student[]>(count + numToAdd);
    
    // Copy existing students into the new array
    for (int i = 0; i < count; ++i) { // Index-based loop requirement
        copyStudent(newArray[i], students[i]);
    }
    
    // Prompt for new student records
    for (int i = 0; i < numToAdd; ++i) {
        std::cout << "\nStudent " << (count + i + 1) << "\n";
        int id;
        while (true) {
            id = getValidatedInt("ID: ", 1, std::numeric_limits<int>::max());
            if (isDuplicateId(newArray, count + i, id)) {
                std::cout << "Warning: Duplicate ID. Please enter a different ID.\n";
            } else {
                break;
            }
        }
        
        newArray[count + i].id = id;
        newArray[count + i].name = getValidatedName("Name: ");
        
        for (int j = 0; j < SUBJECT_COUNT; ++j) {
            newArray[count + i].marks[j] = getValidatedMark("Mark for subject " + std::to_string(j + 1) + ": ");
        }
        
        // Finalize student metrics
        recalculateStudent(newArray[count + i]);
        totalRecordsCreated++;
    }
    
    // Replacing existing student data frees old memory automatically via unique_ptr
    students = std::move(newArray); 
    count += numToAdd;
    std::cout << "Successfully added records.\n";
}

// Prints out all students in a neat tabular format
void displayStudents(const std::unique_ptr<Student[]>& students, int count) {
    // Handles Edge Case: Display students when none exist
    if (count == 0) {
        std::cout << "No students available\n";
        return;
    }
    
    std::cout << "\n===== Student Records =====\n";
    // Using iomanip headers for proper column alignment
    std::cout << std::left << std::setw(5) << "ID" 
              << std::setw(15) << "Name" 
              << std::setw(8) << "M1" 
              << std::setw(8) << "M2" 
              << std::setw(8) << "M3" 
              << std::setw(8) << "Total" 
              << std::setw(8) << "Avg" 
              << std::setw(8) << "Grade" 
              << "Status\n";
    std::cout << "------------------------------------------------------------------------------------\n";
    
    // Range-based loop requirement for display formatting
    ConstStudentRange range{students.get(), students.get() + count};
    for (const auto& s : range) {
        std::cout << std::left << std::setw(5) << s.id
                  << std::setw(15) << s.name
                  << std::fixed << std::setprecision(2)
                  << std::setw(8) << s.marks[0]
                  << std::setw(8) << s.marks[1]
                  << std::setw(8) << s.marks[2]
                  << std::setw(8) << s.total
                  << std::setw(8) << s.average
                  << std::setw(8) << gradeToString(s.grade)
                  << statusToString(s.status) << "\n";
    }
}

// Searches for a student by ID using an iterator
void searchStudentById(const std::unique_ptr<Student[]>& students, int count) {
    // Handles Edge Case: Search by ID when none exist
    if (count == 0) {
        std::cout << "Warning: No students available to search.\n";
        return;
    }
    int id = getValidatedInt("Enter student ID to search: ", 1, std::numeric_limits<int>::max());
    
    // Iterator-based loop requirement for searching
    // Auto to deduce the iterator type
    auto begin = students.get();
    auto end = students.get() + count;
    auto it = std::find_if(begin, end, [id](const Student& s) { return s.id == id; });
    
    // Checks if the iterator reached the end without a match
    if (it != end) {
        std::cout << "\nStudent Found:\n";
        std::cout << "ID: " << it->id << "\nName: " << it->name << "\nMarks: "
                  << it->marks[0] << " " << it->marks[1] << " " << it->marks[2] << "\n"
                  << "Total: " << it->total << ", Average: " << std::fixed << std::setprecision(2) << it->average << "\n"
                  << "Grade: " << gradeToString(it->grade) << ", Status: " << statusToString(it->status) << "\n";
    } else {
        std::cout << "Warning: No match exists for ID " << id << ".\n";
    }
}

// Edits a student's data and recalculates grades
void editStudent(std::unique_ptr<Student[]>& students, int count) {
    // Handles Edge Case: Edit student when none exist
    if (count == 0) {
        std::cout << "Warning: No students available to edit.\n";
        return;
    }
    int id = getValidatedInt("Enter student ID to edit: ", 1, std::numeric_limits<int>::max());
    int index = findStudentIndexById(students, count, id);
    
    // Handles Edge Case: Edit non-existent ID
    if (index == -1) {
        std::cout << "Warning: No match exists for ID " << id << ".\n";
        return;
    }
    
    std::cout << "Editing student: " << students[index].name << "\n";
    students[index].name = getValidatedName("Enter new name: ");
    
    for (int j = 0; j < SUBJECT_COUNT; ++j) {
        students[index].marks[j] = getValidatedMark("Enter new mark for subject " + std::to_string(j + 1) + ": ");
    }
    
    // Ensure all average/total values stay consistent after editing
    recalculateStudent(students[index]);
    std::cout << "Student record updated successfully.\n";
}

// Safely shrinks the array and drops a student record
void deleteStudent(std::unique_ptr<Student[]>& students, int& count) {
    // Handles Edge Case: Delete student when none exist
    if (count == 0) {
        std::cout << "Warning: No students available to delete.\n";
        return;
    }
    int id = getValidatedInt("Enter student ID to delete: ", 1, std::numeric_limits<int>::max());
    int index = findStudentIndexById(students, count, id);
    
    // Handles Edge Case: Delete non-existent ID
    if (index == -1) {
        std::cout << "Warning: No match exists for ID " << id << ".\n";
        return;
    }
    
    std::unique_ptr<Student[]> newArray(nullptr);
    // Handles Edge Case: Delete last remaining student (newArray stays nullptr)
    if (count - 1 > 0) {
        // Create a new array that is 1 size smaller
        newArray = std::make_unique<Student[]>(count - 1);
        for (int i = 0, j = 0; i < count; ++i) {
            if (i != index) {
                copyStudent(newArray[j++], students[i]);
            }
        }
    }
    
    // Automatically frees the old array containing the deleted student
    students = std::move(newArray);
    count--;
    std::cout << "Student deleted successfully.\n";
}

// Finds the student with the most marks using iterators
void findHighestScorer(const std::unique_ptr<Student[]>& students, int count) {
    // Handles Edge Case: Find highest scorer with no students
    if (count == 0) {
        std::cout << "Warning: No students available to find highest scorer.\n";
        return;
    }
    
    // Iterator-based traversal requirement
    auto begin = students.get();
    auto end = students.get() + count;
    auto highest = begin;
    
    // Handles Edge Case: Two students share highest average (strictly > means it stays on the first one found)
    for (auto it = begin + 1; it != end; ++it) {
        if (it->total > highest->total) {
            highest = it;
        }
    }
    
    std::cout << "\nHighest Scorer:\n";
    std::cout << "ID: " << highest->id << "\nName: " << highest->name << "\nMarks: "
              << highest->marks[0] << " " << highest->marks[1] << " " << highest->marks[2] << "\n"
              << "Total: " << highest->total << ", Average: " << std::fixed << std::setprecision(2) << highest->average << "\n"
              << "Grade: " << gradeToString(highest->grade) << ", Status: " << statusToString(highest->status) << "\n";
}

// Summarizes class data, ignoring individual students
void displayClassSummary(const std::unique_ptr<Student[]>& students, int count) {
    // Handles Edge Case: Display class summary with no students
    if (count == 0) {
        std::cout << "Safe Message: No students available for class summary.\n";
        return;
    }
    
    float classTotal = 0;
    int passed = 0;
    int failed = 0;
    float highestAvg = 0;
    
    ConstStudentRange range{students.get(), students.get() + count};
    for (const auto& s : range) {
        classTotal += s.average;
        if (s.status == PASS) {
            passed++;
        } else {
            failed++;
        }
        if (s.average > highestAvg) {
            highestAvg = s.average;
        }
    }
    
    float classAverage = classTotal / count;
    float passRate = (static_cast<float>(passed) / count) * 100.0f;
    
    std::cout << "\n===== Class Summary =====\n";
    std::cout << "Total Students : " << count << "\n";
    std::cout << "Class Average  : " << std::fixed << std::setprecision(2) << classAverage << "\n";
    std::cout << "Passed         : " << passed << "\n";
    std::cout << "Failed         : " << failed << "\n";
    std::cout << "Highest Avg    : " << highestAvg << "\n";
    std::cout << "Pass Rate      : " << passRate << "%\n";
}

// Allows the user to sort records alphabetically or by average
void sortStudents(std::unique_ptr<Student[]>& students, int count) {
    if (count == 0) {
        std::cout << "Warning: No students available to sort.\n";
        return;
    }
    std::cout << "Sort Options:\n1. Sort by name in ascending order\n2. Sort by average in descending order\n";
    int choice = getValidatedInt("Enter your choice: ", 1, 2);
    
    auto begin = students.get();
    auto end = students.get() + count;
    
    if (choice == 1) {
        std::sort(begin, end, [](const Student& a, const Student& b) {
            return a.name < b.name; // ascending alphabetical
        });
    } else {
        std::sort(begin, end, [](const Student& a, const Student& b) {
            return a.average > b.average; // descending numeric
        });
    }
    std::cout << "Students sorted successfully.\n";
}

// Extern volatile bool variable, defined in main.cpp to simulate external state changes
extern volatile bool fileWriteInProgress;

// Saves the formatted tabular data to students.txt separated by pipes
void saveToFile(const std::unique_ptr<Student[]>& students, int count) {
    if (count == 0) {
        std::cout << "Warning: No students available to save.\n";
        return;
    }
    
    // Simulate externally changing state
    fileWriteInProgress = true;
    std::ofstream outFile("students.txt");
    if (!outFile) {
        std::cout << "Error opening file for writing.\n";
        fileWriteInProgress = false;
        return;
    }
    
    outFile << "ID|Name|Mark1|Mark2|Mark3|Total|Average|Grade|Status\n";
    for (int i = 0; i < count; ++i) {
        outFile << students[i].id << "|"
                << students[i].name << "|"
                << students[i].marks[0] << "|"
                << students[i].marks[1] << "|"
                << students[i].marks[2] << "|"
                << students[i].total << "|"
                << students[i].average << "|"
                << gradeToString(students[i].grade) << "|"
                << statusToString(students[i].status) << "\n";
    }
    outFile.close();
    fileWriteInProgress = false;
    std::cout << "Data saved successfully to students.txt.\n";
}

// Replaces in-memory data safely after loading and checking validity of records from students.txt
void loadFromFile(std::unique_ptr<Student[]>& students, int& count) {
    std::ifstream inFile("students.txt");
    if (!inFile) {
        std::cout << "Warning: File does not exist. Program continues safely.\n";
        return;
    }
    
    std::string header;
    // Reads first line and ensures file is not just totally empty
    if (!std::getline(inFile, header)) {
        std::cout << "No records available.\n";
        return;
    }
    
    std::unique_ptr<Student[]> tempArray = std::make_unique<Student[]>(MAX_STUDENTS);
    int tempCount = 0;
    std::string line;
    
    while (std::getline(inFile, line)) {
        if (isBlank(line)) continue;
        
        std::stringstream ss(line);
        std::string token;
        Student temp;
        
        try {
            // Tokenizing input file by pipe delimited separators
            std::getline(ss, token, '|'); temp.id = std::stoi(trim(token));
            std::getline(ss, token, '|'); temp.name = trim(token);
            std::getline(ss, token, '|'); temp.marks[0] = std::stof(trim(token));
            std::getline(ss, token, '|'); temp.marks[1] = std::stof(trim(token));
            std::getline(ss, token, '|'); temp.marks[2] = std::stof(trim(token));
            std::getline(ss, token, '|'); temp.total = std::stof(trim(token));
            std::getline(ss, token, '|'); temp.average = std::stof(trim(token));
            
            std::string gradeStr, statusStr;
            std::getline(ss, gradeStr, '|'); gradeStr = trim(gradeStr);
            std::getline(ss, statusStr); statusStr = trim(statusStr); // Rest of line
            
            // Skip marks that exceed logical bounds
            if (temp.marks[0] < 0 || temp.marks[0] > 100 ||
                temp.marks[1] < 0 || temp.marks[1] > 100 ||
                temp.marks[2] < 0 || temp.marks[2] > 100) {
                continue;
            }
            
            // Validate the enums against known values
            if (gradeStr == "A") temp.grade = Grade::A;
            else if (gradeStr == "B") temp.grade = Grade::B;
            else if (gradeStr == "C") temp.grade = Grade::C;
            else if (gradeStr == "D") temp.grade = Grade::D;
            else if (gradeStr == "F") temp.grade = Grade::F;
            else continue; // Invalid enum value
            
            if (statusStr == "PASS") temp.status = PASS;
            else if (statusStr == "FAIL") temp.status = FAIL;
            else continue; // Invalid enum value
            
            // Check duplicates in loaded data
            if (isDuplicateId(tempArray, tempCount, temp.id)) {
                std::cout << "Warning: Duplicate ID " << temp.id << " found in file. Skipping.\n";
                continue;
            }
            
            copyStudent(tempArray[tempCount++], temp);
            
            if (tempCount >= MAX_STUDENTS) {
                std::cout << "Reached maximum student capacity during loading.\n";
                break;
            }
        } catch (...) {
            // Skip incomplete or corrupt lines using a try catch wrapper
            continue;
        }
    }
    
    if (tempCount == 0) {
        std::cout << "No valid records loaded from file.\n";
    } else {
        // Build final sized array and safely replace the global students memory
        std::unique_ptr<Student[]> finalArray = std::make_unique<Student[]>(tempCount);
        for (int i = 0; i < tempCount; ++i) {
            copyStudent(finalArray[i], tempArray[i]);
        }
        students = std::move(finalArray); // Old memory freed automatically safely
        count = tempCount;
        std::cout << "Successfully loaded " << count << " students from file.\n";
    }
}