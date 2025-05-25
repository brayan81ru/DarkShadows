#pragma once
#include <cstring>
#include <cstdarg>
#include <vector>
#include <algorithm>

namespace DSEngine {
    class DSString {
        char* m_data = nullptr;
        size_t m_length = 0;
        size_t m_capacity = 0;

        // Private helper methods
        void reallocate(size_t newCapacity);
        void copyFrom(const char* str, size_t length);

    public:
        // Constructors & Destructor
        DSString();
        DSString(const char* str);
        DSString(const DSString& other);
        DSString(DSString&& other) noexcept;
        ~DSString();

        // Assignment operators
        DSString& operator=(const char* str);
        DSString& operator=(const DSString& other);
        DSString& operator=(DSString&& other) noexcept;

        // Concatenation
        DSString& operator+=(const char* str);
        DSString& operator+=(const DSString& other);
        DSString operator+(const char* str) const;
        DSString operator+(const DSString& other) const;

        // Comparison
        bool operator==(const char* str) const;
        bool operator==(const DSString& other) const;
        bool operator!=(const char* str) const;
        bool operator!=(const DSString& other) const;

        // Accessors
        size_t Length() const;
        char CharacterAtPos(size_t pos) const;
        int Pos(const char* substr) const;

        // Modifiers
        DSString& ToUpper();
        DSString& ToLower();
        DSString& Trim();

        // Formatting
        static DSString Format(const char* format, ...);

        // Conversion
        const char* c_str() const { return m_data ? m_data : ""; }

        // For range-based for loops
        char* begin() { return m_data; }
        char* end() { return m_data + m_length; }
        const char* begin() const { return m_data; }
        const char* end() const { return m_data + m_length; }
    };
}