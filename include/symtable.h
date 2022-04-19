#ifndef SYMTABLE_H
#define SYMTABLE_H

#include "common.h"
#include <vector>

struct Entry {
    const char* name;
    int type;
};

class SymTable {
public:
    int get_index(const char* name);
    
    Entry* insert(const char* name, int type);
    Entry* look_up(const char* name);
    Entry* look_up_type(const char* name, int type);
    Entry* look_up_by_type(int type);
private:
    std::vector<Entry> table;
};

#endif // !SYMTABLE_H