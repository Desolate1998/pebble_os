#pragma once

namespace std
{
    class String
    {
    public:
        String();
        String(const char* str);

        void copy(const char* str);
        void append(const char* str);
        void append_char(char c);
        void pop();
        void clear();

        bool equals(const char* str) const;
        bool equals(const String& other) const;

        const char* c_str() const;
        int length() const;

        static int strlen(const char* str);
        static bool strcmp(const char* a, const char* b);
        static const char* strchr(const char* str, char c);

    private:
        char buffer[256];
        int len;
    };
}
