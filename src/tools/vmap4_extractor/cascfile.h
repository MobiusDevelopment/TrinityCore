/**
 * This file is part of the MobiusCore project.
 * See AUTHORS file for copyright information.
 */

#define _CRT_SECURE_NO_DEPRECATE
#ifndef _CRT_SECURE_NO_WARNINGS // fuck the police^Wwarnings
#define _CRT_SECURE_NO_WARNINGS
#endif

#ifndef MPQ_H
#define MPQ_H

#include "Define.h"
#include "CascHandles.h"
#include <utility>

class CASCFile
{
    bool eof;
    char *buffer;
    size_t pointer,size;

    // disable copying
    CASCFile(const CASCFile &f) = delete;
    CASCFile& operator=(const CASCFile &f) = delete;

public:
    CASCFile(CASC::StorageHandle const& casc, const char* filename, bool warnNoExist = true);    // filenames are not case sensitive
    ~CASCFile() { close(); }
    size_t read(void* dest, size_t bytes);
    size_t getSize() { return size; }
    size_t getPos() { return pointer; }
    char* getBuffer() { return buffer; }
    char* getPointer() { return buffer + pointer; }
    bool isEof() { return eof; }
    void seek(int offset);
    void seekRelative(int offset);
    void close();
};

inline void flipcc(char *fcc)
{
    std::swap(fcc[0], fcc[3]);
    std::swap(fcc[1], fcc[2]);
}

#endif
