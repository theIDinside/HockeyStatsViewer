//
// Created by cx on 2020-03-21.
//

#pragma once

#include <QtCore/QDate>
#include <iosfwd>

struct CalendarDate {
    CalendarDate() = default;
    CalendarDate(int day, int month, int year) : day(day), month(month), year(year) {}
    ~CalendarDate() = default;
    int day, month, year;

    static CalendarDate from(const QDate& date) {
        CalendarDate cal_date{};
        cal_date.day = date.day();
        cal_date.month = date.month();
        cal_date.year = date.year();
        return cal_date;
    }

    friend bool operator<(const CalendarDate& lhs, const CalendarDate& rhs);
    friend bool operator>(const CalendarDate& lhs, const CalendarDate& rhs);
    friend bool operator==(const CalendarDate& lhs, const CalendarDate& rhs);
    friend std::ostream& operator<<(std::ostream& os, const CalendarDate& c);
};