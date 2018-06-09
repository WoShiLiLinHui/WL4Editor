#ifndef WL4EDITORWINDOW_H
#define WL4EDITORWINDOW_H

#include <QMainWindow>
#include "LevelComponents/Room.h"

namespace Ui {
    class WL4EditorWindow;
}

class WL4EditorWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit WL4EditorWindow(QWidget *parent = 0);
    ~WL4EditorWindow();
    void RenderScreen(LevelComponents::Room *room);

private slots:
    void on_actionOpen_ROM_triggered();

private:
    Ui::WL4EditorWindow *ui;
};

#endif // WL4EDITORWINDOW_H
