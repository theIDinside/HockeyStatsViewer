#pragma once
#include "data/gameinfomodel.h"

#include <QAbstractItemModel>

class GameListItemModel : public QAbstractListModel
{
        Q_OBJECT
public:
    GameListItemModel(std::vector<GameInfoModel>&& gameInfos, QObject* parent=nullptr) : QAbstractListModel(parent), m_gamesToday{std::move(gameInfos)} {}
    // Qt::ItemFlags flags(const QModelIndex& index) const override;
    QVariant data(const QModelIndex& index, int role=Qt::DisplayRole) const override;
    int rowCount(const QModelIndex& parent=QModelIndex()) const override;

    enum Role {
        GetGameID = Qt::UserRole+10,
        GetDate = Qt::UserRole+100,
        GameString = Qt::DisplayRole
    };

private:
    std::vector<GameInfoModel> m_gamesToday; // Core of our model, holds the data. GameInfoModel has a method gameString(), which provides the display data.
};
