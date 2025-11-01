#include "Student.h"
#include "Database.h"
#include "Sorter.h"
#include <iostream>
#include <chrono>
#include <random>
#include <iomanip>

// Random data generator for testing
class TestDataGenerator {
private:
    std::mt19937 rng;
    
    std::vector<std::string> names = {"Іван", "Марія", "Петро", "Ольга", "Андрій", "Катерина", "Михайло", "Анна"};
    std::vector<std::string> surnames = {"Шевченко", "Коваленко", "Бондаренко", "Ткаченко", "Кравченко", "Мельник", "Петренко", "Савченко"};
    std::vector<std::string> groups = {"AAA-11", "BBB-22", "CCC-33", "DDD-44", "EEE-55"};

public:
    TestDataGenerator() : rng(std::random_device{}()) {}

    void generateCSV(const std::string& filename, size_t count) {
        std::ofstream file(filename);
        std::uniform_int_distribution<> nameDist(0, names.size() - 1);
        std::uniform_int_distribution<> surnameDist(0, surnames.size() - 1);
        std::uniform_int_distribution<> yearDist(1950, 2010);
        std::uniform_int_distribution<> monthDist(1, 12);
        std::uniform_int_distribution<> dayDist(1, 28);
        std::uniform_int_distribution<> groupDist(0, groups.size() - 1);
        std::uniform_real_distribution<> ratingDist(0.0, 100.0);

        for (size_t i = 0; i < count; i++) {
            std::string name = names[nameDist(rng)];
            std::string surname = surnames[surnameDist(rng)];
            std::string email = "student" + std::to_string(i) + "@student.org";
            int year = yearDist(rng);
            int month = monthDist(rng);
            int day = dayDist(rng);
            std::string group = groups[groupDist(rng)];
            float rating = ratingDist(rng);
            std::string phone = "38(099)12-34-567";

            file << name << "," << surname << "," << email << ","
                 << year << "," << month << "," << day << ","
                 << group << "," << std::fixed << std::setprecision(2) << rating << "," << phone << "\n";
        }
    }

    std::string getRandomName() {
        std::uniform_int_distribution<> dist(0, names.size() - 1);
        return names[dist(rng)];
    }

    std::string getRandomSurname() {
        std::uniform_int_distribution<> dist(0, surnames.size() - 1);
        return surnames[dist(rng)];
    }

    std::string getRandomEmail(size_t maxId) {
        std::uniform_int_distribution<> dist(0, maxId - 1);
        return "student" + std::to_string(dist(rng)) + "@student.org";
    }

    std::string getRandomGroup() {
        std::uniform_int_distribution<> dist(0, groups.size() - 1);
        return groups[dist(rng)];
    }
};

// Benchmark runner
class Benchmark {
private:
    TestDataGenerator generator;

    void runOperations(IDatabase& db, size_t datasetSize, int A, int B, int C, double timeLimit) {
        std::mt19937 rng(std::random_device{}());
        std::discrete_distribution<> opDist({static_cast<double>(A), static_cast<double>(B), static_cast<double>(C)});

        auto startTime = std::chrono::high_resolution_clock::now();
        size_t opsCount = 0;
        
        while (true) {
            auto currentTime = std::chrono::high_resolution_clock::now();
            double elapsed = std::chrono::duration<double>(currentTime - startTime).count();
            if (elapsed >= timeLimit) break;

            int op = opDist(rng);
            
            if (op == 0) {
                // Operation 1: Find by name and surname
                auto result = db.findByNameSurname(generator.getRandomName(), generator.getRandomSurname());
            } else if (op == 1) {
                // Operation 2: Find groups with duplicate name+surname
                auto result = db.findGroupsWithDuplicateNameSurname();
            } else {
                // Operation 3: Update group by email
                db.updateGroupByEmail(generator.getRandomEmail(datasetSize), generator.getRandomGroup());
            }
            
            opsCount++;
        }

        std::cout << "      Operations in 10s: " << opsCount << std::endl;
    }

public:
    void runBenchmark(const std::string& variantName, IDatabase& db, size_t datasetSize) {
        std::cout << "  Testing " << variantName << " with " << datasetSize << " records:" << std::endl;

        // Generate test data
        std::string filename = "test_" + std::to_string(datasetSize) + ".csv";
        generator.generateCSV(filename, datasetSize);

        // Load data and measure time
        auto loadStart = std::chrono::high_resolution_clock::now();
        db.loadFromFile(filename);
        auto loadEnd = std::chrono::high_resolution_clock::now();
        double loadTime = std::chrono::duration<double>(loadEnd - loadStart).count();

        std::cout << "      Load time: " << std::fixed << std::setprecision(3) << loadTime << "s" << std::endl;
        std::cout << "      Memory usage: " << db.getMemoryUsage() / 1024 << " KB" << std::endl;

        // Run operations (A=5, B=5, C=50 from variant V1)
        runOperations(db, datasetSize, 5, 5, 50, 10.0);
    }

    void runSortBenchmark(size_t datasetSize) {
        std::cout << "\nSort benchmark with " << datasetSize << " records:" << std::endl;

        // Generate test data
        std::string filename = "test_sort_" + std::to_string(datasetSize) + ".csv";
        generator.generateCSV(filename, datasetSize);

        HashMapDB db;
        db.loadFromFile(filename);
        
        // Test standard sort
        {
            auto students = db.getAllStudents();
            auto start = std::chrono::high_resolution_clock::now();
            Sorter::standardSort(students);
            auto end = std::chrono::high_resolution_clock::now();
            double time = std::chrono::duration<double>(end - start).count();
            std::cout << "  Standard sort: " << std::fixed << std::setprecision(4) << time << "s" << std::endl;
            Sorter::saveToCSV(students, "sorted_standard.csv");
        }

        // Test radix sort
        {
            auto students = db.getAllStudents();
            auto start = std::chrono::high_resolution_clock::now();
            Sorter::radixSort(students);
            auto end = std::chrono::high_resolution_clock::now();
            double time = std::chrono::duration<double>(end - start).count();
            std::cout << "  Radix sort: " << std::fixed << std::setprecision(4) << time << "s" << std::endl;
            Sorter::saveToCSV(students, "sorted_radix.csv");
        }
    }
};

int main() {
    std::cout << "=== Student Database Benchmark ===\n" << std::endl;

    Benchmark benchmark;
    std::vector<size_t> sizes = {100, 1000, 10000, 100000};

    // Test all three variants
    for (size_t size : sizes) {
        std::cout << "\n--- Dataset size: " << size << " ---" << std::endl;
        
        {
            HashMapDB db1;
            benchmark.runBenchmark("Variant 1 (HashMap)", db1, size);
        }
        
        {
            MixedDB db2;
            benchmark.runBenchmark("Variant 2 (Mixed)", db2, size);
        }
        
        {
            MapDB db3;
            benchmark.runBenchmark("Variant 3 (Map/BST)", db3, size);
        }
    }

    // Sort benchmarks
    std::cout << "\n\n=== Sort Benchmarks ===" << std::endl;
    for (size_t size : sizes) {
        benchmark.runSortBenchmark(size);
    }

    // Demonstrate sorting on actual data if students.csv exists
    std::cout << "\n\n=== Sorting students.csv ===" << std::endl;
    HashMapDB db;
    try {
        std::ifstream test("../students.csv");
        if (!test.good()) {
            std::cout << "students.csv file not found in parent directory" << std::endl;
        } else {
            db.loadFromFile("../students.csv");
            auto students = db.getAllStudents();
            
            std::cout << "Loaded " << students.size() << " students" << std::endl;
            
            if (!students.empty()) {
                Sorter::standardSort(students);
                Sorter::saveToCSV(students, "students_sorted.csv");
                std::cout << "Sorted students saved to students_sorted.csv" << std::endl;
            }
        }
    } catch (const std::exception& e) {
        std::cout << "Error: " << e.what() << std::endl;
    }

    return 0;
}
