#include "Student.h"
#include "Database.h"
#include "Sorter.h"
#include <iostream>
#include <chrono>
#include <random>
#include <iomanip>

class DataHelper {
private:
    std::mt19937 rng;
    std::vector<Student> allStudents;
    std::vector<std::string> uniqueNames;
    std::vector<std::string> uniqueSurnames;
    std::vector<std::string> uniqueGroups;
    std::vector<std::string> uniqueEmails;

public:
    DataHelper() : rng(std::random_device{}()) {}

    void loadFullDataset(const std::string& filename) {
        std::ifstream file(filename);
        std::string line;
        bool firstLine = true;
        
        std::set<std::string> names, surnames, groups;
        
        while (std::getline(file, line)) {
            if (line.empty()) continue;
            if (firstLine && line.find("m_name") == 0) {
                firstLine = false;
                continue;
            }
            firstLine = false;
            
            Student s = Student::fromCSV(line);
            allStudents.push_back(s);
            names.insert(s.m_name);
            surnames.insert(s.m_surname);
            groups.insert(s.m_group);
            uniqueEmails.push_back(s.m_email);
        }
        
        uniqueNames.assign(names.begin(), names.end());
        uniqueSurnames.assign(surnames.begin(), surnames.end());
        uniqueGroups.assign(groups.begin(), groups.end());
    }

    void createSubset(const std::string& outputFile, size_t count) {
        std::ofstream file(outputFile);
        for (size_t i = 0; i < std::min(count, allStudents.size()); i++) {
            file << allStudents[i].toCSV() << "\n";
        }
    }

    std::string getRandomName() {
        std::uniform_int_distribution<> dist(0, uniqueNames.size() - 1);
        return uniqueNames[dist(rng)];
    }

    std::string getRandomSurname() {
        std::uniform_int_distribution<> dist(0, uniqueSurnames.size() - 1);
        return uniqueSurnames[dist(rng)];
    }

    std::string getRandomEmail(size_t maxId) {
        std::uniform_int_distribution<> dist(0, std::min(maxId, uniqueEmails.size()) - 1);
        return uniqueEmails[dist(rng)];
    }

    std::string getRandomGroup() {
        std::uniform_int_distribution<> dist(0, uniqueGroups.size() - 1);
        return uniqueGroups[dist(rng)];
    }
};

class Benchmark {
private:
    DataHelper dataHelper;

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
                auto result = db.findByNameSurname(dataHelper.getRandomName(), dataHelper.getRandomSurname());
            } else if (op == 1) {
                // Operation 2: Find groups with duplicate name+surname
                auto result = db.findGroupsWithDuplicateNameSurname();
            } else {
                // Operation 3: Update group by email
                db.updateGroupByEmail(dataHelper.getRandomEmail(datasetSize), dataHelper.getRandomGroup());
            }
            
            opsCount++;
        }

        std::cout << "      Operations in 10s: " << opsCount << std::endl;
    }

public:
    void loadData(const std::string& filename) {
        dataHelper.loadFullDataset(filename);
    }

    void runBenchmark(const std::string& variantName, IDatabase& db, size_t datasetSize) {
        std::cout << "  Testing " << variantName << " with " << datasetSize << " records:" << std::endl;

        std::string filename = "test_" + std::to_string(datasetSize) + ".csv";
        dataHelper.createSubset(filename, datasetSize);

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

        // Create subset of data
        std::string filename = "test_sort_" + std::to_string(datasetSize) + ".csv";
        dataHelper.createSubset(filename, datasetSize);

        HashMapDB db;
        db.loadFromFile(filename);
        
        // Test std sort
        {
            auto students = db.getAllStudents();
            auto start = std::chrono::high_resolution_clock::now();
            Sorter::standardSort(students);
            auto end = std::chrono::high_resolution_clock::now();
            double time = std::chrono::duration<double>(end - start).count();
            std::cout << "  Standard sort: " << std::fixed << std::setprecision(4) << time << "s" << std::endl;
            Sorter::saveToCSV(students, "sorted_standard.csv");
        }

        // Test my radix sort
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
    
    std::cout << "Loading full dataset from students.csv..." << std::endl;
    try {
        benchmark.loadData("../students.csv");
        std::cout << "Dataset loaded successfully\n" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error loading students.csv: " << e.what() << std::endl;
        std::cerr << "Make sure students.csv exists in the parent directory." << std::endl;
        return 1;
    }

    std::vector<size_t> sizes = {100, 1000, 10000, 100000};

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

    std::cout << "\n\n=== Sort Benchmarks ===" << std::endl;
    for (size_t size : sizes) {
        benchmark.runSortBenchmark(size);
    }

    return 0;
}
