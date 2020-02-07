#pragma once

#include <QObject>
#include <QListView>

class GameListViewItem : public QListViewItem
{
public:
    explicit GameListViewItem(QListView* parent);
    ~GameListViewItem();

};