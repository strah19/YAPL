#ifndef SYMTABLE_H
#define SYMTABLE_H

#include "common.h"
#include <map>

using SymTableError = int;

#define UNDEFINED_KEY -1

template <typename Key, typename Val>
class SymTable {
public:
    void define(const Key& key, const Val& val) {
        table[key] = val;
    }

    Val get(const Key& key) {
        return table[key];
    }

    Key get_key(const Val& val) {
        for (auto& it : table) {
            if (it.second == val) 
                return it.first;
        }
        throw UNDEFINED_KEY;
    }

    bool found(const Key& key) {
        return (table.find(key) != table.end());
    }

    void update(const Key& key, const Val& val) {
        if (found(key))
            table[key] = val;
        throw UNDEFINED_KEY;
    }
private:
    std::map<Key, Val> table;
};

#endif // !SYMTABLE_H