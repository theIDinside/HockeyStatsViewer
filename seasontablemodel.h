#pragma once

#include <QAbstractItemModel>
class SeasonTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    SeasonTableModel(QList<QString> table_headers, QObject* parent=nullptr);


    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

private:
    QList<QString> m_tableHeaders;

};
