#pragma once
#include <QDialog>

namespace Ui {
class GameDataPopup;
}

class GameDataPopup : public QDialog
{
    Q_OBJECT

public:
    explicit GameDataPopup(QWidget *parent = nullptr);
    ~GameDataPopup();

public slots:
    // Updates the table data, team names data and shows the "pop up" dialog.
    void update_table_data_show(const std::string& homeTeam, const std::string& awayTeam, std::vector<double> htData, std::vector<double> atData);
    void hide_gamedata_dialog();

private:
    Ui::GameDataPopup *ui;
    std::vector<double> hTeamData;
    std::vector<double> aTeamData;
    QList<QString> rowTypes;
    QString home_team;
    QString away_team;
};
