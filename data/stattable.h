#ifndef STATTABLE_H
#define STATTABLE_H

#include <QAbstractTableModel>
#include <QObject>

class StatTable : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit StatTable(QList<QString> rowHeaders, QObject* parent=nullptr);
    void populate_data(QString homeTeam, QString awayTeam, std::vector<double> homedata, std::vector<double> awaydata);

    // QAbstractItemModel interface
public:
    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role=Qt::DisplayRole) const override;

private:
    QList<QString> rowHeaders;
    QString aTeam, hTeam;
    std::vector<double> hData;
    std::vector<double> aData;
};

#endif // STATTABLE_H
