#include "scoringmodel.h"

std::unordered_map<std::string, Strength> ScoringModel::StringToEnumMap{};


Strength ScoringModel::convert_strength_string(const std::string &str)
{
    if(StringToEnumMap.size() == 0) {
        StringToEnumMap["Even"] = Strength::EVEN;
        StringToEnumMap["Short Handed"] = Strength::SHORT_HANDED;
        StringToEnumMap["Power Play"] = Strength::POWER_PLAY;
        StringToEnumMap["Even Empty Net"] = Strength::EVEN_EMPTY_NET;
        StringToEnumMap["Even Penalty Shot"] = Strength::EVEN_PENALTY_SHOT;
        StringToEnumMap["Penalty Shot"] = Strength::PENALTY_SHOT;
        StringToEnumMap["Short Handed Penalty Shot"] = Strength::SHORT_HANDED_PENALTY_SHOT;
        StringToEnumMap["Short Handed Empty Net"] = Strength::SHORT_HANDED_EMPTY_NET;
        StringToEnumMap["Power Play Empty Net"] = Strength::POWER_PLAY_EMPTY_NET;
        StringToEnumMap["Power Play Penalty Shot"] = Strength::POWER_PLAY_PENALTY_SHOT;
        StringToEnumMap["Shootout"] = Strength::SHOOTOUT;
    }
    return StringToEnumMap[str];
}
// CONSTRUCTORS
ScoringModel::ScoringModel() noexcept : m_goal_number(-1), m_time{}, m_strength{Strength::EVEN}, m_scoring_team{""}, m_goal_scorer{""}, m_firstAssist{}, m_secondAssist{}
{

}

ScoringModel::ScoringModel(int goalNumber, GameTime gameTime, Strength strength, const std::string& scoringTeam, const std::string& goalScorer, Assist first, Assist second) noexcept :
    m_goal_number(goalNumber),
    m_time(gameTime),
    m_strength(strength),
    m_scoring_team(scoringTeam),
    m_goal_scorer(goalScorer),
    m_firstAssist{first},
    m_secondAssist{second}
{

}

ScoringModel::ScoringModel(const ScoringModel &copy) noexcept :
    m_goal_number(copy.m_goal_number),
    m_time(copy.m_time),
    m_strength(copy.m_strength),
    m_scoring_team(copy.m_scoring_team),
    m_goal_scorer(copy.m_goal_scorer),
    m_firstAssist{copy.m_firstAssist},
    m_secondAssist{copy.m_secondAssist}
{

}

ScoringModel::ScoringModel(ScoringModel &&m) noexcept :
    m_goal_number(m.m_goal_number),
    m_time(m.m_time),
    m_strength(m.m_strength),
    m_scoring_team(std::move(m.m_scoring_team)),
    m_goal_scorer(std::move(m.m_goal_scorer)),
    m_firstAssist{std::move(m.m_firstAssist)},
    m_secondAssist{std::move(m.m_secondAssist)}
{

}

ScoringModel& ScoringModel::operator=(const ScoringModel &rhs) noexcept
{
    if(this != &rhs) {
        this->m_goal_number = rhs.m_goal_number;
        this->m_time = rhs.m_time;
        this->m_strength = rhs.m_strength;
        this->m_scoring_team = rhs.m_scoring_team;
        this->m_goal_scorer = rhs.m_goal_scorer;
        this->m_firstAssist = rhs.m_firstAssist;
        this->m_secondAssist = rhs.m_secondAssist;
    }
    return *this;
}

Strength ScoringModel::strength() const
{
    return m_strength;
}

int ScoringModel::goal_number() const
{
    return m_goal_number;
}

int ScoringModel::period_number() const
{
    return m_time.m_period;
}

GameTime ScoringModel::scoring_time() const
{
    return m_time;
}

std::string ScoringModel::scoring_team() const
{
    return m_scoring_team;
}

std::string ScoringModel::goal_scorer() const
{
    return m_goal_scorer;
}

ScoringModel::Assists ScoringModel::assists() const
{
    return std::make_pair(m_firstAssist, m_secondAssist);
}

std::ostream& operator<<(std::ostream& os, const Strength& str) {
    std::string output;
    switch (str)
    {
        case Strength::EVEN:
        output = "Even";
        break;
        case Strength::EVEN_PENALTY_SHOT:
        output = "Even Penalty Shot";
        break;
        case Strength::PENALTY_SHOT:
        output = "Penalty Shot";
        break;
        case Strength::EVEN_EMPTY_NET:
        output = "Even Empty Net";
        break;
        case Strength::POWER_PLAY:
        output = "Power play";
        break;
        case Strength::SHORT_HANDED:
        output = "Short handed";
        break;
        case Strength::SHORT_HANDED_EMPTY_NET:
        output = "Short handed empty net";
        break;
        case Strength::SHORT_HANDED_PENALTY_SHOT:
        output = "Short handed penalty shot";
        break;
        case Strength::POWER_PLAY_EMPTY_NET:
        output = "Powerplay empty net";
        break;
        case Strength::POWER_PLAY_PENALTY_SHOT:
        output = "Power play penalty shot";
        break;
        case Strength::SHOOTOUT:
        output = "Shootout";
        break;
    }
    os << output;
    return os;
}

bool ScoringModel::is_empty_net() const
{
    if(m_strength == Strength::EVEN_EMPTY_NET || m_strength == Strength::POWER_PLAY_EMPTY_NET || m_strength == Strength::SHORT_HANDED_EMPTY_NET) {
        return true;
    } else {
        return false;
    }
}

bool ScoringModel::is_unassisted() const
{
    return !(m_firstAssist.has_value() && m_secondAssist.has_value());
}

std::pair<std::optional<std::string>, std::optional<std::string> > convertAssistStrings(const std::string &first, const std::string &second)
{
    {
       if(first == "None") {
           return std::make_pair<std::optional<std::string>, std::optional<std::string>>({}, {});
       } else if(second == "None") {
           return std::make_pair<std::optional<std::string>, std::optional<std::string>>({first}, {});
       } else {
           return std::make_pair<std::optional<std::string>, std::optional<std::string>>({first}, {second});
       }
   }
}
