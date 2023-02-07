//
// Created by jorge on 23/02/2022.
//

#ifndef HARVESTTIMER_QT_CMAKE_MAPUTILS_H
#define HARVESTTIMER_QT_CMAKE_MAPUTILS_H

#include <map>
#include <vector>
#include <set>
#include <QDebug>

class MapUtils {
public:
    template<typename T, typename U>
    static void map_insert_or_create_vector(std::map<T, std::vector<U>> &map, T key, U vector_value);

    template<typename T, typename U, typename V>
    static bool map_insert_or_create_set(std::map<T, std::set<U, V>> &map, T key, U set_value);
//    template<typename T, typename U>
//    static void map_insert_or_create_set(std::map<T, std::set<U>> &map, T key, U set_value);
};

template<typename T, typename U>
void MapUtils::map_insert_or_create_vector(std::map<T, std::vector<U>> &map, T key, U vector_value) {
    typename std::map<T, std::vector<U>>::iterator lb{map.lower_bound(key)};
    if (lb != map.end() && !(map.key_comp()(key, lb->first))) {
        // key already exists
        lb->second.emplace_back(vector_value);
    } else {
        // the key does not exist in the map
        // Use lb as a hint to insert,
        // so it can avoid another lookup
        map.insert(lb, typename std::map<T, std::vector<U>>::value_type(key, {vector_value}));
    }
}

template<typename T, typename U, typename V>
bool MapUtils::map_insert_or_create_set(std::map<T, std::set<U, V>> &map, T key, U set_value) {
    typename std::map<T, std::set<U, V>>::iterator lb{map.lower_bound(key)};

    if (lb != map.end() && !(map.key_comp()(key, lb->first))) {
        // key already exists
        return lb->second.emplace(set_value).second;
    }

    // the key does not exist in the map
    // Use lb as a hint to insert,
    // so it can avoid another lookup
    auto inserted{map.insert(lb, typename std::map<T, std::set<U, V>>::value_type(key, {set_value}))};
    return inserted != map.end();
}


#endif //HARVESTTIMER_QT_CMAKE_MAPUTILS_H
