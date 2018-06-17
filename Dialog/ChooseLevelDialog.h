#ifndef CHOOSELEVELDIALOG_H
#define CHOOSELEVELDIALOG_H

#include <QDialog>

namespace DialogParams
{
    struct PassageAndLevelIndex
    {
        int _PassageIndex;
        int _LevelIndex;
    };

}

namespace Ui {
class ChooseLevelDialog;
}

class ChooseLevelDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ChooseLevelDialog(QWidget *parent = 0);
    ~ChooseLevelDialog();

private slots:
    void on_comboBox_Passage_currentTextChanged(const QString &arg1);

private:
    Ui::ChooseLevelDialog *ui;

public:
    DialogParams::PassageAndLevelIndex GetResult();
};

#endif // CHOOSELEVELDIALOG_H