//
// Created by cx on 2020-03-29.
//

#pragma once
#include <string>
#include <vector>
#include <nlohmann/json.hpp>
using json = nlohmann::json;


class Team {
public:
    Team() = default;

    [[nodiscard]] int id() const;
    [[nodiscard]] const std::string& team_name() const;
    [[nodiscard]] const std::vector<std::string>& abbreviations() const;

    friend void from_json(const json& j, Team& team);

private:
    int m_id;
    std::string m_team_name;
    std::vector<std::string> m_abbreviations;


};