#pragma once
// System headers. These can be removed, as pre compiled headers are used in CMakeLists.txt
#include <vector>

// Other headers
#include "data/gameinfomodel.h"
#include "data/gamemodel.h"
#include "data/gamestatistics.h"
#include <QBarSet>
#include <data/standing.h>
#include <data/trend.h>
#include "iterators.h"
#include "data/gamestatistics.h"

using namespace QtCharts;
using CRange = std::pair<std::vector<GameModel>::const_iterator, std::vector<GameModel>::const_iterator>;
class GameStatistics;
double goalsForAverage(std::list<const GameModel*>& gamesList);
struct FinalResult;

std::string team_scoring(const ScoringModel& score_model);

enum Span : std::size_t {
    Five=5,
    Ten=10,
    Season=81
};

enum DecidedIn {
    DI_REG,
    DI_OT,
    DI_SO
};

struct Result {
    int gameID;
    int team, opponent;
    DecidedIn decidedIn;
    enum PlayedAt {
        Home,
        Away
    } playedAt;
    Result(int gameID, int team, int opponent, Result::PlayedAt playedAt) : gameID(gameID), team(team), opponent(opponent), playedAt(playedAt) {

    }
    Result(Result& c) : gameID(c.gameID), team(c.team), opponent(c.opponent), decidedIn(c.decidedIn) {}
    ~Result() = default;
    /*
     * Pre-condition: The last goal of the game has to be passed to this function. Any other will result in UB.
     */
    void set_decided_in(const ScoringModel& last_goal_made) {
        if(last_goal_made.strength() == Strength::SHOOTOUT) {
            decidedIn = DecidedIn::DI_SO;
            return;
        }
        else if(last_goal_made.period_number() == 4) {
            decidedIn = DecidedIn::DI_OT;
            return;
        } else {
            decidedIn = DecidedIn::DI_REG;
        }
    }
    bool won() const { return team > opponent; }
    bool atHome() const { return playedAt == Home; }
    bool atAway() const { return playedAt == Away; }
};

class TeamStats
{
public:

    using Successes = int;
    using Attempts = int;

    using ResultRatio = std::pair<Successes, Attempts>;

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
    std::vector<double> goals_for_avg(Span span) const;                                      // DONE
    std::vector<double> goals_against_avg(Span span) const;                                  // DONE
    std::vector<double> shots_for_avg(Span span) const;                                      // DONE
    std::vector<double> shots_against_avg(Span span) const;                                  // DONE

    // Returns season averages before each of last_amount_games games
    [[nodiscard]] std::vector<double> gf_avg_last_x_games(std::size_t last_amount_games) const;
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

    std::vector<GameModel> get_games_with_standing(Standing standing) const;
    std::vector<GameModel> get_games_which_had_score(int team, int opponent) const;

    /* requires result type to be default constructable */
    template<typename Result, typename Fn>
    Result accumulate_game_stats(Fn fn) {
        return std::accumulate(m_gamesPlayed.cbegin(), m_gamesPlayed.cend(), Result{}, fn);
    }

    template<typename Fn>
    void for_each_game(Fn fn) {
        for(const auto& game: m_gamesPlayed) fn(game);
    }

    ResultRatio analyze_season_with_standing(Standing standing) const;

    // Span analysis combined with predicate home/away
    std::vector<double> goals_for_avg_at(GameModel::TeamType type, Span = Span::Season) const;                               // TODO:
    std::vector<double> goals_against_avg_at(GameModel::TeamType type, Span = Span::Season) const;                           // TODO:
    std::vector<double> shots_for_avg_at(GameModel::TeamType type, Span = Span::Season) const;                               // TODO:
    std::vector<double> shots_against_avg_at(GameModel::TeamType type, Span = Span::Season) const;                           // TODO:

    std::vector<double> gf_avg_by_period(Span span, GamePeriod period) const;
    std::vector<double> ga_avg_by_period(Span span, GamePeriod period) const;
    std::vector<double> sf_avg_by_period(Span span, GamePeriod period) const;
    std::vector<double> sa_avg_by_period(Span span, GamePeriod period) const;



    [[nodiscard]] std::vector<double> pdo_game_average(Span span) const;                                   // DONE
    [[nodiscard]] std::vector<double> pdo(Span span) const;                                                // DONE
    [[nodiscard]] std::vector<double> corsi(Span span) const;                                              // TODO: implement
    [[nodiscard]] std::vector<double> corsi_average(Span span) const;                                      // TODO: implement
    [[nodiscard]] ResultRatio wins_against_division(const std::string& division) const;                               // DONE
    [[nodiscard]] ResultRatio losses_against_division(const std::string& div) const;                                  // DONE
    [[nodiscard]] std::vector<ResultRatio> special_teams(Span span, GameModel::SpecialTeamType type) const;           // DONE
    [[nodiscard]] std::vector<double> special_team_efficiency(Span span, GameModel::SpecialTeamType sp_type) const;   // DONE
    [[nodiscard]] std::vector<double> times_in_pp_game_average(Span span) const;                                      // DONE
    [[nodiscard]] std::vector<double> times_in_pk_game_average(Span span) const;                                      // TODO: implement
    [[nodiscard]] CRange games_range_from_back(std::size_t amt) const;
    [[nodiscard]] ResultRatio result_at_home() const;
    [[nodiscard]] ResultRatio result_on_road() const;
    [[nodiscard]] ResultRatio empty_net_letups() const;
    [[nodiscard]] ResultRatio empty_net_scoring() const;                            // DONE
    [[nodiscard]] ResultRatio games_with_pp_goals() const;
    [[nodiscard]] ResultRatio games_with_pk_letups() const;
    std::pair<int, int> wins_after_standing(int team, int opponent);
    std::pair<int, int> wins_after_standing_at_time(const GameInfoModel& gInfo, std::pair<int, int> standing, GameTime game_time);
    std::pair<int, int> loss_after_standing(int team, int opponent);
    [[nodiscard]] std::string team_name() const;
    double goals_made_after_time_avg(const GameTime& time);
    [[nodiscard]] std::size_t games_played() const;
    [[nodiscard]] const std::vector<GameModel>& get_games() const;

    friend std::vector<GameInfoModel> last_opponents_game_info(const TeamStats& ts, int count);

    [[nodiscard]] TrendComparison compare_game_to_trend_stats(const GameModel& game) const;
    [[nodiscard]] std::vector<ScoringModel> get_all_goals_for() const;

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
