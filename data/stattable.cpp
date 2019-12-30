#include "stattable.h"
#include <QBrush>
#include <QDebug>
StatTable::StatTable(QList<QString> rowHeaders, QObject *parent) : QAbstractTableModel(parent), rowHeaders{rowHeaders}, aTeam{"Away Team"}, hTeam{"Home team"}
{

}

void StatTable::populate_data(QString homeTeam, QString awayTeam, std::vector<double> homedata, std::vector<double> awaydata)
{
    aTeam = awayTeam;
    hTeam = homeTeam;
    hData.clear();
    aData.clear();
    std::copy(homedata.cbegin(), homedata.cend(), std::back_inserter(hData));
    std::copy(awaydata.cbegin(), awaydata.cend(), std::back_inserter(aData));
}
int StatTable::rowCount(const QModelIndex &parent) const
{
    return rowHeaders.length();
}
int StatTable::columnCount(const QModelIndex &parent) const
{
    return 2;
}
QVariant StatTable::data(const QModelIndex &index, int role) const
{
    if(!index.isValid()) {
        return QVariant{};
    } else if(index.isValid()) {
        if(role == Qt::DisplayRole) {
            if(auto col = index.column(); col == 0) { // Away Team Column
                qDebug() << "Away EN Goals: " << aData[index.row()];
                return aData[index.row()];
            } else if(col == 1) { // Home team column
                // qDebug() << "Away EN Goals: " << hData[index.row()];
                return hData[index.row()];
            }
        } else if(role == Qt::BackgroundRole || role == Qt::BackgroundColorRole) {
            auto dataIndex = index.row();
            auto teamIndex = index.column();
            if(aData[dataIndex] == hData[dataIndex]) return QBrush{Qt::lightGray};
            if(teamIndex == 0) { // we have picked away team
                switch(dataIndex) {
                case 0:
                case 2:
                    if(aData[dataIndex] > hData[dataIndex]) return QBrush{Qt::green};
                    else return QBrush{Qt::red};
                case 1:
                case 3:
                    if(aData[dataIndex] < hData[dataIndex]) return QBrush{Qt::green};
                    else return QBrush{Qt::red};
                }
            } else if(teamIndex == 1) {
                switch(dataIndex) {
                case 0:
                case 2:
                    if(aData[dataIndex] < hData[dataIndex]) return QBrush{Qt::green};
                    else return QBrush{Qt::red};
                case 1:
                case 3:
                    if(aData[dataIndex] > hData[dataIndex]) return QBrush{Qt::green};
                    else return QBrush{Qt::red};
                }
            }
        }
    }
    return QVariant{};
}

QVariant StatTable::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role == Qt::DisplayRole && orientation == Qt::Horizontal) { // Display team names in column headers
        if(section == 0) return aTeam;
        else if(section == 1) return hTeam;
    } else if(role == Qt::DisplayRole && orientation == Qt::Vertical && (section < rowHeaders.length())) { // Display row headers
        return rowHeaders[section];
    }
    return QVariant{};
}
