#include "EditModeDockWidget.h"
#include "ui_EditModeDockWidget.h"

#include <WL4EditorWindow.h>
extern WL4EditorWindow *singleton;

/// <summary>
/// Construct the instance of the EditModeDockWidget.
/// </summary>
/// <remarks>
/// Also initializes the enum and index mappings to quickly create EditModeParams structs.
/// </remarks>
/// <param name="parent">
/// The parent QWidget.
/// </param>
EditModeDockWidget::EditModeDockWidget(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::EditModeDockWidget)
{
    ui->setupUi(this);

    // Set up internal data structures
    modeEnums[ui->RadioButton_LayerMode]  = Ui::LayerEditMode;
    modeEnums[ui->RadioButton_EntityMode] = Ui::EntityEditMode;
    modeEnums[ui->RadioButton_DoorMode]   = Ui::DoorEditMode;
    modeEnums[ui->RadioButton_CameraMode] = Ui::CameraEditMode;
    layerIndices[ui->RadioButton_EditOnLayer0] = 0;
    layerIndices[ui->RadioButton_EditOnLayer1] = 1;
    layerIndices[ui->RadioButton_EditOnLayer2] = 2;
    layerIndices[ui->RadioButton_EditOnLayer3] = 3;
    difficultyIndices[ui->RadioButton_NMode] = 0;
    difficultyIndices[ui->RadioButton_HMode] = 1;
    difficultyIndices[ui->RadioButton_SHMode] = 2;
    modeGroup = new QButtonGroup(ui->editModeGroupBox);
    modeGroup->addButton(ui->RadioButton_LayerMode);
    modeGroup->addButton(ui->RadioButton_EntityMode);
    modeGroup->addButton(ui->RadioButton_DoorMode);
    modeGroup->addButton(ui->RadioButton_CameraMode);
    layerGroup = new QButtonGroup(ui->selectedLayerGroupBox);
    layerGroup->addButton(ui->RadioButton_EditOnLayer0);
    layerGroup->addButton(ui->RadioButton_EditOnLayer1);
    layerGroup->addButton(ui->RadioButton_EditOnLayer2);
    layerGroup->addButton(ui->RadioButton_EditOnLayer3);
    difficultyGroup = new QButtonGroup(ui->selectedDifficultyGroupBox);
    difficultyGroup->addButton(ui->RadioButton_NMode);
    difficultyGroup->addButton(ui->RadioButton_HMode);
    difficultyGroup->addButton(ui->RadioButton_SHMode);

    //Set the widget height
    QFontMetrics fontMetrics(ui->CheckBox_AlphaView->font());
    int rowHeight = fontMetrics.lineSpacing();
    ui->dockWidgetContents->setFixedHeight(18 * rowHeight); // TODO: Make this exact, calculate using margins
}

/// <summary>
/// Deconstruct the EditModeDockWidget and clean up its instance objects on the heap.
/// </summary>
EditModeDockWidget::~EditModeDockWidget()
{
    delete ui;
}

void EditModeDockWidget::SetLayersCheckBoxEnabled(int index, bool usable)
{
    switch(index)
    {
        case 0: ui->CheckBox_Layer0View->setEnabled(usable); ui->CheckBox_Layer0View->setChecked(usable); ui->RadioButton_EditOnLayer0->setEnabled(usable); break;
        case 2: ui->CheckBox_Layer2View->setEnabled(usable); ui->CheckBox_Layer2View->setChecked(usable); ui->RadioButton_EditOnLayer2->setEnabled(usable); break;
        case 3: ui->CheckBox_Layer3View->setEnabled(usable); ui->CheckBox_Layer3View->setChecked(usable); ui->RadioButton_EditOnLayer3->setEnabled(usable); break;
        case 7: ui->CheckBox_AlphaView->setEnabled(usable); ui->CheckBox_AlphaView->setChecked(usable); break;
    }
    ui->RadioButton_EditOnLayer1->setChecked(true);
}

bool *EditModeDockWidget::GetLayersVisibilityArray()
{
    bool *LayersVisibilityArray = new bool[8];
    LayersVisibilityArray[0] = ui->CheckBox_Layer0View->isChecked();
    LayersVisibilityArray[1] = ui->CheckBox_Layer1View->isChecked();
    LayersVisibilityArray[2] = ui->CheckBox_Layer2View->isChecked();
    LayersVisibilityArray[3] = ui->CheckBox_Layer3View->isChecked();
    LayersVisibilityArray[4] = ui->CheckBox_EntityView->isChecked();
    LayersVisibilityArray[5] = ui->CheckBox_DoorView->isChecked();
    LayersVisibilityArray[6] = ui->CheckBox_CameraView->isChecked();
    LayersVisibilityArray[7] = ui->CheckBox_AlphaView->isChecked();
    return LayersVisibilityArray;
}

/// <summary>
/// Retrieve the selected edit mode options as a structure.
/// </summary>
/// <return>
/// A struct filled out with the selected edit mode options.
/// </return>
struct Ui::EditModeParams EditModeDockWidget::GetEditModeParams()
{
    QAbstractButton *selectedModeButton = modeGroup->checkedButton();
    QAbstractButton *selectedLayerButton = layerGroup->checkedButton();
    QAbstractButton *selectedDifficultyButton = difficultyGroup->checkedButton();
    struct Ui::EditModeParams params;
    params.editMode = modeEnums[selectedModeButton];
    params.selectedLayer = layerIndices[selectedLayerButton];
    params.layersEnabled[0] = ui->CheckBox_Layer0View->isChecked();
    params.layersEnabled[1] = ui->CheckBox_Layer1View->isChecked();
    params.layersEnabled[2] = ui->CheckBox_Layer2View->isChecked();
    params.layersEnabled[3] = ui->CheckBox_Layer3View->isChecked();
    params.entitiesEnabled = ui->CheckBox_EntityView->isChecked();
    params.doorsEnabled = ui->CheckBox_DoorView->isChecked();
    params.alphaBlendingEnabled = ui->CheckBox_AlphaView->isChecked();
    params.cameraAreasEnabled = ui->CheckBox_CameraView->isChecked();
    params.seleteddifficulty = difficultyIndices[selectedDifficultyButton];
    return params;
}

/// <summary>
/// Redraw the screen when the checkbox is toggled.
/// </summary>
/// <param name="arg1">
/// Unused.
/// </param>
void EditModeDockWidget::on_CheckBox_Layer0View_stateChanged(int arg1)
{
    (void) arg1;
    if(ui->CheckBox_Layer0View->isChecked() && singleton->GetCurrentRoom()->IsLayer0ColorBlendingEnable())
    {

        ui->CheckBox_AlphaView->setChecked(true);
        ui->CheckBox_AlphaView->setEnabled(true);
    }
    else
    {
        ui->CheckBox_AlphaView->setChecked(false);
        ui->CheckBox_AlphaView->setEnabled(false);
    }
    singleton->RenderScreenVisibilityChange();

}

/// <summary>
/// Redraw the screen when the checkbox is toggled.
/// </summary>
/// <param name="arg1">
/// Unused.
/// </param>
void EditModeDockWidget::on_CheckBox_Layer1View_stateChanged(int arg1)
{
    (void) arg1;
    singleton->RenderScreenFull();
}

/// <summary>
/// Redraw the screen when the checkbox is toggled.
/// </summary>
/// <param name="arg1">
/// Unused.
/// </param>
void EditModeDockWidget::on_CheckBox_Layer2View_stateChanged(int arg1)
{
    (void) arg1;
    singleton->RenderScreenFull();
}

/// <summary>
/// Redraw the screen when the checkbox is toggled.
/// </summary>
/// <param name="arg1">
/// Unused.
/// </param>
void EditModeDockWidget::on_CheckBox_Layer3View_stateChanged(int arg1)
{
    (void) arg1;
    singleton->RenderScreenFull();
}

/// <summary>
/// Redraw the screen when the checkbox is toggled.
/// </summary>
/// <param name="arg1">
/// Unused.
/// </param>
void EditModeDockWidget::on_CheckBox_EntityView_stateChanged(int arg1)
{
    (void) arg1;
    singleton->RenderScreenVisibilityChange();
}

/// <summary>
/// Redraw the screen when the checkbox is toggled.
/// </summary>
/// <param name="arg1">
/// Unused.
/// </param>
void EditModeDockWidget::on_CheckBox_DoorView_stateChanged(int arg1)
{
    (void) arg1;
    singleton->RenderScreenVisibilityChange();
}

/// <summary>
/// Redraw the screen when the checkbox is toggled.
/// </summary>
/// <param name="arg1">
/// Unused.
/// </param>
void EditModeDockWidget::on_CheckBox_CameraView_stateChanged(int arg1)
{
    (void) arg1;
    singleton->RenderScreenVisibilityChange();
}

/// <summary>
/// Redraw the screen when the checkbox is toggled.
/// </summary>
/// <param name="arg1">
/// Unused.
/// </param>
void EditModeDockWidget::on_CheckBox_AlphaView_stateChanged(int arg1)
{
    (void) arg1;
    singleton->RenderScreenVisibilityChange(); // TODO this should probably be a full re-render
}

/// <summary>
/// Unselect the selected Door when change to other edit mode.
/// </summary>
/// <param name="checked">
/// show the check state of the RadioButton_DoorMode.
/// </param>
void EditModeDockWidget::on_RadioButton_DoorMode_toggled(bool checked)
{
    if(!checked)
    {
        singleton->Graphicsview_UnselectDoor();
    }
}
