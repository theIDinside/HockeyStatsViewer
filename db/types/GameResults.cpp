#include "GameResults.h"

GameModel Game::convert(const Game& game) {
    std::vector<ScoringModel> scoringSummary{};
    for(const auto& goal : game.m_goals) {
        auto scoringModel = Goal::convert(goal);
        scoringSummary.push_back(scoringModel);
    }
    GameModel gm{game.m_game_info.m_game_id, TeamNames{game.m_game_info.m_home, game.m_game_info.m_away}, game.m_winning_team, game.m_game_info.m_date, game.m_shots, game.m_final_result, game.m_face_off, game.m_power_play, game.m_penalty_infraction_minutes, game.m_hits, game.m_blocked_shots, game.m_give_aways, std::move(scoringSummary)};
    return gm;        
}

ScoringModel Goal::convert(const Goal& goal) {
    ScoringModel s{goal.m_goal_number, goal.m_time, convert_strength(goal.m_strength), goal.m_scoring_team, goal.m_goal_scorer, goal.m_first_assist, goal.m_second_assist};
    return s;
}

GameInfoModel GameInfo::convert(const GameInfo& game_info) {
    return GameInfoModel{game_info.m_game_id, TeamNames{game_info.m_home, game_info.m_away}, game_info.m_date};
}
