#ifndef UTILITY_H
#define UTILITY_H
#include <iostream>
#include <unordered_map>
#include <map>

template <typename Cont>
void print(const Cont& cont) {
    for(auto&& elem:cont) {
        std::cout << elem << ' ';
    }
    std::cout << std::endl;
}
template <typename T, typename U>
void print(const std::unordered_map<T, U>& map) {
    for(auto&& [key, value]:map) {
        std::cout << key << ": " << value << std::endl;
    }
}
template <typename T, typename U>
void print(const std::map<T, U>& map) {
    for(auto&& [key, value]:map) {
        std::cout << key << ": " << value << std::endl;
    }
}

#endif // UTILITY_H
