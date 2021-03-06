#include "gamedatapopup.h"
#include "ui_gamedatapopup.h"
#include <QHeaderView>

GameDataPopup::GameDataPopup(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GameDataPopup)
{
    setWindowFlags(this->windowFlags() | Qt::FramelessWindowHint);
    rowTypes << "GF" << "GA" << "PP%" << "PK%" << "SF" << "SA" << "Times in PP" << "Times in PK" << "Save %" << "Shot Efficiency";
    ui->setupUi(this);
    setToolTip("Left column displays this game's stats.\nRight column shows relative difference between game and trend stats.\nTrend stats are compiled from prior games only.");
}

GameDataPopup::~GameDataPopup()
{
    delete ui;
}

void GameDataPopup::update_table_data_show(const std::string &homeTeam, const std::string &awayTeam, TableData htData, TableData atData)
{
    home_team = homeTeam.c_str();
    away_team = awayTeam.c_str();
    hTeamData = htData;
    aTeamData = atData;

    auto&[hGameStats, hTrendStats] = hTeamData;
    auto&[aGameStats, aTrendStats] = aTeamData;

    ui->hTeamLabel->setText(home_team);
    ui->aTeamLabel->setText(away_team);

    PopupTableModel* homeModel = new PopupTableModel();
    PopupTableModel* awayModel = new PopupTableModel();
    homeModel->populate_data(home_team, hGameStats, hTrendStats);
    awayModel->populate_data(away_team, aGameStats, aTrendStats);

    ui->aTeamLabelView->setModel(awayModel);
    ui->hTeamTableView->setModel(homeModel);

    ui->hTeamTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->aTeamLabelView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

}

void GameDataPopup::hide_gamedata_dialog()
{
    hide();
}
