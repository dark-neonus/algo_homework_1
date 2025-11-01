#ifndef DATABASE_H
#define DATABASE_H

#include "Student.h"
#include <vector>
#include <unordered_map>
#include <map>
#include <set>
#include <fstream>
#include <algorithm>
#include <memory>

// Base interface for all database variants
class IDatabase {
public:
    virtual ~IDatabase() = default;
    virtual void loadFromFile(const std::string& filename) = 0;
    virtual std::vector<Student> findByNameSurname(const std::string& name, const std::string& surname) = 0;
    virtual std::set<std::string> findGroupsWithDuplicateNameSurname() = 0;
    virtual bool updateGroupByEmail(const std::string& email, const std::string& newGroup) = 0;
    virtual std::vector<Student> getAllStudents() const = 0;
    virtual size_t getMemoryUsage() const = 0;
};

// Variant 1: Hash map based lookup
class HashMapDB : public IDatabase {
private:
    std::vector<Student> students;
    std::unordered_map<std::string, std::vector<size_t>> nameIndex;
    std::unordered_map<std::string, size_t> emailIndex;

    std::string getNameKey(const std::string& name, const std::string& surname) const {
        return name + "|" + surname;
    }

public:
    void loadFromFile(const std::string& filename) override {
        std::ifstream file(filename);
        std::string line;
        
        students.clear();
        nameIndex.clear();
        emailIndex.clear();
        
        bool firstLine = true;
        while (std::getline(file, line)) {
            if (line.empty()) continue;
            // Skip header of csv
            if (firstLine && line.find("m_name") == 0) {
                firstLine = false;
                continue;
            }
            firstLine = false;
            
            Student s = Student::fromCSV(line);
            size_t idx = students.size();
            students.push_back(s);
            
            std::string key = getNameKey(s.m_name, s.m_surname);
            nameIndex[key].push_back(idx);
            emailIndex[s.m_email] = idx;
        }
    }

    std::vector<Student> findByNameSurname(const std::string& name, const std::string& surname) override {
        std::vector<Student> result;
        std::string key = getNameKey(name, surname);
        auto it = nameIndex.find(key);
        if (it != nameIndex.end()) {
            for (size_t idx : it->second) {
                result.push_back(students[idx]);
            }
        }
        return result;
    }

    std::set<std::string> findGroupsWithDuplicateNameSurname() override {
        std::set<std::string> result;
        for (const auto& [key, indices] : nameIndex) {
            if (indices.size() > 1) {
                std::set<std::string> groups;
                for (size_t idx : indices) {
                    groups.insert(students[idx].m_group);
                }
                if (groups.size() > 1) {
                    result.insert(groups.begin(), groups.end());
                }
            }
        }
        return result;
    }

    bool updateGroupByEmail(const std::string& email, const std::string& newGroup) override {
        auto it = emailIndex.find(email);
        if (it != emailIndex.end()) {
            students[it->second].m_group = newGroup;
            return true;
        }
        return false;
    }

    std::vector<Student> getAllStudents() const override {
        return students;
    }

    size_t getMemoryUsage() const override {
        size_t size = students.capacity() * sizeof(Student);
        for (const auto& [k, v] : nameIndex) {
            size += k.capacity() + v.capacity() * sizeof(size_t);
        }
        for (const auto& [k, v] : emailIndex) {
            size += k.capacity() + sizeof(size_t);
        }
        return size;
    }
};

// Variant 2: Mixed approach (hash for emails + vector search for names)
class MixedDB : public IDatabase {
private:
    std::vector<Student> students;
    std::unordered_map<std::string, size_t> emailIndex;

public:
    void loadFromFile(const std::string& filename) override {
        std::ifstream file(filename);
        std::string line;
        
        students.clear();
        emailIndex.clear();
        
        bool firstLine = true;
        while (std::getline(file, line)) {
            if (line.empty()) continue;
            // Skip header of csv
            if (firstLine && line.find("m_name") == 0) {
                firstLine = false;
                continue;
            }
            firstLine = false;
            
            Student s = Student::fromCSV(line);
            size_t idx = students.size();
            students.push_back(s);
            emailIndex[s.m_email] = idx;
        }
    }

    std::vector<Student> findByNameSurname(const std::string& name, const std::string& surname) override {
        std::vector<Student> result;
        for (const auto& s : students) {
            if (s.m_name == name && s.m_surname == surname) {
                result.push_back(s);
            }
        }
        return result;
    }

    std::set<std::string> findGroupsWithDuplicateNameSurname() override {
        std::set<std::string> result;
        std::map<std::string, std::set<std::string>> nameToGroups;
        
        for (const auto& s : students) {
            std::string key = s.m_name + "|" + s.m_surname;
            nameToGroups[key].insert(s.m_group);
        }
        
        for (const auto& [key, groups] : nameToGroups) {
            if (groups.size() > 1) {
                result.insert(groups.begin(), groups.end());
            }
        }
        return result;
    }

    bool updateGroupByEmail(const std::string& email, const std::string& newGroup) override {
        auto it = emailIndex.find(email);
        if (it != emailIndex.end()) {
            students[it->second].m_group = newGroup;
            return true;
        }
        return false;
    }

    std::vector<Student> getAllStudents() const override {
        return students;
    }

    size_t getMemoryUsage() const override {
        size_t size = students.capacity() * sizeof(Student);
        for (const auto& [k, v] : emailIndex) {
            size += k.capacity() + sizeof(size_t);
        }
        return size;
    }
};

// Variant 3: Map-based (BST approach)
class MapDB : public IDatabase {
private:
    std::vector<Student> students;
    std::map<std::string, std::vector<size_t>> nameIndex;
    std::unordered_map<std::string, size_t> emailIndex;

    std::string getNameKey(const std::string& name, const std::string& surname) const {
        return name + "|" + surname;
    }

public:
    void loadFromFile(const std::string& filename) override {
        std::ifstream file(filename);
        std::string line;
        
        students.clear();
        nameIndex.clear();
        emailIndex.clear();
        
        bool firstLine = true;
        while (std::getline(file, line)) {
            if (line.empty()) continue;
            // Skip header if it starts with "m_name"
            if (firstLine && line.find("m_name") == 0) {
                firstLine = false;
                continue;
            }
            firstLine = false;
            
            Student s = Student::fromCSV(line);
            size_t idx = students.size();
            students.push_back(s);
            
            std::string key = getNameKey(s.m_name, s.m_surname);
            nameIndex[key].push_back(idx);
            emailIndex[s.m_email] = idx;
        }
    }

    std::vector<Student> findByNameSurname(const std::string& name, const std::string& surname) override {
        std::vector<Student> result;
        std::string key = getNameKey(name, surname);
        auto it = nameIndex.find(key);
        if (it != nameIndex.end()) {
            for (size_t idx : it->second) {
                result.push_back(students[idx]);
            }
        }
        return result;
    }

    std::set<std::string> findGroupsWithDuplicateNameSurname() override {
        std::set<std::string> result;
        for (const auto& [key, indices] : nameIndex) {
            if (indices.size() > 1) {
                std::set<std::string> groups;
                for (size_t idx : indices) {
                    groups.insert(students[idx].m_group);
                }
                if (groups.size() > 1) {
                    result.insert(groups.begin(), groups.end());
                }
            }
        }
        return result;
    }

    bool updateGroupByEmail(const std::string& email, const std::string& newGroup) override {
        auto it = emailIndex.find(email);
        if (it != emailIndex.end()) {
            students[it->second].m_group = newGroup;
            return true;
        }
        return false;
    }

    std::vector<Student> getAllStudents() const override {
        return students;
    }

    size_t getMemoryUsage() const override {
        size_t size = students.capacity() * sizeof(Student);
        for (const auto& [k, v] : nameIndex) {
            // 32 is BSD overhead
            size += k.capacity() + v.capacity() * sizeof(size_t) + 32;
        }
        for (const auto& [k, v] : emailIndex) {
            size += k.capacity() + sizeof(size_t);
        }
        return size;
    }
};

#endif
