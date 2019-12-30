#ifndef GAMELISTVIEWITEM_H
#define GAMELISTVIEWITEM_H

#include <QObject>
#include <QListView>

class GameListViewItem : public QListViewItem
{
public:
    explicit GameListViewItem(QListView* parent);
    ~GameListViewItem();

};

#endif // GAMELISTVIEWITEM_H
