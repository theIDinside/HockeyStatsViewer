#pragma once
#include "gamemodel.h"
#include <QAbstractTableModel>

class LiveResultModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit LiveResultModel(QObject* parent=nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

private:

};
