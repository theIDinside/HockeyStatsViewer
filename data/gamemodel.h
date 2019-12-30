#pragma once
#include <string>
#include <ctime>
#include <vector>
#include <chrono>
#include <iostream>
#include <set>
#include <unordered_map>
#include "scoringmodel.h"
#include "trend.h"
#include "valueholder.h"

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
private:
    int m_game_id;
    TeamNames m_teams;
    std::string m_team_won;
    std::chrono::system_clock::time_point m_date_played;
    std::time_t m_date_played_time_t;
    std::vector<IntResults> m_shots_on_goal;
    IntResults m_final_result;
    IntResults m_face_off_wins;
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

    GameModel() :
        m_game_id(0), m_teams{"", ""},
        m_team_won{""}, m_date_played{},
        m_date_played_time_t{}, m_shots_on_goal{},
        m_final_result{0, 0}, m_face_off_wins{0, 0},
        m_penalty_infraction_minutes{0,0}, m_hits{0,0},
        m_blocked_shots{}, m_give_aways{}, m_goals{}
    {
        std::cout << "In GameModel default constructor..." << std::endl;
    }
    GameModel(int id, const TeamNames& teams, const std::string& team_won,
              std::chrono::system_clock::time_point date_played, const std::vector<IntResults>& shots,
              IntResults final_result, IntResults FO, PowerPlay PP,
              IntResults PIM, IntResults hits, IntResults blocked_shots, IntResults give_aways, std::vector<ScoringModel>&& scoringSummary);

    GameModel(const GameModel& copy);
    GameModel(GameModel&& copy);
    GameModel& operator=(const GameModel& rhs) {
        if(this != &rhs) {
            m_game_id = rhs.m_game_id;
            m_teams = rhs.m_teams;
            m_team_won = rhs.m_team_won;
            m_date_played = rhs.m_date_played;
            m_date_played_time_t = {};
            std::copy(rhs.m_shots_on_goal.begin(), rhs.m_shots_on_goal.end(), std::back_inserter(m_shots_on_goal));
            m_final_result = rhs.m_final_result;
            m_face_off_wins = rhs.m_face_off_wins;
            m_power_play = rhs.m_power_play;
            m_penalty_infraction_minutes = rhs.m_penalty_infraction_minutes;
            m_hits = rhs.m_hits;
            m_blocked_shots = rhs.m_blocked_shots;
            m_give_aways = rhs.m_give_aways;
            m_init = true;
            m_goals.clear();
            std::copy(rhs.m_goals.cbegin(), rhs.m_goals.cend(), std::back_inserter(m_goals));
        }
        return *this;
    }
    ~GameModel() {}
    bool is_set() const;
    bool set(int id, TeamNames teams, const std::string& team_won, std::vector<IntResults> shots, IntResults final_result, IntResults FO, PowerPlay PP, IntResults PIM, IntResults hits, IntResults blocked_shots, IntResults give_aways);

    TeamType get_team_type(const std::string& team) const;
    TeamType get_opponent_team_type(const std::string& team) const;

    std::string get_opponent_team_name(const std::string& team) const;

    SpecialTeams get_special_teams(TeamType team, SpecialTeamType sp_type) const;

    std::pair<int, int> power_play_stats(TeamType t) const;
    std::pair<int, int> penalty_kill_stats(TeamType t) const;

    bool operator==(const GameModel& rhs);

    double pp_efficiency(TeamType t) const;
    double pk_efficiency(TeamType t) const;
    int pp_attempts(TeamType t) const;
    int pp_goals(TeamType t) const;
    int pk_attempts(TeamType t) const;
    int pk_letups(TeamType t) const;
    double shot_efficiency(TeamType t) const;
    double save_pct(TeamType t) const;
    int goals_by(TeamType t) const;
    int shots_by(TeamType t) const;

    int shots_period_by(GamePeriod period, TeamType t) const;
    int goals_period_by(GamePeriod period, TeamType t) const;

    double calculate_pdo(TeamType t) const;

    constexpr const std::string& winning_team() const { return m_team_won; }
    constexpr const std::string& home_team() const { return m_teams.home; }
    constexpr const std::string& away_team() const { return m_teams.away; }
    constexpr auto game_id() const { return m_game_id; }

    std::pair<int, int> score_after_time(GameTime time) const;
    std::vector<ScoringModel> goals() const;


    /**
     * @brief scoringProgression - Returns the standings in game. Returns a vector of pairs, for example [0,1,timeOfGoal], [0,2,timeOfGoal], [1,2, timeOfGoal] etc, in the format
     * [away, home, game time]: [std::string, std::string, GameTime]. This makes deficit/surplus goals analysis easy.
     * @return std::vector<std::tuple<int, int, GameTime>>
     */
    std::vector< std::tuple<int, int, GameTime> > get_scoring_progression() const;

    bool team_had_goal_difference(TeamType type, int diff) const;

    bool had_standing(int home, int away, const GameTime& time = GameTime{}) const;

    std::optional<GameTime> team_had_deficit(const std::string& team, int diff) const;
    std::optional<GameTime> team_had_lead(const std::string& team, int diff) const;
    std::optional<GameTime> had_standing(const std::string& team_name, int team, int opponent);

    std::vector<std::tuple<int, int, GameTime>> goals_made_after_time(const GameTime& time) const;
    std::vector<std::tuple<int, int, GameTime>> goals_made_after_standing(std::pair<int, int> score) const;
};
