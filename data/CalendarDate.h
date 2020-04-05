//
// Created by cx on 2020-03-21.
//

#pragma once

#include <QtCore/QDate>

struct CalendarDate {
    CalendarDate() = default;
    ~CalendarDate() = default;
    int day, month, year;

    static CalendarDate from(const QDate& date);
};

bool operator<(const CalendarDate& lhs, const CalendarDate& rhs) {
    return (lhs.year < rhs.year || lhs.month < rhs.month || lhs.day < rhs.day);
}

bool operator==(const CalendarDate& lhs, const CalendarDate& rhs) {
    return (lhs.year == rhs.year && lhs.month == rhs.month && lhs.day == rhs.day);
}

CalendarDate CalendarDate::from(const QDate &date) {
    CalendarDate cal_date{};
    cal_date.day = date.day();
    cal_date.month = date.month();
    cal_date.year = date.year();
    return cal_date;
}
