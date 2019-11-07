#ifndef __ATOI_H__
#define __ATOI_H__

// Hex string to integer
template <typename T>
T atoi(const char* str) {
    size_t i = 0;
    bool pos = true;
    T num = 0;

    while (str[i] == ' ') ++i;

    if (str[i] == '-') {
        ++i;
        pos = false;
    }
    
    while (true) {
        const char c = str[i];
        if (c == '\0') {
            return pos ? num : -num;
        } else {
            num <<= 4;
            if (c >= 'a') {
                constexpr char base = 'a' - 10;
                num |= c - base;   //a - f
            } if (c >= 'A') {
                constexpr char base = 'A' - 10;
                num |= c - base;   //A - F
            } else if (c >= '0') {
                num |= c - '0';   //0 - 9
            } else {
                return pos ? num : -num;  //space
            }
        }
        ++i;
    }
    // should not reach here
    return pos ? num : -num;
}

#endif
