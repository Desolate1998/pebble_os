#include "string.h"

namespace std
{
    String::String()
    {
        len = 0;
        buffer[0] = '\0';
    }

    String::String(const char* str)
    {
        copy(str);
    }

    void String::copy(const char* str)
    {
        len = 0;
        while (str[len] && len < 255)
        {
            buffer[len] = str[len];
            len++;
        }
        buffer[len] = '\0';
    }

    void String::append(const char* str)
    {
        int i = 0;
        while (str[i] && len < 255)
        {
            buffer[len++] = str[i++];
        }
        buffer[len] = '\0';
    }

    void String::append_char(char c)
    {
        if (len < 255)
        {
            buffer[len++] = c;
            buffer[len] = '\0';
        }
    }

    void String::pop()
    {
        if (len > 0)
        {
            len--;
            buffer[len] = '\0';
        }
    }

    void String::clear()
    {
        len = 0;
        buffer[0] = '\0';
    }

    bool String::equals(const char* str) const
    {
        return strcmp(buffer, str);
    }

    bool String::equals(const String& other) const
    {
        return strcmp(buffer, other.buffer);
    }

    const char* String::c_str() const
    {
        return buffer;
    }

    int String::length() const
    {
        return len;
    }

    int String::strlen(const char* str)
    {
        int l = 0;
        while (str[l]) ++l;
        return l;
    }

    bool String::strcmp(const char* a, const char* b)
    {
        while (*a && *b)
        {
            if (*a != *b) return false;
            ++a;
            ++b;
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
