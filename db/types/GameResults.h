//
// Created by cx on 2020-03-22.
//

#pragma once

#include <string>
#include <vector>
#include <gametime.h>
#include <valueholder.h>
#include <data/CalendarDate.h>
#include <nlohmann/json.hpp>
#include <data/gamemodel.h>
#include <data/gameinfomodel.h>
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

inline Strength convert_strength(const TeamStrength& str) {
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
    std::string m_home, m_away;
    CalendarDate m_date;

    static GameInfoModel convert(const GameInfo& gi);
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

    static ScoringModel convert(const Goal& goal);
    friend void from_json(const json& j, Goal& g);
};

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

    static GameModel convert(const Game& game);

};