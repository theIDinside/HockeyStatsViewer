#include "statsheet.h"

StatSheet::StatSheet(const std::string& team) : m_team(team)
{

}

QList<QString> StatSheet::value_defs() const
{
    QList<QString> list;
    list << "GF" << "GA" << "PP" << "PK" << "SF" << "SA";
    return list;
}

constexpr std::array<const char*, 6> StatSheet::table_row_headers() const
{
    return {"GF", "GA", "PP", "PK", "SF", "SA"};
}
