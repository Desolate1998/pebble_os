#include "string.h"

namespace std
{
    String::String()
    {
        length = 0;
        buffer[0] = '\0';
    }

    String::String(const char* str)
    {
        copy(str);
    }

    void String::copy(const char* str)
    {
        length = 0;
        while (str[length] && length < 255)
        {
            buffer[length] = str[length];
            length++;
        }
        buffer[length] = '\0';
    }

    void String::append(const char* str)
    {
        int i = 0;
        while (str[i] && length < 255)
        {
            buffer[length++] = str[i++];
        }
        buffer[length] = '\0';
    }

    bool String::equals(const char* str) const
    {
        return strcmp(buffer, str);
    }

    bool String::equals(const String& other) const
    {
        return strcmp(buffer, other.buffer);
    }

    void String::clear()
    {
        length = 0;
        buffer[0] = '\0';
    }

    const char* String::c_str() const
    {
        return buffer;
    }

    int String::strlen(const char* str)
    {
        int len = 0;
        while (str[len]) ++len;
        return len;
    }

    bool String::strcmp(const char* a, const char* b)
    {
        while (*a && *b)
        {
            if (*a != *b) return false;
            ++a; ++b;
        }
        return *a == '\0' && *b == '\0';
    }

    const char* String::strchr(const char* str, char c)
    {
        while (*str)
        {
            if (*str == c) return str;
            ++str;
        }
        return nullptr;
    }
}
