#pragma once
// Other headers
#include <QAbstractTableModel>
#include <QObject>
// System headers. These can be removed, as pre compiled headers are used in CMakeLists.txt
#include <array>

constexpr auto ColLength = 7;
using ColumnData = std::array<double, ColLength>;
using TableData = std::pair<ColumnData, ColumnData>;

class PopupTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    PopupTableModel(QObject* parent=nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    void populate_data(QString team, ColumnData gameData, ColumnData trendData);

    QString team;
    ColumnData gameStats;
    ColumnData trendStats;
};
