#include "CalendarDate.h"

bool operator<(const CalendarDate& lhs, const CalendarDate& rhs) {

    if(lhs.year < rhs.year) {
        return true;
    } else if(lhs.year == rhs.year && lhs.month < rhs.month) {
        return true;
    } else if(lhs.year == rhs.year && lhs.month == rhs.month && lhs.day < rhs.day) {
        return true;
    } else {
        return false;
    }

}

bool operator>(const CalendarDate& lhs, const CalendarDate& rhs) {
    return !(lhs < rhs);
}


bool operator==(const CalendarDate& lhs, const CalendarDate& rhs) {
    return (lhs.year == rhs.year && lhs.month == rhs.month && lhs.day == rhs.day);
}