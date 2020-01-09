#pragma once
#include <QList>
#include <string>
#include <array>

class StatSheet
{
public:
    StatSheet(const std::string& team);

    QList<QString> value_defs() const;
    constexpr std::array<const char*, 6> table_row_headers() const;
private:
    std::string m_team;
    bool at_home;
    float m_gf;
    float m_ga;
    float m_pp;
    float m_pk;
    float m_sf;
    float m_sa;
};
