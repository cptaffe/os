// Copyright 2016 Connor Taffe

#ifndef SRC_LIBK_DS_MAP_H_
#define SRC_LIBK_DS_MAP_H_

#include "src/libk/ds/vec.h"

namespace basilisk {

template <typename K, typename V>
class Map {
 public:
  class Entry {
   public:
    Entry(K k, V v) : key{k}, value{v} {}
    const K &getKey() const { return key; }
    V getValue() const { return value; }

   private:
    K key;
    V value;
  };

  void insert(K key, V value);
  V operator[](const K &key);
  void erase(K key);

  typename Vec<Entry>::Iter begin() { return entries.begin(); }
  typename Vec<Entry>::Iter end() { return entries.end(); }

 private:
  Vec<Entry> entries;
};

template <typename K, typename V>
void Map<K, V>::insert(K key, V value) {
  entries.push(Entry{key, value});
}

template <typename K, typename V>
V Map<K, V>::operator[](const K &key) {
  for (size_t i; i < entries.getSize(); i++) {
    if (entries[i].getKey() == key) {
      return entries[i].getValue();
    }
  }
  // TODO(cptaffe): throw error
  return entries[0].getValue();
}

template <typename K, typename V>
void Map<K, V>::erase(K key) {
  for (size_t i; i < entries.getSize(); i++) {
    if (entries[i].getKey() == key) {
      entries.erase(i);
    }
  }
}

}  // namespace basilisk

#endif  // SRC_LIBK_DS_MAP_H_
