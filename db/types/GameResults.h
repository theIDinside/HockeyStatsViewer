//
// Created by cx on 2020-03-22.
//

#pragma once

#include <string>
#include <vector>
#include <gametime.h>
#include <valueholder.h>
#include "data/CalendarDate.h"
#include <nlohmann/json.hpp>
#include <data/gamemodel.h>
#include <db/types/Team.h>

using Teams = TeamsValueHolder<std::string>;
using TeamIDs = TeamsValueHolder<int>;
using IntResults = TeamsValueHolder<int>;
using FloatResults = TeamsValueHolder<double>;
using PowerPlay = TeamsValueHolder<SpecialTeams>;
using Assist = std::optional<std::string>;
using json = nlohmann::json;

enum TeamStrength {
    TS_EVEN,
    TS_EVEN_PENALTY_SHOT,
    TS_PENALTY_SHOT,
    TS_EVEN_EMPTY_NET,
    TS_POWER_PLAY,
    TS_SHORT_HANDED,
    TS_SHORT_HANDED_EMPTY_NET,
    TS_SHORT_HANDED_PENALTY_SHOT,
    TS_POWER_PLAY_EMPTY_NET,
    TS_POWER_PLAY_PENALTY_SHOT,
    TS_SHOOTOUT
};

Strength convert_strength(const TeamStrength& str) {
    switch(str) {
        case TS_EVEN:
            return Strength::EVEN;
        case TS_EVEN_PENALTY_SHOT:
            return Strength::EVEN_PENALTY_SHOT;
        case TS_PENALTY_SHOT:
            return Strength::PENALTY_SHOT;
        case TS_EVEN_EMPTY_NET:
            return Strength::EVEN_EMPTY_NET;
        case TS_POWER_PLAY:
            return Strength::POWER_PLAY;
        case TS_SHORT_HANDED:
            return Strength::SHORT_HANDED;
        case TS_SHORT_HANDED_EMPTY_NET:
            return Strength::SHORT_HANDED_EMPTY_NET;
        case TS_SHORT_HANDED_PENALTY_SHOT:
            return Strength::SHORT_HANDED_PENALTY_SHOT;
        case TS_POWER_PLAY_EMPTY_NET:
            return Strength::POWER_PLAY_EMPTY_NET;
        case TS_POWER_PLAY_PENALTY_SHOT:
            return Strength::POWER_PLAY_PENALTY_SHOT;
        case TS_SHOOTOUT:
            return Strength::SHOOTOUT;
    }
}

struct GameInfo {
    GameInfo() = default;
    ~GameInfo() = default;
    int m_game_id;
    int m_home, m_away;
    CalendarDate m_date;
};

struct Goal {
    Goal() = default;
    int m_goal_number{};
    GameTime m_time{};
    TeamStrength m_strength = TeamStrength::TS_EVEN;
    std::string m_scoring_team{};
    std::string m_goal_scorer{};
    Assist m_first_assist{};
    Assist m_second_assist{};

    friend ScoringModel convert(const Goal& goal);
    friend void from_json(const json& j, Goal& g);
};

ScoringModel convert(const Goal& goal) {
    ScoringModel s{goal.m_goal_number, goal.m_time, convert_strength(goal.m_strength), goal.m_scoring_team, goal.m_goal_scorer, goal.m_first_assist, goal.m_second_assist};
    return s;
}

struct Game {
    Game() = default;
    ~Game() = default;
    GameInfo m_game_info;
    std::string m_winning_team{};
    std::vector<IntResults> m_shots{};
    IntResults m_final_result{};
    FloatResults m_face_off{};
    PowerPlay m_power_play{};
    IntResults m_penalty_infraction_minutes{};
    IntResults m_hits{};
    IntResults m_blocked_shots{};
    IntResults m_give_aways{};
    IntResults m_take_aways{};
    std::vector<Goal> m_goals{};

    friend GameModel convert(const Game& game);

};

std::vector<ScoringModel> convert(const std::vector<Goal>& goals) {
    std::vector<ScoringModel> scoring_summary;
    for(const auto& goal : goals) {
        auto score = convert(goal);
        scoring_summary.push_back(score);
    }
    return scoring_summary;
}

void to_json(json& j, const CalendarDate& d) {
    j = json{{"day", d.day}, {"month", d.month}, {"year", d.year }};
}
void from_json(const json& j, CalendarDate& d) {
    j.at("day").get_to(d.day);
    j.at("month").get_to(d.month);
    j.at("year").get_to(d.year);
}

void to_json(json& j, const GameInfo& gi) {
    j = json{
            { "home", gi.m_home }, {"away", gi.m_away},
            { "date", {"day", gi.m_date.day}, {"month", gi.m_date.month}, {"year", gi.m_date.year }},
            { "gid", gi.m_game_id }
    };
}

void from_json(const json& j, Goal& goal) {

    int minutes = 0;
    int seconds = 0;
    int period_number = 0;
    j.at("goal_number").get_to(goal.m_goal_number);
    j.at("player").get_to(goal.m_goal_scorer);
    j.at("team").get_to(goal.m_scoring_team);
    j.at("period").at("number").get_to(period_number);
    j.at("period").at("time").at("minutes").get_to(minutes);
    j.at("period").at("time").at("seconds").get_to(seconds);
}

void from_json(const json& j, GameInfo& gi) {
    j.at("home").get_to(gi.m_home);
    j.at("away").get_to(gi.m_away);
    j.at("date").get_to(gi.m_date);
    j.at("gid").get_to(gi.m_game_id);
}

void from_json(const json& j, Game& game) {
    j.at("game_info").get_to(game.m_game_info);
    j.at("goals").get_to(game.m_goals);
    j.at("year").get_to(d.year);
}