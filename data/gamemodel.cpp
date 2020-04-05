#include "gamemodel.h"
#include <stdexcept>

// System headers. These can be removed, as pre compiled headers are used in CMakeLists.txt
#include <iostream>
#include <numeric>
#include <algorithm>
#include <utility>

static auto gameModelConstructedDebug = 0;
std::string team_scoring(const ScoringModel& score_model) {
    return teams_map()[score_model.m_scoring_team];
}
std::unordered_map<std::string, std::string> GameModel::g_Teams{};
std::set<std::string> GameModel::g_TeamSet{};
std::unordered_map<std::string, std::string> GameModel::g_TeamDivision{};

std::unordered_map<std::string, std::string>& teams_map(){
    if(GameModel::g_Teams.size() == 0) {
        std::cout << "Global teams map object not constructed. Constructing...\n";
        GameModel::g_Teams["ANA"] = GameModel::g_Teams["DUCKS"] = "Anaheim Ducks";
        GameModel::g_Teams["BOS"] = GameModel::g_Teams["BRUINS"] = "Boston Bruins";
        GameModel::g_Teams["ARI"] = GameModel::g_Teams["COYOTES"] = "Arizona Coyotes";
        GameModel::g_Teams["BUF"] = GameModel::g_Teams["SABRES"] = "Buffalo Sabres";
        GameModel::g_Teams["CAR"] = GameModel::g_Teams["HURRICANES"]  = "Carolina Hurricanes";
        GameModel::g_Teams["CHI"] = GameModel::g_Teams["BLACKHAWKS"] = "Chicago Blackhawks";
        GameModel::g_Teams["CGY"] = GameModel::g_Teams["FLAMES"] = "Calgary Flames";
        GameModel::g_Teams["CBJ"] = GameModel::g_Teams["BLUE JACKETS"] = GameModel::g_Teams["JACKETS"] = "Columbus Blue Jackets";
        GameModel::g_Teams["COL"] = GameModel::g_Teams["AVALANCHE"] = "Colorado Avalanche";
        GameModel::g_Teams["DAL"] = GameModel::g_Teams["STARS"] = "Dallas Stars";
        GameModel::g_Teams["EDM"] = GameModel::g_Teams["OILERS"] = "Edmonton Oilers";
        GameModel::g_Teams["NJD"] = GameModel::g_Teams["DEVILS"] = GameModel::g_Teams["N.J"] = "New Jersey Devils"; // for those places where N.J is used instead of NJD
        GameModel::g_Teams["NYI"] = GameModel::g_Teams["ISLANDERS"] = "New York Islanders";
        GameModel::g_Teams["NYR"] = GameModel::g_Teams["RANGERS"] = "New York Rangers";
        GameModel::g_Teams["MIN"] = GameModel::g_Teams["WILD"] = "Minnesota Wild";
        GameModel::g_Teams["PHI"] = GameModel::g_Teams["FLYERS"] = "Philadelphia Flyers";
        GameModel::g_Teams["DET"] = GameModel::g_Teams["RED WINGS"] = GameModel::g_Teams["WINGS"] = "Detroit Red Wings";
        GameModel::g_Teams["LAK"] = GameModel::g_Teams["L.A"] = GameModel::g_Teams["KINGS"] = "Los Angeles Kings";
        GameModel::g_Teams["OTT"] = GameModel::g_Teams["SENATORS"] = "Ottawa Senators";
        GameModel::g_Teams["MTL"] = GameModel::g_Teams["CANADIENS"] = "Montreal Canadiens";
        GameModel::g_Teams["PIT"] = GameModel::g_Teams["PENGUINS"] = "Pittsburgh Penguins";
        GameModel::g_Teams["WSH"] = GameModel::g_Teams["CAPITALS"] = "Washington Capitals";
        GameModel::g_Teams["STL"] = GameModel::g_Teams["BLUES"] = "St. Louis Blues";
        GameModel::g_Teams["SJS"] = GameModel::g_Teams["S.J"] = GameModel::g_Teams["SHARKS"] = "San Jose Sharks";
        GameModel::g_Teams["TBL"] = GameModel::g_Teams["T.B"] = GameModel::g_Teams["LIGHTNING"] = "Tampa Bay Lightning";
        GameModel::g_Teams["VAN"] = GameModel::g_Teams["CANUCKS"] = "Vancouver Canucks";
        GameModel::g_Teams["TOR"] = GameModel::g_Teams["LEAFS"] = GameModel::g_Teams["MAPLE LEAFS"] = "Toronto Maple Leafs";
        GameModel::g_Teams["WPG"] = GameModel::g_Teams["JETS"] = "Winnipeg Jets";
        GameModel::g_Teams["VGK"] = GameModel::g_Teams["GOLDEN KNIGHTS"] = GameModel::g_Teams["KNIGHTS"] = "Vegas Golden Knights";
        GameModel::g_Teams["FLA"] = GameModel::g_Teams["PANTHERS"] = "Florida Panthers";
        GameModel::g_Teams["NSH"] = GameModel::g_Teams["PREDATORS"] = "Nashville Predators";
        for(const auto& [_, v] : GameModel::g_Teams) {
            GameModel::g_TeamSet.insert(v);
        }
        std::cout << "Done constructing teams map. Team abbreviations Key count: " << GameModel::g_Teams.size() << std::endl;
        std::cout << "Total team count: " << GameModel::g_TeamSet.size() << std::endl;
    }
    return GameModel::g_Teams;
}

std::string GameModel::getDivision(const std::string &team)
{
    auto it = teams_map().find(team);
    if(it == teams_map().end()) {
        auto secondarySearch = GameModel::g_TeamSet.find(team);
        if(secondarySearch == GameModel::g_TeamSet.end()) {
            throw std::runtime_error{"Could not find a team with name (or abbreviation): " + team + "\t In function getDivision(const std::string& team) - [gamemodel.cpp]"};
        }
        return teams_division_map()[*secondarySearch];
    } else {
        return teams_division_map()[it->second];
    }
}

std::string GameModel::getConference(const std::string &team)
{
    auto division = GameModel::getDivision(team);
    if(division == "Metropolitan" || division == "Atlantic") {
        return "Eastern";
    } else {
        return "Western";
    }
}

GameModel::GameModel() :
    m_game_id(0), m_teams{"", ""},
    m_team_won{""}, m_date_played{},
    m_shots_on_goal{{0,0},{0,0}, {0,0}},
    m_final_result{0, 0}, m_face_off_wins{0, 0},
    m_penalty_infraction_minutes{0,0}, m_hits{0,0},
    m_blocked_shots{}, m_give_aways{}, m_goals{}
{
    std::cout << "In GameModel default constructor..." << std::endl;
}

// CONSTRUCTORS 
GameModel::GameModel(int id, TeamNames  teams, std::string  team_won, CalendarDate date_played, const std::vector<IntResults>& shots,
                     IntResults final_result, FloatResults FO, PowerPlay PP, IntResults PIM,
                     IntResults hits, IntResults blocked_shots, IntResults give_aways, std::vector<ScoringModel>&& scoringSummary) noexcept :
    m_game_id(id), m_teams{std::move(teams)},
    m_team_won{std::move(team_won)}, m_date_played{date_played}, m_shots_on_goal{}, m_final_result{std::move(final_result)},
    m_face_off_wins{std::move(std::move(FO))}, m_power_play{std::move(PP)},
    m_penalty_infraction_minutes{std::move(PIM)}, m_hits{std::move(hits)},
    m_blocked_shots{std::move(blocked_shots)}, m_give_aways{std::move(give_aways)}, m_goals{std::move(scoringSummary)}
{
    gameModelConstructedDebug++;
    // std::cout << "In GameModel constructor... Constructed object " << gameModelConstructedDebug << " times" << "\n";
    std::copy(shots.begin(), shots.end(), std::back_inserter(m_shots_on_goal));
    m_init = true;
}

GameModel::GameModel(const GameModel &copy) : m_game_id(copy.m_game_id), m_teams{copy.m_teams},
    m_team_won{copy.m_team_won}, m_date_played{copy.m_date_played}, m_shots_on_goal{copy.m_shots_on_goal}, m_final_result{copy.m_final_result},
    m_face_off_wins{copy.m_face_off_wins}, m_power_play{copy.m_power_play},
    m_penalty_infraction_minutes{copy.m_penalty_infraction_minutes}, m_hits{copy.m_hits},
    m_blocked_shots{copy.m_blocked_shots}, m_give_aways{copy.m_give_aways}, m_goals{}
{
    std::copy(copy.m_goals.cbegin(), copy.m_goals.cend(), std::back_inserter(m_goals));
}

GameModel::GameModel(GameModel &&copy) :
    m_game_id(copy.m_game_id), m_teams{std::move(copy.m_teams)}, m_team_won{std::move(copy.m_team_won)},
    m_date_played{copy.m_date_played}, m_date_played_time_t{copy.m_date_played_time_t},
    m_shots_on_goal{std::move(copy.m_shots_on_goal)},
    m_final_result{std::move(copy.m_final_result)}, m_face_off_wins{std::move(copy.m_face_off_wins)},
    m_power_play{std::move(copy.m_power_play)}, m_penalty_infraction_minutes{std::move(copy.m_penalty_infraction_minutes)},
    m_hits{std::move(copy.m_hits)}, m_blocked_shots{std::move(copy.m_blocked_shots)}, m_give_aways{std::move(copy.m_give_aways)},
    m_goals{std::move(copy.m_goals)}
{
    gameModelConstructedDebug++;
    // std::cout << "In GameModel move constructor... Constructed object " << gameModelConstructedDebug << " times" << "\n";
}

GameModel &GameModel::operator=(const GameModel &rhs) {
    if(this != &rhs) {
        m_game_id = rhs.m_game_id;
        m_teams = rhs.m_teams;
        m_team_won = rhs.m_team_won;
        m_date_played = rhs.m_date_played;
        m_date_played_time_t = {};
        m_shots_on_goal.clear(); // Wow what a difficult bug to find this created, when I forgot to clear the vector.
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

GameModel::TeamType GameModel::get_team_type(const std::string &team) const
{
    if(team == m_teams.home) {
        return TeamType::HOME;
    } else if(team == m_teams.away) {
        return TeamType::AWAY;
    } else {
        throw std::runtime_error{"The team name you provided: '" + team + "' - does not play in this game!"};
    }
}

GameModel::TeamType GameModel::get_opponent_team_type(const std::string &team) const
{
    if(team == m_teams.home)
        return TeamType::AWAY;
    else if(team == m_teams.away)
        return TeamType::HOME;
    else {
        throw std::runtime_error{"The team name you provided: '" + team + "' - does not play in this game!"};
    }
}

std::string GameModel::get_opponent_team_name(const std::string &team) const
{
    try {
        auto team_t = get_opponent_team_type(team);
        if(team_t == TeamType::HOME) return m_teams.away;
        else return m_teams.home;
    } catch(std::exception& e) {
        std::cout << "Exception caught in GameModel::getOpponentTeamName: \n" << e.what() << std::endl;
    }
    return "";
}

SpecialTeams GameModel::get_special_teams(GameModel::TeamType team, GameModel::SpecialTeamType sp_type) const
{
    if(team == TeamType::HOME) {
        if(sp_type == SpecialTeamType::POWERPLAY) {
            return m_power_play.home;
        } else {
            return m_power_play.away;
        }
    } else {
        if(sp_type == SpecialTeamType::POWERPLAY) {
            return m_power_play.away;
        } else {
            return m_power_play.home;
        }
    }
}

std::pair<int, int> GameModel::power_play_stats(GameModel::TeamType t) const
{
    if(t == TeamType::HOME) {
        return to_pair(m_power_play.home);
    } else {
        return to_pair(m_power_play.away);
    }
}

std::pair<int, int> GameModel::penalty_kill_stats(GameModel::TeamType t) const
{
    if(t == TeamType::HOME) {
        auto [goals, attempts] = to_pair(m_power_play.away);
        auto successesDefending = attempts-goals;
        return std::make_pair(successesDefending, attempts);
    } else {
        auto [goals, attempts] = to_pair(m_power_play.home);
        auto successesDefending = attempts-goals;
        return std::make_pair(successesDefending, attempts);
    }
}

bool GameModel::operator==(const GameModel &rhs) {
    return rhs.away_team() == away_team() && rhs.home_team() == home_team() && game_id() == rhs.game_id();
}

double GameModel::pp_efficiency(GameModel::TeamType t) const
{
    if(t == TeamType::AWAY) {
        return m_power_play.away.get_efficiency(SpecialTeams::SPType::PowerPlay);
    } else if(t == TeamType::HOME) {
        return m_power_play.home.get_efficiency(SpecialTeams::SPType::PowerPlay);
    } else {
        return -1;
    }
}

double GameModel::pk_efficiency(GameModel::TeamType t) const
{
    if(t == TeamType::AWAY) {
        return m_power_play.home.get_efficiency(SpecialTeams::SPType::PenaltyKilling); // calculated, based off of opponent's Power play efficiency
    } else if (t == TeamType::HOME) {
        return m_power_play.away.get_efficiency(SpecialTeams::SPType::PenaltyKilling); // calculated, based off of opponent's Power play efficiency
    } else {
        return -1;
    }
}

int GameModel::pp_attempts(GameModel::TeamType t) const
{
    if(t == TeamType::AWAY) {
        return m_power_play.away.m_attempts;
    } else {
        return m_power_play.home.m_attempts;
    }
}

int GameModel::pp_goals(GameModel::TeamType t) const
{
    if(t == TeamType::AWAY) {
        return m_power_play.away.m_goals;
    } else {
        return m_power_play.home.m_goals;
    }
}

int GameModel::pk_attempts(GameModel::TeamType t) const
{
    if(t == TeamType::AWAY) {
        return pp_attempts(TeamType::HOME);
    } else {
        return pp_attempts(TeamType::AWAY);
    }
}

int GameModel::pk_letups(GameModel::TeamType t) const
{
    if(t == TeamType::AWAY) {
        return m_power_play.home.m_goals;
    } else {
        return m_power_play.away.m_goals;
    }
}

double GameModel::shot_efficiency(GameModel::TeamType t) const
{
    return goals_by(t) / shots_by(t);
}

int GameModel::goals_by(GameModel::TeamType t, GoalType gt) const
{
    if(gt == GoalType::Any) {
        if(t == TeamType::AWAY) {
           return m_final_result.away;
        } else {
           return m_final_result.home;
        }
    } else {
        auto allGoals = goals();
        auto gs = 0;
        if(t == TeamType::AWAY) {
            gs = std::accumulate(allGoals.begin(), allGoals.end(), 0, [&](auto& acc, const ScoringModel& g) {
               if(teams_map()[g.scoring_team()] == m_teams.away && from_goal(g) == gt) {
                   return acc + 1;
               }
               return acc;
            });
        } else {
            gs = std::accumulate(allGoals.begin(), allGoals.end(), 0, [&](auto& acc, const ScoringModel& g) {
               if(teams_map()[g.scoring_team()] == m_teams.home && from_goal(g) == gt) {
                   return acc + 1;
               }
               return acc;
            });
        }
        return gs;
    }
}

int GameModel::shots_by(GameModel::TeamType t) const
{
    auto shots = 0;
    if(t == TeamType::AWAY) {
        shots= std::accumulate(m_shots_on_goal.begin(), m_shots_on_goal.end(), 0, [&](auto acc, const auto& period) {
            return acc + period.away;
        });
    } else {
        shots = std::accumulate(m_shots_on_goal.begin(), m_shots_on_goal.end(), 0, [&](auto acc, const auto& period) {
            return acc + period.home;
        });
    }
    return shots;
}

int GameModel::shots_period_by(GamePeriod period, GameModel::TeamType t) const
{
    if(t == TeamType::HOME) {
        return m_shots_on_goal[period].home;
    } else {
        return m_shots_on_goal[period].away;
    }
}

int GameModel::goals_period_by(GamePeriod period, GameModel::TeamType t) const
{
    int goals = 0;
    if(t == TeamType::HOME) {
        for(const auto& goal : m_goals) {
            if(static_cast<std::size_t>(goal.period_number()) == (period+1) && teams_map()[goal.scoring_team()] == m_teams.home) {
                goals++;
            }
        }
    } else {
        for(const auto& goal : m_goals) {
            if(static_cast<std::size_t>(goal.period_number()) == (period+1) && teams_map()[goal.scoring_team()] == m_teams.away) {
                goals++;
            }
        }
    }
    return goals;
}

double GameModel::calculate_pdo(GameModel::TeamType t) const
{
    auto shotefficiency = shot_efficiency(t);
    auto savePercent = 1-shot_efficiency((t == TeamType::AWAY) ? TeamType::HOME : TeamType::AWAY);
    return savePercent + shotefficiency;
}

std::pair<int, int> GameModel::score_after_time(GameTime time) const
{
    auto scoring = get_scoring_progression();
    std::pair<int, int> score{0, 0};
    for(const auto& [away, home, g_time] : scoring) {
        if(time >= g_time) {
            score.first = away;
            score.second = home;
        } else {
            break;
        }
    }
    return score;
}

std::vector<ScoringModel> GameModel::goals() const
{
    return m_goals;
}

std::vector<std::tuple<int, int, GameTime>> GameModel::get_scoring_progression() const
{
    std::vector<std::tuple<int, int, GameTime>> scoringProgress{};
    auto home = 0;
    auto away = 0;

    for(const auto& g : m_goals) {

        if(teams_map()[g.scoring_team()] == m_teams.away) {
            away++;
        } else {
            home++;
        }
        auto standing = std::make_tuple(away, home, g.scoring_time());
        scoringProgress.push_back(standing);
    }
    return scoringProgress;
}

bool GameModel::team_had_goal_difference(GameModel::TeamType type, int diff) const
{
    using PlayedAt = GameModel::TeamType;
    auto goals = get_scoring_progression();
    if(type == PlayedAt::HOME) {
        for(const auto& [away, home, gameTime] : goals) {
            auto _diff = home - away;
            if(_diff == diff) return true;
        }
    } else {
        for(const auto& [away, home, gameTime] : goals) {
            auto _diff = away - home;
            if(_diff == diff) return true;
        }
    }
    return false;
}

std::optional<GameTime> GameModel::team_had_deficit(const std::string& team, int diff) const
{
    auto scoring = get_scoring_progression();
    if(team == m_teams.away) {
        for(const auto& [away, home, time] : scoring) {
            if(home - away == diff)
                return time;
        }
    } else if(team == m_teams.home) {
        for(const auto& [away, home, time] : scoring) {
            if(away - home == diff)
                return time;
        }
    } else {
        throw std::runtime_error{"Team provided to function did not play in this game: " + team};
    }
    return {};
}

std::optional<GameTime> GameModel::team_had_lead(const std::string& team, int diff) const
{
    auto scoring = get_scoring_progression();
    if(team == m_teams.away) {
        for(const auto& [away, home, time] : scoring) {
            if(away - home == diff)
                return time;
        }
    } else {
        for(const auto& [away, home, time] : scoring) {
            if(home - away == diff)
                return time;
        }
    }
    return {};
}

std::optional<GameTime> GameModel::had_standing(const std::string &team_name, int team, int opponent) const
{
    auto scoring = get_scoring_progression();
    // if 0-0 is passed in, we return when the first goal was scored. Otherwise we will not find any 0-0 games when we search for them (since scoring only records goals, not actual standings).
    if(team == 0 && opponent == 0) {
        const auto& [away, home, time] = scoring[0];
        return time;
    }
    if(team_name == m_teams.away) {
        for(const auto& [away, home, time] : scoring) {
            if(team == away && opponent == home) return time;
        }
    } else {
        for(const auto& [away, home, time] : scoring) {
            if(team == home && opponent == away) return time;
        }
    }
        return {};
    }

bool GameModel::had_standing_at_time(const std::string &team_name, int team, int opponent, GameTime live_time) const
{
    auto scores = get_scoring_progression();
    if(team == 0 && opponent == 0) { // find all games that had 0-0 at time live_time, which is true, if live_time is before the time of the first scored goal in game, gtime
        const auto&[away, home, timeOfFirstGoal] = scores[0];
        return live_time <= timeOfFirstGoal;
    }
    if(team_name == m_teams.home) {
        for(auto idx = 0; idx < scores.size(); ++idx) {
            const auto&[away, home, timeOfGoal] = scores[idx];
            if(team == home && opponent == away) { // return true if game had score team-opponent at or before time live_time
                if(timeOfGoal <= live_time && idx+1 == scores.size()) {
                    return true;
                } else {
                    const auto& nextScore = scores[idx+1];
                    const auto&[awayNext, homeNext, timeOfNextGoal] = nextScore;
                    return live_time.in_range_of(timeOfGoal, timeOfNextGoal);
                }
            }            
        }
    } else if(team_name == m_teams.away) {
        for(auto idx = 0; idx < scores.size(); ++idx) {
            const auto&[away, home, timeOfGoal] = scores[idx];
            if(team == away && opponent == home) {
                if(timeOfGoal <= live_time && idx+1 == scores.size()) {
                    return true;
                } else {
                    const auto& nextScore = scores[idx+1];
                    const auto&[awayNext, homeNext, timeOfNextGoal] = nextScore;
                    return live_time.in_range_of(timeOfGoal, timeOfNextGoal);
                }
            }
        }
    } else {
        throw std::runtime_error("Team " + team_name + " does not play in this game");
    }
    return false;
}

std::vector<std::tuple<int, int, GameTime> > GameModel::goals_made_after_time(const GameTime &time) const
{
    std::vector<std::tuple<int, int, GameTime>> filterResults{};
    auto scores = get_scoring_progression();
    std::copy_if(scores.begin(), scores.end(), std::back_inserter(filterResults), [&](const auto& tuple) {
        auto game_time = std::get<GameTime>(tuple);
        return game_time >= time;
    });

    return filterResults;
}

std::unordered_map<std::string, std::string> &teams_division_map()
{
    if(GameModel::g_TeamDivision.size() == 0) {
        GameModel::g_TeamDivision["Anaheim Ducks"] = "Pacific";
        GameModel::g_TeamDivision["Boston Bruins"] = "Atlantic";
        GameModel::g_TeamDivision["Arizona Coyotes"] = "Pacific";
        GameModel::g_TeamDivision["Buffalo Sabres"] = "Atlantic";
        GameModel::g_TeamDivision["Carolina Hurricanes"] = "Metropolitan";
        GameModel::g_TeamDivision["Chicago Blackhawks"] = "Central";
        GameModel::g_TeamDivision["Calgary Flames"] = "Pacific";
        GameModel::g_TeamDivision["Columbus Blue Jackets"] = "Metropolitan";
        GameModel::g_TeamDivision["Colorado Avalanche"] = "Central";
        GameModel::g_TeamDivision["Dallas Stars"] = "Central";
        GameModel::g_TeamDivision["Edmonton Oilers"] = "Pacific";
        GameModel::g_TeamDivision["New Jersey Devils"] = "Metropolitan"; // for those places where N.J is used instead of NJD
        GameModel::g_TeamDivision["New York Islanders"] = "Metropolitan";
        GameModel::g_TeamDivision["New York Rangers"] = "Metropolitan";
        GameModel::g_TeamDivision["Minnesota Wild"] = "Central";
        GameModel::g_TeamDivision["Philadelphia Flyers"] = "Metropolitan";
        GameModel::g_TeamDivision["Detroit Red Wings"] = "Atlantic";
        GameModel::g_TeamDivision["Los Angeles Kings"] = "Pacific";
        GameModel::g_TeamDivision["Ottawa Senators"] = "Atlantic";
        GameModel::g_TeamDivision["Montreal Canadiens"] = "Atlantic";
        GameModel::g_TeamDivision["Pittsburgh Penguins"] = "Metropolitan";
        GameModel::g_TeamDivision["Washington Capitals"] = "Metropolitan";
        GameModel::g_TeamDivision["St. Louis Blues"] = "Central<";
        GameModel::g_TeamDivision["San Jose Sharks"] = "Pacific";
        GameModel::g_TeamDivision["Tampa Bay Lightning"] = "Atlantic";
        GameModel::g_TeamDivision["Vancouver Canucks"] = "Pacific";
        GameModel::g_TeamDivision["Toronto Maple Leafs"] = "Atlantic";
        GameModel::g_TeamDivision["Winnipeg Jets"] = "Central";
        GameModel::g_TeamDivision["Vegas Golden Knights"] = "Pacific";
        GameModel::g_TeamDivision["Florida Panthers"] = "Atlantic";
        GameModel::g_TeamDivision["Nashville Predators"] = "Central";
        return GameModel::g_TeamDivision;
    } else {
        return GameModel::g_TeamDivision;
    }
}

std::string to_string(GamePeriod p)
{
    auto res = "UN INITIALIZED PERIOD TIME NAME";
    switch(p) {
        case First:
            res = "1";
        break;
        case Second:
            res = "2";
        break;
        case Third:
            res = "3";
        break;
        case OT:
            res = "OT";
        break;
    }
    return res;
}

GoalType from_goal(const ScoringModel &g)
{
    switch(g.m_strength) {
        case Strength::SHOOTOUT:
            return GoalType::SO;
        default:
            return GoalType::Game;
    }
}
