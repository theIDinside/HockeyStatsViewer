#include "popuptablemodel.h"

PopupTableModel::PopupTableModel(QObject* parent) : QAbstractTableModel(parent)
{
}

int PopupTableModel::rowCount(const QModelIndex &parent) const
{
    return gameStats.size();
}

int PopupTableModel::columnCount(const QModelIndex &parent) const
{
    return 2;
}

QVariant PopupTableModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid()) {
     return QVariant{};
    } else {
        if(role == Qt::DisplayRole) {
            if(auto col = index.column(); col == 0 && index.row() < ColLength) {
                if(auto value = gameStats[index.row()]; value > 0.0 && index.column() > 0) {
                    auto valueString = QString("+%1").arg(value);
                    return valueString;
                }
                return gameStats[index.row()];
            } else {
                if(auto value = trendStats[index.row()]; value > 0.0 && index.column() > 0) {
                    auto valueString = QString("+%1").arg(value);
                    return valueString;
                }
                return trendStats[index.row()];
            }
        }
        if (role == Qt::TextAlignmentRole )
            return Qt::AlignCenter;
    }
    return QVariant{};
}

QVariant PopupTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role == Qt::DisplayRole) {
        if(orientation == Qt::Horizontal) {
            if(section == 0) return QString{"Game Stats"};
            else if(section == 1) return QString{"Trend stats"};
        } else {
            switch(section) {
                case 0: return QString{"PP%"};
                case 1: return QString{"PK%"};
                case 2: return QString{"GF"};
                case 3: return QString{"GA"};
                case 4: return QString{"SF"};
                case 5: return QString{"SA"};
            }
        }
    }
    return QVariant{};
}

void PopupTableModel::populate_data(QString team, ColumnData gameData, ColumnData trendData)
{
    this->team = team;
    this->gameStats = gameData;
    this->trendStats = trendData;

    gameStats[0] *= 100.0;
    gameStats[1] *= 100.0;

    trendStats[0] *= 100.0;
    trendStats[1] *= 100.0;
}
