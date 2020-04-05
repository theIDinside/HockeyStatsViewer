#pragma once

// System headers. These can be removed, as pre compiled headers are used in CMakeLists.txt
#include <string>
#include <ctime>
#include <vector>
#include <chrono>
#include <iostream>
#include <set>
#include <unordered_map>

#include "scoringmodel.h"
#include "standing.h"
// #include "trend.h"
#include "valueholder.h"
#include "CalendarDate.h"

using TeamNames = TeamsValueHolder<std::string>;
using IntResults = TeamsValueHolder<int>;
using FloatResults = TeamsValueHolder<double>;
using PowerPlay = TeamsValueHolder<SpecialTeams>;

std::unordered_map<std::string, std::string>& teams_map();
std::unordered_map<std::string, std::string>& teams_division_map();

enum GamePeriod : std::size_t {
    First = 0,
    Second = 1,
    Third = 2,
    OT = 3
};

std::string to_string(GamePeriod p);
GoalType from_goal(const ScoringModel& g);
/**
 * @brief Our C++ type, corresponding to the GameModel model, defined in our node scrape application. This is a base class. Extend this class for whatever front end you desire.
 */
class GameModel {
public:
    enum TeamType {
        HOME,
        AWAY
    };
    static std::unordered_map<std::string, std::string> g_Teams;
    static std::set<std::string> g_TeamSet;
    static std::unordered_map<std::string, std::string> g_TeamDivision;

    static std::string getDivision(const std::string& team);
    static std::string getConference(const std::string& team);

private:
    bool m_init = false;
public:
    int m_game_id;
    TeamNames m_teams;
    std::string m_team_won;
    CalendarDate m_date_played;
    std::vector<IntResults> m_shots_on_goal;
    IntResults m_final_result;
    FloatResults m_face_off_wins;
    PowerPlay m_power_play;
    IntResults m_penalty_infraction_minutes;
    IntResults m_hits;
    IntResults m_blocked_shots;
    IntResults m_give_aways;
    std::vector<ScoringModel> m_goals;
public:
    enum SpecialTeamType {
        POWERPLAY,
        PENALTYKILL
    };

    GameModel();
    GameModel(int id, TeamNames teams, std::string team_won,
              CalendarDate date_played, const std::vector<IntResults>& shots,
              IntResults final_result, FloatResults FO, PowerPlay PP,
              IntResults PIM, IntResults hits, IntResults blocked_shots, IntResults give_aways, std::vector<ScoringModel>&& scoringSummary) noexcept;

    GameModel(const GameModel& copy);
    GameModel(GameModel&& copy) noexcept;
    GameModel& operator=(const GameModel& rhs);
    ~GameModel() = default;

    TeamType get_team_type(const std::string& team) const;
    TeamType get_opponent_team_type(const std::string& team) const;

    std::string get_opponent_team_name(const std::string& team) const;

    SpecialTeams get_special_teams(TeamType team, SpecialTeamType sp_type) const;

    std::pair<int, int> power_play_stats(TeamType t) const;
    std::pair<int, int> penalty_kill_stats(TeamType t) const;

    bool operator==(const GameModel& rhs);

    [[nodiscard]] double pp_efficiency(TeamType t) const;
    [[nodiscard]] double pk_efficiency(TeamType t) const;
    [[nodiscard]] int pp_attempts(TeamType t) const;
    [[nodiscard]] int pp_goals(TeamType t) const;
    [[nodiscard]] int pk_attempts(TeamType t) const;
    [[nodiscard]] int pk_letups(TeamType t) const;
    [[nodiscard]] double shot_efficiency(TeamType t) const;
    [[nodiscard]] double save_pct(TeamType t) const;
    [[nodiscard]] int goals_by(TeamType t, GoalType gt = GoalType::Any) const;
    [[nodiscard]] int shots_by(TeamType t) const;
    [[nodiscard]] int shots_period_by(GamePeriod period, TeamType t) const;
    [[nodiscard]] int goals_period_by(GamePeriod period, TeamType t) const;
    [[nodiscard]] double calculate_pdo(TeamType t) const;
    [[nodiscard]] constexpr const std::string& winning_team() const { return m_team_won; }
    [[nodiscard]] constexpr const std::string& home_team() const { return m_teams.home; }
    [[nodiscard]] constexpr const std::string& away_team() const { return m_teams.away; }
    [[nodiscard]] constexpr auto game_id() const { return m_game_id; }

    std::pair<int, int> score_after_time(GameTime time) const;
    std::vector<ScoringModel> goals() const;


    /**
     * @brief scoringProgression - Returns the standings in game. Returns a vector of pairs, for example [0,1,timeOfGoal], [0,2,timeOfGoal], [1,2, timeOfGoal] etc, in the format
     * [away, home, game time]: [std::string, std::string, GameTime]. This makes deficit/surplus goals analysis easy.
     * @return std::vector<std::tuple<int, int, GameTime>>
     */
    std::vector< std::tuple<int, int, GameTime> > get_scoring_progression() const;

    bool team_had_goal_difference(TeamType type, int diff) const;
    std::optional<GameTime> team_had_deficit(const std::string& team, int diff) const;
    std::optional<GameTime> team_had_lead(const std::string& team, int diff) const;
    std::optional<GameTime> had_standing(const std::string& team_name, int team, int opponent) const;
    bool had_standing_at_time(const std::string& team_name, int team, int opponent, GameTime time) const;

    std::vector<std::tuple<int, int, GameTime>> goals_made_after_time(const GameTime& time) const;
    std::vector<ScoringModel> goals_made_after_standing(const std::string& team, int teamscore, int opponent) const;
    std::vector<ScoringModel> goals_after_standing_gametime(const std::string& team, int teamscore, int opponent) const;
};
