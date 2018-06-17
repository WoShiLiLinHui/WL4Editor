#include "ChooseLevelDialog.h"
#include "ui_ChooseLevelDialog.h"

ChooseLevelDialog::ChooseLevelDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChooseLevelDialog)
{
    ui->setupUi(this);

    QStringList PassageNameSet; //TODO: read the names directly from the ROM, then we can change the names.
    PassageNameSet<<"Entry Passage"<<"Emerald Passage"<<"Ruby Passage"<< \
                    "Topaz Passage"<<"Sapphire Passage"<<"Golden Passage";
    ui->comboBox_Passage->addItems(PassageNameSet);
    ui->comboBox_Passage->setCurrentIndex(0);
}

ChooseLevelDialog::~ChooseLevelDialog()
{
    delete ui;
}

void ChooseLevelDialog::on_comboBox_Passage_currentTextChanged(const QString &arg1)
{
    QStringList LevelSet1, LevelSet2;
    LevelSet1<<"First Level"<<"Second Level"<<"Third Level"<<"Fourth Level"<<"Boss Level";
    LevelSet2<<"First Level"<<"Boss Level";
    ui->comboBox_Level->clear();
    if(ui->comboBox_Passage->currentIndex()==0 || ui->comboBox_Passage->currentIndex()==5)
        ui->comboBox_Level->addItems(LevelSet2);
    else
        ui->comboBox_Level->addItems(LevelSet1);
}

DialogParams::PassageAndLevelIndex ChooseLevelDialog::GetResult()
{
    DialogParams::PassageAndLevelIndex tmpRetStruct;
    tmpRetStruct._PassageIndex = ui->comboBox_Passage->currentIndex();
    tmpRetStruct._LevelIndex = ui->comboBox_Level->currentIndex();
    if((ui->comboBox_Passage->currentIndex()==0 || ui->comboBox_Passage->currentIndex()==5) && ui->comboBox_Level->currentIndex()==1)
        tmpRetStruct._LevelIndex = 4;
    return tmpRetStruct;
}