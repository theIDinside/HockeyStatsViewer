//
// Created by cx on 2020-03-29.
//

#include "Team.h"
int Team::id() const {
    return m_id;
}

const std::string &Team::team_name() const {
    return m_team_name;
}

const std::vector<std::string> &Team::abbreviations() const {
    return m_abbreviations;
}

void to_json(json& j, const Team& team) {
    j = json{{"id", team.id()}, {"name", team.team_name()}, {"abbreviations", team.abbreviations()}};
}
void from_json(const json& j, Team& team) {
    j.at("id").get_to(team.m_id);
    j.at("name").get_to(team.m_team_name);
    j.at("abbreviations").get_to(team.m_abbreviations);
}

bool operator==(const Team& lhs, const Team& rhs) {
    return lhs.id() == rhs.id();
}

bool operator==(const Team& lhs, const std::string& team_string) {
    if(team_string == lhs.team_name()) {
        return true;
    }
    for(const auto& abbr: lhs.m_abbreviations) {
        if(abbr == team_string) return true;
    }
    return false;
}

bool operator<(const Team& lhs, const Team& rhs) {
    return lhs.id() < rhs.id();
}
bool operator>(const Team& lhs, const Team& rhs) {
    return lhs.id() > rhs.id();
}