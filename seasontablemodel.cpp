#include "seasontablemodel.h"

SeasonTableModel::SeasonTableModel(QList<QString> table_headers, QObject* parent) : QAbstractTableModel(parent), m_tableHeaders(table_headers)
{

}

int SeasonTableModel::rowCount(const QModelIndex &parent) const
{

}

int SeasonTableModel::columnCount(const QModelIndex &parent) const
{

}

QVariant SeasonTableModel::data(const QModelIndex &index, int role) const
{

}

QVariant SeasonTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        return m_tableHeaders[section];
    }
    return QVariant();
}
