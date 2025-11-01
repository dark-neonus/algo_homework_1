#ifndef STUDENT_H
#define STUDENT_H

#include <string>
#include <sstream>
#include <vector>

struct Student {
    std::string m_name;
    std::string m_surname;
    std::string m_email;
    int m_birth_year;
    int m_birth_month;
    int m_birth_day;
    std::string m_group;
    float m_rating;
    std::string m_phone_number;

    static Student fromCSV(const std::string& line) {
        Student s;
        std::stringstream ss(line);
        std::string field;
        
        std::getline(ss, s.m_name, ',');
        std::getline(ss, s.m_surname, ',');
        std::getline(ss, s.m_email, ',');
        
        std::getline(ss, field, ',');
        s.m_birth_year = std::stoi(field);
        
        std::getline(ss, field, ',');
        s.m_birth_month = std::stoi(field);
        
        std::getline(ss, field, ',');
        s.m_birth_day = std::stoi(field);
        
        std::getline(ss, s.m_group, ',');
        
        std::getline(ss, field, ',');
        s.m_rating = std::stof(field);

        // Handle case where last field dont have trailing comma
        if (std::getline(ss, s.m_phone_number, ',') || std::getline(ss, s.m_phone_number)) {
            // Remove any trailing whitespace or newline
            s.m_phone_number.erase(s.m_phone_number.find_last_not_of(" \n\r\t") + 1);
        }
        
        return s;
    }

    std::string toCSV() const {
        std::stringstream ss;
        ss << m_name << "," << m_surname << "," << m_email << ","
           << m_birth_year << "," << m_birth_month << "," << m_birth_day << ","
           << m_group << "," << m_rating << "," << m_phone_number;
        return ss.str();
    }
};

// Comparator for sorting by (surname, name)
struct StudentComparator {
    bool operator()(const Student& a, const Student& b) const {
        if (a.m_surname != b.m_surname)
            return a.m_surname < b.m_surname;
        return a.m_name < b.m_name;
    }
};

#endif
