#pragma once
#include <string>
#include <list>

enum Division {
    Atlantic,
    Pacific,
    Central,
    Metropolitan
};

enum Conference {
    Western,
    Eastern
};

class Team
{
public:
    Team();
    Team(const std::string& m_TeamName);

private:
    std::string m_Name;
    std::list<std::string> m_Abbreviations;
    Division m_Div;
    Conference m_Conf;

    int m_TeamID;

};
