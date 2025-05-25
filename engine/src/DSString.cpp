#include "DSString.h"
#include <cctype>
#include <stdexcept>
namespace DSEngine {
    // Private helper to reallocate memory
    void DSString::reallocate(size_t newCapacity) {
        if (newCapacity <= m_capacity) return;

        char* newData = new char[newCapacity + 1]; // +1 for null terminator
        if (m_data) {
            std::memcpy(newData, m_data, m_length + 1); // Copy existing data
            delete[] m_data;
        }
        m_data = newData;
        m_capacity = newCapacity;
    }

    // Private helper to copy from C-string
    void DSString::copyFrom(const char* str, size_t length) {
        if (length == 0) {
            if (m_data) m_data[0] = '\0';
            m_length = 0;
            return;
        }

        reallocate(length);
        std::memcpy(m_data, str, length);
        m_data[length] = '\0';
        m_length = length;
    }

    // Constructors
    DSString::DSString() {
        reallocate(15); // Default small buffer
        m_data[0] = '\0';
    }

    DSString::DSString(const char* str) {
        if (str) {
            m_length = std::strlen(str);
            copyFrom(str, m_length);
        } else {
            reallocate(15);
            m_data[0] = '\0';
        }
    }

    DSString::DSString(const DSString& other) {
        copyFrom(other.m_data, other.m_length);
    }

    DSString::DSString(DSString&& other) noexcept
        : m_data(other.m_data), m_length(other.m_length), m_capacity(other.m_capacity) {
        other.m_data = nullptr;
        other.m_length = 0;
        other.m_capacity = 0;
    }

    DSString::~DSString() {
        delete[] m_data;
    }

    // Assignment operators
    DSString& DSString::operator=(const char* str) {
        if (str) {
            size_t len = std::strlen(str);
            copyFrom(str, len);
        } else {
            copyFrom("", 0);
        }
        return *this;
    }

    DSString& DSString::operator=(const DSString& other) {
        if (this != &other) {
            copyFrom(other.m_data, other.m_length);
        }
        return *this;
    }

    DSString& DSString::operator=(DSString&& other) noexcept {
        if (this != &other) {
            delete[] m_data;
            m_data = other.m_data;
            m_length = other.m_length;
            m_capacity = other.m_capacity;

            other.m_data = nullptr;
            other.m_length = 0;
            other.m_capacity = 0;
        }
        return *this;
    }

    // Concatenation
    DSString& DSString::operator+=(const char* str) {
        if (!str) return *this;

        size_t len = std::strlen(str);
        if (len == 0) return *this;

        size_t newLength = m_length + len;
        reallocate(newLength);
        std::memcpy(m_data + m_length, str, len + 1); // +1 to copy null terminator
        m_length = newLength;
        return *this;
    }

    DSString& DSString::operator+=(const DSString& other) {
        return operator+=(other.m_data);
    }

    DSString DSString::operator+(const char* str) const {
        DSString result(*this);
        result += str;
        return result;
    }

    DSString DSString::operator+(const DSString& other) const {
        return operator+(other.m_data);
    }

    // Comparison
    bool DSString::operator==(const char* str) const {
        if (!str) return m_length == 0;
        return std::strcmp(m_data, str) == 0;
    }

    bool DSString::operator==(const DSString& other) const {
        return operator==(other.m_data);
    }

    bool DSString::operator!=(const char* str) const {
        return !operator==(str);
    }

    bool DSString::operator!=(const DSString& other) const {
        return !operator==(other);
    }

    // Accessors
    size_t DSString::Length() const {
        return m_length;
    }

    char DSString::CharacterAtPos(size_t pos) const {
        if (pos >= m_length) {
            throw std::out_of_range("Position out of range");
        }
        return m_data[pos];
    }

    int DSString::Pos(const char* substr) const {
        if (!substr || !m_data) return -1;

        const char* pos = std::strstr(m_data, substr);
        if (pos) {
            return static_cast<int>(pos - m_data);
        }
        return -1;
    }

    // Modifiers
    DSString& DSString::ToUpper() {
        if (!m_data) return *this;

        for (size_t i = 0; i < m_length; ++i) {
            m_data[i] = static_cast<char>(std::toupper(static_cast<unsigned char>(m_data[i])));
        }
        return *this;
    }

    DSString& DSString::ToLower() {
        if (!m_data) return *this;

        for (size_t i = 0; i < m_length; ++i) {
            m_data[i] = static_cast<char>(std::tolower(static_cast<unsigned char>(m_data[i])));
        }
        return *this;
    }

    DSString& DSString::Trim() {
        if (!m_data || m_length == 0) return *this;

        size_t start = 0;
        size_t end = m_length - 1;

        // Find first non-whitespace
        while (start <= end && std::isspace(static_cast<unsigned char>(m_data[start]))) {
            start++;
        }

        // Find last non-whitespace
        while (end >= start && std::isspace(static_cast<unsigned char>(m_data[end]))) {
            end--;
        }

        if (start > 0 || end < m_length - 1) {
            size_t newLength = end - start + 1;
            if (newLength > 0) {
                std::memmove(m_data, m_data + start, newLength);
            }
            m_data[newLength] = '\0';
            m_length = newLength;
        }

        return *this;
    }

    // Formatting
    // In DSString.cpp replace the Format implementation with:
    DSString DSString::Format(const char* format, ...) {
        if (!format) return DSString();

        va_list args;
        va_start(args, format);

        // First pass: calculate required size
        int size = 0;
        const char* p = format;
        va_list argsCopy;
        va_copy(argsCopy, args);

        while (*p != '\0') {
            if (*p == '%') {
                p++;
                if (*p == 'D') {
                    // Handle DSString argument
                    const DSString* dsStr = va_arg(argsCopy, const DSString*);
                    size += (dsStr ? dsStr->Length() : 0);
                    p++;
                } else {
                    // Handle standard format specifiers
                    char buff[32];
                    size += vsnprintf(buff, sizeof(buff), p, argsCopy);
                    while (*p != '\0' && !isalpha(*p)) p++; // Skip to end of specifier
                }
            } else {
                size++;
                p++;
            }
        }
        va_end(argsCopy);

        if (size <= 0) {
            va_end(args);
            return DSString();
        }

        // Second pass: actual formatting
        DSString result;
        result.reallocate(size); // Pre-allocate space

        p = format;
        char* out = result.m_data;

        while (*p != '\0') {
            if (*p == '%') {
                p++;
                if (*p == 'D') {
                    // Handle DSString argument
                    const DSString* dsStr = va_arg(args, const DSString*);
                    if (dsStr) {
                        size_t len = dsStr->Length();
                        std::memcpy(out, dsStr->c_str(), len);
                        out += len;
                    }
                    p++;
                } else {
                    // Handle standard format specifiers
                    char specifier[32];
                    char* s = specifier;
                    *s++ = '%';

                    // Reconstruct the format specifier
                    while (*p != '\0' && !isalpha(*p)) {
                        *s++ = *p++;
                    }
                    if (*p != '\0') {
                        *s++ = *p++;
                    }
                    *s = '\0';

                    out += vsprintf(out, specifier, args);
                }
            } else {
                *out++ = *p++;
            }
        }
        *out = '\0';
        result.m_length = out - result.m_data;

        va_end(args);
        return result;
    }
}