#include "utils.h"

auto timeZoneAdjustedTimePoint(int span) {
    auto now = std::chrono::system_clock::now();
    std::time_t current_time = std::chrono::system_clock::to_time_t(now);
    std::chrono::system_clock::time_point time_point = std::chrono::system_clock::from_time_t(current_time);
    auto adjusted_for_timeZone = time_point + std::chrono::hours{span};
    auto new_time_point = std::chrono::system_clock::to_time_t(adjusted_for_timeZone);
    return new_time_point;
}

std::pair<std::chrono::system_clock::time_point, std::chrono::system_clock::time_point> make_date_range_from_today(unsigned int span)
{
    auto tp = timeZoneAdjustedTimePoint(-6);
    auto begin = std::gmtime(&tp);
    begin->tm_hour = 0;
    begin->tm_min=0;
    begin->tm_sec=0;
    auto end = *begin;
    end.tm_mday += span;
    end.tm_isdst = -1;
    auto b = std::mktime(begin);
    auto e = std::mktime(&end);
    auto begin_ctp =    std::chrono::system_clock::from_time_t(b);
    auto end_ctp =      std::chrono::system_clock::from_time_t(e);
    return std::make_pair(begin_ctp, end_ctp);
}
