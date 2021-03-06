#ifndef TILE16DOCKWIDGET_H
#define TILE16DOCKWIDGET_H

#include <QDockWidget>
#include <QGraphicsScene>
#include <QPixmap>
#include <QGraphicsPixmapItem>

#include "WL4Constants.h"
#include "ROMUtils.h"
#include "LevelComponents/Tileset.h"

namespace Ui {
    class Tile16DockWidget;
}

class Tile16DockWidget : public QDockWidget
{
    Q_OBJECT

public:
    explicit Tile16DockWidget(QWidget *parent = 0);
    ~Tile16DockWidget();

private:
    Ui::Tile16DockWidget *ui;
    LevelComponents::Tileset *SelectedTileset = nullptr;
    QGraphicsScene *Tile16MAPScene = nullptr;
    QGraphicsPixmapItem *SelectionBox;
    unsigned short SelectedTile;

public:
    void SetTileInfoText(QString str);
    int SetTileset(int _tilesetIndex);
    LevelComponents::Tileset *GetSelectedTileset() { return SelectedTileset; }
    void SetSelectedTile(unsigned short tile);
    unsigned short GetSelectedTile() { return SelectedTile; }
};

#endif // TILE16DOCKWIDGET_H
