
// System headers. These can be removed, as pre compiled headers are used in CMakeLists.txt
#include <algorithm>

// Other headers
#include <list>
#include <QDebug>
#include "teamstats.h"

CRange TeamStats::games_range_from_back(std::size_t amt) const
{
    auto allGamesCount = m_gamesPlayed.size();
    auto begin = std::next(m_gamesPlayed.cbegin(), static_cast<long>(allGamesCount-amt));
    return std::make_pair(begin, m_gamesPlayed.cend());
}

TeamStats::ResultRatio TeamStats::result_at_home() const
{
    ResultRatio gamesAtHome{0, 0};
    auto& [wins, attempts] = gamesAtHome;
    for(const auto& g : m_gamesPlayed) {
        if(g.home_team() == m_team) {
            attempts++;
            if(g.winning_team() == m_team)
                wins++;

        }
    }
    return gamesAtHome;
}

TeamStats::ResultRatio TeamStats::result_on_road() const
{
    ResultRatio gamesOnRoad{0, 0};
    auto& [wins, attempts] = gamesOnRoad;
    for(const auto& g : m_gamesPlayed) {
        if(g.home_team() == m_team) {
            attempts++;
            if(g.winning_team() == m_team)
                wins++;

        }
    }
    return gamesOnRoad;
}

TeamStats::TeamStats(const TeamStats &copy) : m_team{copy.m_team}
{
    std::copy(copy.m_gamesPlayed.begin(), copy.m_gamesPlayed.end(), std::back_inserter(m_gamesPlayed));
    m_begin = m_gamesPlayed.begin();
    m_end = m_gamesPlayed.end();
    m_cbegin = m_gamesPlayed.cbegin();
    m_cend = m_gamesPlayed.cend();

}

TeamStats::TeamStats(const std::string &team, std::vector<GameModel> &&games) : m_team{team}, m_gamesPlayed{std::move(games)}, m_begin(m_gamesPlayed.begin()), m_end(m_gamesPlayed.end()), m_cbegin(m_gamesPlayed.cbegin()), m_cend(m_gamesPlayed.cend())
{
    std::sort(m_gamesPlayed.begin(), m_gamesPlayed.end(), [](GameModel& f, GameModel& l) {
        return f.game_id() < l.game_id();
    });
}

std::vector<GameModel>::iterator TeamStats::begin() const
{
    return m_begin;
}

std::vector<GameModel>::iterator TeamStats::end() const
{
    return m_end;
}

std::vector<GameModel>::const_iterator TeamStats::cbegin() const
{
    return m_cbegin;
}

std::vector<GameModel>::const_iterator TeamStats::cend() const
{
    return m_cend;
}

std::vector<const GameModel *> TeamStats::lastGames(std::size_t gameCount) const
{
    std::vector<const GameModel*> games{};
    if(gameCount >= m_gamesPlayed.size()) {
        for(const auto& g : m_gamesPlayed) {
            games.push_back(&g);
        }
    } else {
        auto end = m_gamesPlayed.crbegin() + static_cast<std::vector<const GameModel*>::difference_type>(gameCount);
        for(auto it = m_gamesPlayed.crbegin(); it < end; it++) {
            games.push_back(&(*it));
            gameCount--;
        }
        std::sort(games.begin(), games.end(), [&](auto f, auto s) {
           return f->game_id() < s->game_id();
        });
    }
    return games;
}

std::vector<double> TeamStats::goals_for_avg(Span span) const
{
    std::vector<double> spanAverage{};
    spanAverage.reserve(span);
    if(m_gamesPlayed.size() < static_cast<std::size_t>(span) * 2 && span != Span::Season) {

    } else if(span == Span::Season) {
        auto goals = std::accumulate(m_gamesPlayed.cbegin(), m_gamesPlayed.cend(), 0, [&](auto& acc, const GameModel& game) {
            return acc + game.goals_by(game.get_team_type(m_team));
        });
        auto average = static_cast<double>(goals) / static_cast<double>(m_gamesPlayed.size());
        spanAverage.push_back(average);
    } else {
        auto [begin, end] = games_range_from_back(span*2 - 1);
        auto _span = static_cast<double>(span);
        iter::win::accumulate_windows(begin, end, static_cast<std::size_t>(span), std::back_inserter(spanAverage), [&](auto& acc, const auto& game) {
            return acc + static_cast<double>(game.goals_by(game.get_team_type(m_team))) / _span;
        });
    }
    return spanAverage;
}

std::vector<double> TeamStats::goals_against_avg(Span span) const
{
    std::vector<double> spanAverage{};
    spanAverage.reserve(span);
    if(m_gamesPlayed.size() < static_cast<std::vector<double>::size_type>(span) * 2 && span != Span::Season) {

    } else if(span == Span::Season) {
        auto games = lastGames(Span::Season);
        auto goalsMade = 0;
        for(const auto& g : m_gamesPlayed) {
            goalsMade += g.goals_by(g.get_opponent_team_type(m_team));
        }
        auto average = static_cast<double>(goalsMade) / static_cast<double>(m_gamesPlayed.size());
        spanAverage.push_back(average);
    } else {
        auto [begin, end] = games_range_from_back(span*2 - 1);
        auto _span = static_cast<double>(span);
        iter::win::accumulate_windows(begin, end, static_cast<std::size_t>(span), std::back_inserter(spanAverage), [&](auto& acc, const auto& game) {
            return acc + static_cast<double>(game.goals_by(game.get_opponent_team_type(m_team))) / _span;
        });
    }
    return spanAverage;
}

std::vector<double> TeamStats::shots_for_avg(Span span) const
{
    std::vector<double> spanAverage{};
    spanAverage.reserve(span);
    if(m_gamesPlayed.size() < static_cast<std::vector<double>::size_type>(span) * 2 && span != Span::Season) {
        // TODO: Handle when season has not progressed long enough to analyze average over span of games
    } else if(span == Span::Season) {
        auto totalShots = std::accumulate(m_gamesPlayed.begin(), m_gamesPlayed.end(), 0, [&](auto i, const auto& game) {
            return i + game.shots_by(game.get_team_type(m_team));
        });
        auto res = static_cast<double>(totalShots) / static_cast<double>(m_gamesPlayed.size());
        spanAverage.push_back(res);
    } else {
        auto [begin, end] = games_range_from_back(span*2 - 1);
        auto _span = static_cast<double>(span);
        iter::win::accumulate_windows(begin, end, static_cast<std::size_t>(span), std::back_inserter(spanAverage), [&](auto& acc, const auto& game) {
            return acc + static_cast<double>(game.shots_by(game.get_team_type(m_team))) / _span;
        });
    }
    return spanAverage;
}

std::vector<double> TeamStats::shots_against_avg(Span span) const
{
    std::vector<double> spanAverage{};
    spanAverage.reserve(span);
    if(m_gamesPlayed.size() < static_cast<std::vector<double>::size_type>(span) * 2 && span != Span::Season) {
        // TODO: Handle when season has not progressed long enough to analyze average over span of games
    } else if (span == Span::Season) {
        auto totalShots = std::accumulate(m_gamesPlayed.begin(), m_gamesPlayed.end(), 0, [&](auto i, const auto& game) {
            return i + game.shots_by(game.get_opponent_team_type(m_team));
        });
        auto res = static_cast<double>(totalShots) / static_cast<double>(m_gamesPlayed.size());
        spanAverage.push_back(res);
    } else {
        auto[begin, end] = games_range_from_back(span * 2 - 1);
        auto _span = static_cast<double>(span);
        iter::win::accumulate_windows(begin, end, static_cast<std::size_t>(span), std::back_inserter(spanAverage), [&](auto& acc, const auto& game){
            return acc + game.shots_by(game.get_opponent_team_type(m_team)) / _span;
        });
    }
    return spanAverage;
}

std::vector<double> TeamStats::gf_avg_last_x_games(std::size_t last_amount_games) const
{
    auto games_played = m_gamesPlayed.size();
    std::vector<double> average{};
    auto end = std::next(m_gamesPlayed.cbegin(), games_played - (last_amount_games - 1));
    auto divisor = (games_played - (last_amount_games - 1));
    for(; end <= m_gamesPlayed.cend(); ++end) {
        double goals_made = std::accumulate(m_gamesPlayed.cbegin(), end, 0.0, [&](auto& acc, const GameModel& game) {
            return acc + (double)game.goals_by(game.get_team_type(m_team));
        });
        double result = goals_made / (double)divisor;
        ++divisor;
        average.push_back(result);
    }
    return average;
}

std::vector<GameModel> TeamStats::get_games_which_had_score(int team, int opponent) const
{
    std::vector<GameModel> results{};

    std::copy_if(m_gamesPlayed.cbegin(), m_gamesPlayed.cend(), std::back_inserter(results), [&](const GameModel& game) {
        return game.had_standing(m_team, team, opponent).has_value();
    });
    return results;
}

std::vector<double> TeamStats::gf_avg_by_period(Span span, GamePeriod period) const
{
    std::vector<double> gfAverageByPeriod{};
    if(m_gamesPlayed.size() < static_cast<std::vector<double>::size_type>(span) * 2 && span != Season) {

    } else if(span == Span::Season) {

    } else {
        auto [begin, end] = games_range_from_back(span*2 - 1);
        auto divisor = static_cast<double>(span);
        iter::win::accumulate_windows(begin, end, span, std::back_inserter(gfAverageByPeriod), [&](auto& acc, const GameModel& game) {
            return acc + static_cast<double>(game.goals_period_by(period, game.get_team_type(m_team))) / divisor;
        });
    }
    return gfAverageByPeriod;
}

std::vector<double> TeamStats::ga_avg_by_period(Span span, GamePeriod period) const
{
    std::vector<double> gaAverageByPeriod{};
    if(m_gamesPlayed.size() < static_cast<std::vector<double>::size_type>(span) * 2 && span != Season) {

    } else if(span == Span::Season) {

    } else {
        auto [begin, end] = games_range_from_back(span*2 - 1);
        auto divisor = static_cast<double>(span);
        iter::win::accumulate_windows(begin, end, span, std::back_inserter(gaAverageByPeriod), [&](auto& acc, const GameModel& game) {
            return acc + static_cast<double>(game.goals_period_by(period, game.get_opponent_team_type(m_team))) / divisor;
        });
    }
    return gaAverageByPeriod;
}

std::vector<double> TeamStats::sf_avg_by_period(Span span, GamePeriod period) const
{
    std::vector<double> sfAverageByPeriod{};
    if(m_gamesPlayed.size() < static_cast<std::vector<double>::size_type>(span) * 2 && span != Season) {

    } else if(span == Span::Season) {

    } else {
        auto [begin, end] = games_range_from_back(span*2 - 1);
        auto divisor = static_cast<double>(span);
        iter::win::accumulate_windows(begin, end, span, std::back_inserter(sfAverageByPeriod), [&](auto& acc, const GameModel& game) {
            return acc + static_cast<double>(game.shots_period_by(period, game.get_team_type(m_team))) / divisor;
        });
    }
    return sfAverageByPeriod;
}

std::vector<double> TeamStats::sa_avg_by_period(Span span, GamePeriod period) const
{
    std::vector<double> saAverageByPeriod{};
    if(m_gamesPlayed.size() < static_cast<std::vector<double>::size_type>(span) * 2 && span != Season) {

    } else if(span == Span::Season) {

    } else {
        auto [begin, end] = games_range_from_back(span*2 - 1);
        auto divisor = static_cast<double>(span);
        iter::win::accumulate_windows(begin, end, span, std::back_inserter(saAverageByPeriod), [&](auto& acc, const GameModel& game) {
            return acc + static_cast<double>(game.shots_period_by(period, game.get_opponent_team_type(m_team))) / divisor;
        });
    }
    return saAverageByPeriod;
}

/*!
 * @brief spanPDOGameAverage - Returns the PDO average over a @a span of games.
 * @param span
 * @return std::vector<double>
 */
std::vector<double> TeamStats::pdo_game_average(Span span) const
{
    std::vector<double> spanPDO{};
    if(m_gamesPlayed.size() < static_cast<std::vector<double>::size_type>(span) * 2 && span != Span::Season) {
        throw std::runtime_error{"There are not enough games to analyze, for a " + std::to_string(static_cast<int>(span)) + " span average."};
    } else if(span == Span::Season) {
        auto SeasonPDO = static_cast<double>(std::accumulate(m_gamesPlayed.begin(), m_gamesPlayed.end(), 0, [&](auto i, const auto& game) {
            return i + game.calculate_pdo(game.get_team_type(m_team));
        })) / static_cast<double>(m_gamesPlayed.size());
        spanPDO.push_back(SeasonPDO);
    } else {
        auto[begin ,end] = games_range_from_back(span*2 - 1);
        std::vector<double> PDOOverSpan {};
        auto _span = static_cast<double>(span);
        iter::win::accumulate_windows(begin, end, span, std::back_inserter(spanPDO), [&](auto acc, const auto& game){
           return acc + static_cast<double>(game.calculate_pdo(game.get_team_type(m_team))) / _span;
        });
    }
    return spanPDO;
}

/**
 * @brief spanPDO - Returns actual PDO over \a span of games, not the average PDO.
 * @param span
 * @return
 */
std::vector<double> TeamStats::pdo(Span span) const
{
    std::vector<double> spanPDO{};
    if(m_gamesPlayed.size() <  static_cast<std::vector<double>::size_type>(span) * 2 && span != Span::Season) {

    } else if(span == Span::Season) {
        auto [shotsFor, shotsAgainst, goalsFor, goalsAgainst] = std::accumulate(m_gamesPlayed.begin(), m_gamesPlayed.end(), std::tuple<int, int, int, int>{0, 0, 0, 0}, [&](auto& acc, const auto& game) {
            auto opponent = game.get_opponent_team_type(m_team);
            auto team = game.get_team_type(m_team);
            auto& [shotsFor, shotsAgainst, goalsFor, goalsAgainst] = acc;
            shotsFor = game.shots_by(team);
            shotsAgainst += game.shots_by(opponent);
            goalsFor += game.goals_by(team);
            goalsAgainst += game.goals_by(opponent);
            return acc;
        });
        auto PDO = (1.0 - static_cast<double>(goalsAgainst) / static_cast<double>(shotsAgainst) + (static_cast<double>(goalsFor) / static_cast<double>(shotsFor)));
        spanPDO.push_back(PDO);
    } else {
        auto[begin ,end] = games_range_from_back(span*2 - 1);
        std::vector<std::tuple<int, int, int, int>> PDOStats{};
        iter::win::accumulate_windows(begin, end, span, std::back_inserter(PDOStats), [&](auto& acc, const auto& game) {
            auto& [shotsFor, shotsAgainst, goalsFor, goalsAgainst] = acc;
            auto opponent = game.get_opponent_team_type(m_team);
            auto team = game.get_team_type(m_team);
            shotsFor += game.shots_by(team);
            shotsAgainst += game.shots_by(opponent);
            goalsFor += game.goals_by(team);
            goalsAgainst += game.goals_by(opponent);
            return acc;
        });
        std::transform(PDOStats.cbegin(), PDOStats.cend(), std::back_inserter(spanPDO), [&](const auto& stat) {
            const auto& [shotsFor, shotsAgainst, goalsFor, goalsAgainst] = stat;
            auto PDO = (1.0 - static_cast<double>(goalsAgainst) / static_cast<double>(shotsAgainst) + (static_cast<double>(goalsFor) / static_cast<double>(shotsFor)));
            return PDO;
        });
    }
    return spanPDO;
}
/*!
\brief winsAgainstDivision Calculates amount of wins out of games played against a specific \a divison.

    Following the current praxis of when returning a pair of ints, to return [successes, perAttempts], this
    being logically easier to reason about, since if we want to find out success ratio in %, we do
    success / perAttempts.
*/
TeamStats::ResultRatio TeamStats::wins_against_division(const std::string& division) const
{
    return std::accumulate(m_gamesPlayed.begin(), m_gamesPlayed.end(), std::pair<int, int>{0, 0}, [&](auto& acc, const auto& game) {
        auto& [won, gamesPlayed] = acc;
        if(game.away_team() == m_team) {
            if(GameModel::getDivision(game.home_team()) == division) {
                gamesPlayed++;
                if(game.winning_team() == m_team) {
                    won++;
                }
            }
        } else if(game.home_team() == m_team) {
            if(GameModel::getDivision(game.away_team()) == division) {
                gamesPlayed++;
                if(game.winning_team() == m_team) {
                    won++;
                }
            }
        }
        return acc;
    });
}

TeamStats::ResultRatio TeamStats::losses_against_division(const std::string &div) const {
    auto divStats = wins_against_division(div);
    auto [wins, attempts] = divStats;
    return std::make_pair((attempts-wins), attempts);
}

std::vector<TeamStats::ResultRatio> TeamStats::special_teams(Span span, GameModel::SpecialTeamType sp_type) const
{
    std::vector<std::pair<int, int>> spanSpecialTeams{};
    if(m_gamesPlayed.size() < static_cast<std::vector<double>::size_type>(span) && span != Span::Season) {

    } else if (span == Span::Season) {
        auto PPSeason = std::accumulate(m_gamesPlayed.cbegin(), m_gamesPlayed.cend(), ResultRatio{0, 0}, [&](auto& acc, const auto& game) {
            auto [goals, attempts] = to_pair(game.get_special_teams(game.get_team_type(m_team), sp_type));
            auto& [goalTotals, attemptTotals] = acc;
            goalTotals += goals;
            attemptTotals += attempts;
            return acc;
        });
        spanSpecialTeams.push_back(PPSeason);
    } else {
        auto[begin ,end] = games_range_from_back(span*2 - 1);
        iter::win::accumulate_windows(begin, end, static_cast<std::size_t>(span), std::back_inserter(spanSpecialTeams), [&](auto& acc, const auto& game) {
            auto [goals, attempts] = to_pair(game.get_special_teams(game.get_team_type(m_team), sp_type));
            auto& [goalTotals, attemptTotals] = acc;
            goalTotals += goals;
            attemptTotals += attempts;
            return acc;
        });
    }
    return spanSpecialTeams;
}

std::vector<double> TeamStats::special_team_efficiency(Span span, GameModel::SpecialTeamType sp_type) const
{
    auto specialTeamsOverSpan = special_teams(span, sp_type);
    std::vector<double> spSpan{};
    std::transform(specialTeamsOverSpan.begin(), specialTeamsOverSpan.end(), std::back_inserter(spSpan), [&](auto item) {
        auto[goals, attempts] = item;
        if(sp_type == GameModel::SpecialTeamType::POWERPLAY)
            return static_cast<double>(goals) / static_cast<double>(attempts);
        else
            return (1.0 - static_cast<double>(goals) / static_cast<double>(attempts));
    });
    return spSpan;
}

std::vector<double> TeamStats::times_in_pp_game_average(Span span) const {
    std::vector<double> spanAverage{};
    if(m_gamesPlayed.size() < static_cast<std::vector<double>::size_type>(span) && span != Span::Season) {

    } else if (span == Span::Season) {

    } else {
        auto[b, e] = games_range_from_back(span*2 - 1);
        iter::win::accumulate_windows(b, e, static_cast<std::size_t>(span), std::back_inserter(spanAverage), [&](auto& acc, const auto& game) {
            return acc + iter::win::avg(game.pp_attempts(game.get_team_type(m_team)), span);
        });
    }
    return spanAverage;
}

std::vector<double> TeamStats::times_in_pk_game_average(Span span) const {
    std::vector<double> spanAverage{};
    if(m_gamesPlayed.size() < static_cast<std::vector<double>::size_type>(span) && span != Span::Season) {

    } else if (span == Span::Season) {

    } else {
        auto[b, e] = games_range_from_back(span*2 - 1);
        iter::win::accumulate_windows(b, e, static_cast<std::size_t>(span), std::back_inserter(spanAverage), [&](auto& acc, const auto& game) {
            return acc + iter::win::avg(game.pk_attempts(game.get_team_type(m_team)), span);
        });
    }
    return spanAverage;
}

TeamStats::ResultRatio TeamStats::empty_net_letups() const
{
    return std::accumulate(cbegin(), cend(), ResultRatio{0,0}, [&](auto& acc, const auto& game) {
        auto& [ENLetUpGames, LostGames] = acc;
        if(game.winning_team() != m_team) {
            ++LostGames;
            for(const auto& goal : game.goals()) {
                if(goal.is_empty_net() && teams_map()[goal.scoring_team()] != m_team) {
                    ENLetUpGames++;
                    break;
                }
            }
        }
        return acc;
    });
}

/**
 * @brief Analyzes games for goals that were made in an empty net.
 * @return std::pair<int, int> where .first is goals made in empty net, and .second amount of wins/attempts.
 */
TeamStats::ResultRatio TeamStats::empty_net_scoring() const{
    return std::accumulate(m_gamesPlayed.begin(), m_gamesPlayed.end(), ResultRatio{0, 0}, [&](auto& acc, const auto& game) {
       auto& [emptyNettingGames, won] = acc;
        if(game.winning_team() == m_team) {
           ++won;
           for(const auto& goal : game.goals()) {
               if(goal.is_empty_net() && goal.scoring_team() == m_team) {
                   ++emptyNettingGames;
                   break; // we don't want to actually count two empty net goals in one game. We are only interested in the ratio of empty net goals/games
               }
           }
        }
       return acc;
    });
}

TeamStats::ResultRatio TeamStats::games_with_pp_goals() const
{
    auto p = std::make_pair(0, m_gamesPlayed.size());
    p.first = std::accumulate(m_gamesPlayed.cbegin(), m_gamesPlayed.cend(), 0, [&](auto& acc, const auto& game) {
       for(const auto& goal : game.goals()) {
        if(goal.scoring_team() == m_team && goal.strength() == Strength::POWER_PLAY) {
            return acc + 1;
        }
       }
       return acc;
    });
    return p;
}

TeamStats::ResultRatio TeamStats::games_with_pk_letups() const
{
    auto p = std::make_pair(0, m_gamesPlayed.size());
    p.first = std::accumulate(m_cbegin, m_cend, 0, [&](auto& acc, const auto& game) {
       for(const auto& goal : game.goals()) {
           if(team_scoring(goal) != m_team && goal.strength() == Strength::POWER_PLAY) return acc + 1;
       }
       return acc;
    });
    return p;
}

std::pair<int, int> TeamStats::wins_after_standing(int team, int opponent)
{
    std::pair<int, int> result{0, 0};
    auto& [winCount, gamesWithStanding] = result;
    auto diff = team - opponent;
    if(diff < 0) {
        std::cout << m_team << " has a deficit of " << diff << std::endl;
        for(const auto& game : m_gamesPlayed) {
            if(game.team_had_deficit(m_team, std::abs(diff))) {
                gamesWithStanding++;
                if(game.winning_team() == m_team) winCount++;
            }
        }
    } else if (diff > 0) {
        std::cout << m_team << " has a lead of " << diff << std::endl;
        for(const auto& game : m_gamesPlayed) {
            if(game.team_had_lead(m_team, std::abs(diff))) {
                gamesWithStanding++;
                if(game.winning_team() == m_team) winCount++;
            }
        }
    } else { // Equal standing
        for(const auto& game : m_gamesPlayed) {
            auto teamType = game.get_team_type(m_team);
            if(teamType == GameModel::TeamType::HOME) {
                if(game.had_standing(m_team, team, opponent));
            } else {

            }
        }
    }
    return result;
}

std::pair<int, int> TeamStats::loss_after_standing(int team, int opponent)
{
    std::pair<int, int> result{0, 0};
    auto& [lossCount, gamesWithStanding] = result;
    auto diff = team - opponent;
    if(diff < 0) {
        std::cout << m_team << " has a deficit of " << diff << std::endl;
        for(const auto& game : m_gamesPlayed) {
            if(game.team_had_deficit(m_team, std::abs(diff))) {
                gamesWithStanding++;
                if(game.winning_team() != m_team) lossCount++;
            }
        }
    } else if (diff > 0) {
        std::cout << m_team << " has a lead of " << diff << std::endl;
        for(const auto& game : m_gamesPlayed) {
            if(game.team_had_lead(m_team, std::abs(diff))) {
                gamesWithStanding++;
                if(game.winning_team() != m_team) lossCount++;
            }
        }
    } else {

    }
    return result;
}

std::string TeamStats::team_name() const
{
    return m_team;
}

double TeamStats::goals_made_after_time_avg(const GameTime &time)
{
    auto span = static_cast<double>(m_gamesPlayed.size());
    auto goals_made_after = std::accumulate(m_gamesPlayed.begin(), m_gamesPlayed.end(), 0, [&](auto& acc, const auto& game) {
       return acc + game.goals_made_after_time(time).size();
    });
    return static_cast<double>(goals_made_after) / span;
}

std::size_t TeamStats::games_played() const
{
    return m_gamesPlayed.size();
                                                                                          }

const std::vector<GameModel> &TeamStats::get_games() const
{
    return m_gamesPlayed;
}

TrendComparison TeamStats::compare_game_to_trend_stats(const GameModel &game) const
{
    assert(game.home_team() == m_team || game.away_team() == m_team);
    TrendComparisonAccumulator comp{m_team};
    auto gameID = game.game_id();
    auto begin = m_gamesPlayed.cbegin();
    auto end = std::find_if(m_gamesPlayed.cbegin(), m_gamesPlayed.cend(), [&](const auto& g) { return g.game_id() == game.game_id();});
    if(end == m_gamesPlayed.cend()) throw std::runtime_error("Could not find a game with id: " + std::to_string(gameID) + " that " + m_team + " played in");
    auto statsUpUntilGame = std::accumulate(begin, end, TrendComparisonAccumulator{m_team}, [&](auto& acc, const auto& g) {
        acc.push_game_stats( GameStatistics::from(m_team, g) );
        return acc;
    });
    auto gamestats = GameStatistics::from(m_team, game);
    return TrendComparison{statsUpUntilGame, gamestats};
}

std::vector<ScoringModel> TeamStats::get_all_goals_for() const
{
    std::vector<ScoringModel> GoalsFor;
    for(const GameModel& game : m_gamesPlayed) {
        for(const ScoringModel& g : game.goals()) {
            if(team_scoring(g) == m_team) {
                GoalsFor.push_back(g);
            }
        }
    }
    return GoalsFor;
}


std::vector<const GameModel *> TeamStats::games_before(int gameID) const
{
    std::vector<const GameModel*> filteredGames{};
    std::transform(m_gamesPlayed.cbegin(), m_gamesPlayed.cend(), std::back_inserter(filteredGames), [&](const auto& game) {
        return &game;
    });
    std::remove_if(filteredGames.begin(), filteredGames.end(), [&](auto ptr) {
        return ptr->game_id() >= gameID;
    });
    return filteredGames;
}

std::vector<GameInfoModel> last_opponents_game_info(const TeamStats& ts, int count)
{
    std::vector<GameInfoModel> opponents{};
    std::transform(ts.m_gamesPlayed.crbegin(), ts.m_gamesPlayed.crbegin()+count, std::back_inserter(opponents), [&](const auto& game) {
        TeamsValueHolder<std::string> teams{game.home_team(), game.away_team()};
        return GameInfoModel{game.game_id(), teams};
    });
    std::reverse(opponents.begin(), opponents.end());
    return opponents;
}
