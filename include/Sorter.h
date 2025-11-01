#ifndef SORTER_H
#define SORTER_H

#include "Student.h"
#include <vector>
#include <algorithm>
#include <fstream>

class Sorter {
public:
    static void standardSort(std::vector<Student>& students) {
        std::sort(students.begin(), students.end(), StudentComparator());
    }

    // MSD radix sort for strings
    static void radixSort(std::vector<Student>& students) {
        if (students.empty()) return;
        radixSortHelper(students, 0, students.size(), 0);
    }

    static void saveToCSV(const std::vector<Student>& students, const std::string& filename) {
        std::ofstream file(filename);
        for (const auto& s : students) {
            file << s.toCSV() << "\n";
        }
    }

private:
    static int getChar(const Student& s, size_t pos) {
        std::string combined = s.m_surname + "|" + s.m_name;
        if (pos >= combined.size()) return -1;
        return static_cast<unsigned char>(combined[pos]);
    }

    static void radixSortHelper(std::vector<Student>& students, size_t start, size_t end, size_t charPos) {
        if (end - start <= 1) return;
        
        // Use insertion sort for small subarrays
        if (end - start < 10) {
            for (size_t i = start + 1; i < end; i++) {
                Student key = students[i];
                size_t j = i;
                while (j > start && StudentComparator()(key, students[j - 1])) {
                    students[j] = students[j - 1];
                    j--;
                }
                students[j] = key;
            }
            return;
        }

        // Count characters (256 possible bytes + 1 for end-of-string)
        const int R = 257;
        std::vector<int> count(R + 1, 0);
        
        // Count frequencies
        for (size_t i = start; i < end; i++) {
            int c = getChar(students[i], charPos);
            count[c + 2]++;
        }
        
        // Compute cumulative counts
        for (int r = 0; r < R; r++) {
            count[r + 1] += count[r];
        }
        
        // Distribute
        std::vector<Student> aux(end - start);
        for (size_t i = start; i < end; i++) {
            int c = getChar(students[i], charPos);
            aux[count[c + 1]++] = students[i];
        }
        
        // Copy back
        for (size_t i = 0; i < aux.size(); i++) {
            students[start + i] = aux[i];
        }
        
        // Recursively sort each character group
        for (int r = 0; r < R; r++) {
            size_t subStart = start + count[r];
            size_t subEnd = start + count[r + 1];
            if (subEnd - subStart > 0 && r >= 0) {
                radixSortHelper(students, subStart, subEnd, charPos + 1);
            }
        }
    }
};

#endif
