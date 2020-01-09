#pragma once
#include "gametime.h"
#include <tuple>
#include <optional>
#include <unordered_map>
std::pair<std::optional<std::string>, std::optional<std::string>> convertAssistStrings(const std::string& first, const std::string& second);

enum GoalType {
    Game,
    SO,
    Any
};

enum Strength {
    EVEN,
    EVEN_PENALTY_SHOT,
    PENALTY_SHOT,
    EVEN_EMPTY_NET,
    POWER_PLAY,
    SHORT_HANDED,
    SHORT_HANDED_EMPTY_NET,
    SHORT_HANDED_PENALTY_SHOT,
    POWER_PLAY_EMPTY_NET,
    POWER_PLAY_PENALTY_SHOT,
    SHOOTOUT
};

class ScoringModel
{
public:

    friend std::string team_scoring(const ScoringModel& score_model);
    friend GoalType from_goal(const ScoringModel& g);
    static std::unordered_map<std::string, Strength> StringToEnumMap;
    static Strength convert_strength_string(const std::string& str);

    using Assists = std::pair<std::optional<std::string>, std::optional<std::string>>;
    using Assist = std::optional<std::string>;

private:
    int m_goal_number;
    GameTime m_time;
    Strength m_strength;
    std::string m_scoring_team;
    std::string m_goal_scorer;
    Assist m_firstAssist;
    Assist m_secondAssist;

public:
    ScoringModel();

    ScoringModel(int goal_number, GameTime gameTime, Strength strength, const std::string& scoring_team, const std::string& goal_scorer, Assist first, Assist second);
    ScoringModel(const ScoringModel&);
    ScoringModel(ScoringModel&&);

    ScoringModel& operator=(const ScoringModel& rhs);
    ~ScoringModel() {}

    Strength strength() const;
    int goal_number() const;
    int period_number() const;
    GameTime scoring_time() const;
    std::string scoring_team() const;
    std::string goal_scorer() const;
    Assists assists() const;

    bool is_empty_net() const;
    bool is_unassisted() const;

};
