#pragma once

#include "data/gameinfomodel.h"
#include "data/gamemodel.h"
#include "mdbconnection.h"
#include <QBarSet>
#include <vector>
#include <data/trend.h>
#include "iterators.h"

using namespace QtCharts;
using CRange = std::pair<std::vector<GameModel>::const_iterator, std::vector<GameModel>::const_iterator>;

double goalsForAverage(std::list<const GameModel*>& gamesList);
struct FinalResult;

std::string team_scoring(const ScoringModel& score_model);


class TeamStats
{
public:

    using Successes = int;
    using Attempts = int;

    using ResultRatio = std::pair<Successes, Attempts>;

    enum Span : std::size_t {
        Five=5,
        Ten=10,
        Season=81
    };

    enum Division {
        Atlantic,
        Pacific,
        Central,
        Metropolitan
    };

    TeamStats() : m_team{""}, m_gamesPlayed{}, m_begin(m_gamesPlayed.begin()), m_end(m_gamesPlayed.end()), m_cbegin(m_gamesPlayed.cbegin()), m_cend(m_gamesPlayed.cend()) {}
    TeamStats(const TeamStats& copy);
    TeamStats(const std::string& team, std::vector<GameModel>&& games);
    ~TeamStats() {}

    std::vector<GameModel>::iterator begin() const;
    std::vector<GameModel>::iterator end() const;
    std::vector<GameModel>::const_iterator cbegin() const;
    std::vector<GameModel>::const_iterator cend() const;


    std::vector<const GameModel*> lastGames(std::size_t gameCount=5) const;                             // DONE
    // Span analysis
    std::vector<double> goals_for_avg(TeamStats::Span span) const;                                      // DONE
    std::vector<double> goals_against_avg(TeamStats::Span span) const;                                  // DONE
    std::vector<double> shots_for_avg(TeamStats::Span span) const;                                      // DONE
    std::vector<double> shots_against_avg(TeamStats::Span span) const;                                  // DONE

    // Returns season averages before each of last_amount_games games
    std::vector<double> gf_avg_last_x_games(std::size_t last_amount_games) const;
    std::vector<double> ga_avg_last_x_games(std::size_t last_amount_games) const;
    std::vector<double> sf_avg_last_x_games(std::size_t last_amount_games) const;
    std::vector<double> sa_avg_last_x_games(std::size_t last_amount_games) const;

    // Fn should be a lambda, that retrieves the stats we want an average for, over game range 0->(games_played-last_amount_games) ... 0->games_played
    // The lambda has definition (std::string teamName, Iterator first, Iterator end, element_count) where end must *not* be dereferenced, as it can (and will) at
    // one point be m_gamesPlayed.cend(), and dereferencing an end iterator is UB.
    template<typename StatFn>
    std::vector<double> season_avg_last_x_games(std::size_t last_amount_games, StatFn statAvgCalculation) const {
        auto games_played = m_gamesPlayed.size();
        std::vector<double> average{};
        auto end = std::next(m_gamesPlayed.cbegin(), games_played - (last_amount_games - 1));
        for(; end <= m_gamesPlayed.cend(); ++end) {
            double result = statAvgCalculation(m_team, m_gamesPlayed.cbegin(), end, std::distance(m_gamesPlayed.cbegin(), end));
            average.push_back(result);
        }
        return average;
    }

    // Span analysis combined with predicate home/away
    std::vector<double> goals_for_avg_at(GameModel::TeamType type, Span = Span::Season) const;                               // TODO:
    std::vector<double> goals_against_avg_at(GameModel::TeamType type, Span = Span::Season) const;                           // TODO:
    std::vector<double> shots_for_avg_at(GameModel::TeamType type, Span = Span::Season) const;                               // TODO:
    std::vector<double> shots_against_avg_at(GameModel::TeamType type, Span = Span::Season) const;                           // TODO:

    std::vector<double> gf_avg_by_period(TeamStats::Span span, GamePeriod period) const;
    std::vector<double> ga_avg_by_period(TeamStats::Span span, GamePeriod period) const;
    std::vector<double> sf_avg_by_period(TeamStats::Span span, GamePeriod period) const;
    std::vector<double> sa_avg_by_period(TeamStats::Span span, GamePeriod period) const;

    std::vector<double> pdo_game_average(TeamStats::Span span) const;                                   // DONE
    std::vector<double> pdo(TeamStats::Span span) const;                                                // DONE
    std::vector<double> corsi(TeamStats::Span span) const;                                              // TODO: implement
    std::vector<double> corsi_average(TeamStats::Span span) const;                                      // TODO: implement
    ResultRatio wins_against_division(const std::string& division) const;                               // DONE
    ResultRatio losses_against_division(const std::string& div) const;                                  // DONE
    std::vector<ResultRatio> special_teams(Span span, GameModel::SpecialTeamType type) const;           // DONE
    std::vector<double> special_team_efficiency(Span span, GameModel::SpecialTeamType sp_type) const;   // DONE
    std::vector<double> times_in_pp_game_average(Span span) const;                                      // DONE
    std::vector<double> times_in_pk_game_average(Span span) const;                                      // TODO: implement
    CRange games_range_from_back(std::size_t amt) const;
    ResultRatio result_at_home() const;
    ResultRatio result_on_road() const;
    ResultRatio empty_net_letups() const;
    ResultRatio empty_net_scoring() const;                            // DONE
    ResultRatio games_with_pp_goals() const;
    ResultRatio games_with_pk_letups() const;
    std::pair<int, int> wins_after_standing(int team, int opponent);
    std::pair<int, int> wins_after_standing_at_time(const GameInfoModel& gInfo, std::pair<int, int> standing, GameTime game_time);
    std::pair<int, int> loss_after_standing(int team, int opponent);
    std::string team_name() const;
    double goals_made_after_time_avg(const GameTime& time);
    std::size_t games_played() const;
    const std::vector<GameModel>& get_games() const;

    friend std::vector<GameInfoModel> last_opponents_game_info(const TeamStats& ts, int count);

    TrendComparison compare_game_to_trend_stats(const GameModel& game) const;

    friend std::vector<GameModel> get_games_of(std::shared_ptr<MDbConnection> connection, const std::string& team);

    // TODO: This obviously is not returning a vector of ints, but rather a vector of a at this point undefined struct (analysis data)
    std::vector<const GameModel*> games_before(int gameID) const;
private:
    std::string m_team;
    std::vector<GameModel> m_gamesPlayed;
    std::vector<GameModel>::iterator m_begin;
    std::vector<GameModel>::iterator m_end;
    std::vector<GameModel>::const_iterator m_cbegin;;
    std::vector<GameModel>::const_iterator m_cend;
};


struct FinalResult {
    std::pair<int, int> m_current_standing;
    std::pair<int, int> m_final_result;
    GameModel::TeamType m_type;
    bool m_won;
    bool m_OT;


    FinalResult() : m_current_standing{0, 0}, m_final_result{0, 0}, m_type(GameModel::TeamType::AWAY), m_won(false), m_OT(false) {}
    FinalResult(std::pair<int, int> current, std::pair<int, int> final_result, GameModel::TeamType type, bool won, bool inOT) :
        m_current_standing{current}, m_final_result{final_result}, m_type{type}, m_won(won), m_OT(inOT)
    { }
    FinalResult(const FinalResult& copy) : m_current_standing(copy.m_current_standing), m_final_result(copy.m_final_result), m_type(copy.m_type), m_won(copy.m_won), m_OT(copy.m_OT) {}
    ~FinalResult() {}

};
