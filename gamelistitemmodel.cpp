#include "gamelistitemmodel.h"

// System headers. These can be removed, as pre compiled headers are used in CMakeLists.txt
#include <chrono>
#include <ctime>



QVariant GameListItemModel::data(const QModelIndex &index, int role) const
{
    auto row = static_cast<std::size_t>(index.row());
    if(!index.isValid()) {
        return QVariant();
    }
    if(row >= m_gamesToday.size()) {
        return QVariant();
    }

    if(role == Qt::DisplayRole) { // GameString == Qt::DisplayRole
        return m_gamesToday.at(row).gameString();
    } 
    /* TODO: I don't remember what this is actually for.
    else if(role == GameListItemModel::Role::GetDate) {
        long long t = std::chrono::system_clock::to_time_t(m_gamesToday[row].m_date_played.value());
        return QVariant(t);
    } 
    */
    else if(role == GameListItemModel::Role::GetGameID) {
        return QVariant(m_gamesToday[row].m_game_id);
    } else {
        return QVariant();
    }
}

int GameListItemModel::rowCount(const QModelIndex &parent) const
{
    return static_cast<int>(m_gamesToday.size());
}

