#pragma once
#include <Arduino.h>

class MemoryHelper {
public:
    /**
     * @brief Clean copy string to char[]
     * 
     * @param src           Source string to copy
     * @param dest          Destination char array
     * @param destSize      Destination array size
     * @return size_t       Num bytes copied
     */
    static size_t stringToChar(String src, char *dest, size_t destSize) {
        memset(dest, 0, destSize);
        if (src.length() <= 0) {
            return 0;
        }
        if (src.length() < destSize) {
            destSize = src.length();
        }
        memcpy(dest, src.c_str(), destSize);
        return destSize;
    }
};