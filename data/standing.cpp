#include "standing.h"

Standing::Standing(std::string home_team, std::string away_team, int home, int away, GameTime gametime) :
    home(home_team), away(away_team),
    mHome(home), mAway(away),
    mGameTime(gametime)
{

}

int Standing::away_difference()
{
    return mAway - mHome;
}

int Standing::home_difference()
{
    return mHome - mAway;
}

int Standing::difference_of(const std::string &team)
{
    if(team == home) {
        return home_difference();
    } else if(team == away) {
        return away_difference();
    } else {
        throw std::runtime_error("Teams playing in this game: " + home + " vs " + away + " You entered: " + team);
    }
}
