#pragma once

// System headers. These can be removed, as pre compiled headers are used in CMakeLists.txt
#include <chrono>
#include <ctime>

#include <tuple>
// #include <sstream>
#include <iostream>
// #include <fstream>
#include <QDate>
#include <algorithm>
// #include <shared_mutex>
// #include <mutex>

// Other headers
#include <bsoncxx/types.hpp>
#include <iomanip>
using chrono_tp = std::chrono::system_clock::time_point;

auto timeZoneAdjustedTimePoint(int span);
std::pair<std::chrono::system_clock::time_point, std::chrono::system_clock::time_point> make_date_range_from_today(unsigned int span);

template <typename Integer>
double divide_integers(Integer A, Integer B) {
    return static_cast<double>(A) / static_cast<double>(B);
}

