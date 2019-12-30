#include "gamedatapopup.h"
#include "ui_gamedatapopup.h"

GameDataPopup::GameDataPopup(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GameDataPopup)
{
    setWindowFlags(this->windowFlags() | Qt::FramelessWindowHint);
    rowTypes << "GF" << "GA" << "PP" << "PK" << "SF" << "SA" << "Times in PP" << "Times in PK" << "Save %" << "Shot Efficiency";
    ui->setupUi(this);
}

GameDataPopup::~GameDataPopup()
{
    delete ui;
}

void GameDataPopup::update_table_data_show(const std::string &homeTeam, const std::string &awayTeam, std::vector<double> htData, std::vector<double> atData)
{
    home_team = homeTeam.c_str();
    away_team = awayTeam.c_str();
    std::copy(htData.cbegin(), htData.cend(), std::back_inserter(hTeamData));
    std::copy(atData.cbegin(), atData.cend(), std::back_inserter(aTeamData));

    show();
}

void GameDataPopup::hide_gamedata_dialog()
{
    hide();
}
