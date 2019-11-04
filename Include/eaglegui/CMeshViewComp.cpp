#include <eaglegui/CMeshViewComp.h>


namespace eaglegui
{


// protected methods

// reimplemented (iqtui::CComponentBase)

void CMeshViewComp::OnGuiCreated()
{
	BaseClass::OnGuiCreated();

	CEagleWidget* widgetPtr = GetQtWidget();
	Q_ASSERT(widgetPtr != NULL);

	widgetPtr->SetSceneEventHandler(this);

	eagle::IScene3d* scenePtr = GetScene();
	if (scenePtr != NULL){
		CMeshShape* meshShapePtr = dynamic_cast<CMeshShape*>(this);
		Q_ASSERT(meshShapePtr != NULL);
		scenePtr->AddShapeToScene(meshShapePtr);

		m_gridShape.SetGridValue(0.2);
		m_gridShape.SetCount(15);
		scenePtr->AddShapeToScene(&m_gridShape);

		m_axisShape.SetAxisLength(3.0);
		scenePtr->AddShapeToScene(&m_axisShape);
	}
}


void CMeshViewComp::OnGuiDestroyed()
{
	CEagleWidget* widgetPtr = GetQtWidget();
	Q_ASSERT(widgetPtr != NULL);

	eagle::IScene3d* scenePtr = GetScene();
	if (scenePtr != NULL){
		scenePtr->RemoveShapeFromScene(&m_gridShape);
		scenePtr->RemoveShapeFromScene(&m_axisShape);

		CMeshShape* meshShapePtr = dynamic_cast<CMeshShape*>(this);
		Q_ASSERT(meshShapePtr != NULL);
		scenePtr->RemoveShapeFromScene(meshShapePtr);
	}

	BaseClass::OnGuiDestroyed();
}


// reimplemented (eaglegui::ISceneEventHandler

void CMeshViewComp::OnShowGrid(bool show)
{
	m_gridShape.SetVisible(show);
}


void CMeshViewComp::OnShowAxis(bool show)
{
	m_axisShape.SetVisible(show);
}


// reimplemented (iqtgui::TRestorableGuiWrap)

void CMeshViewComp::OnRestoreSettings(const QSettings& settings)
{
	BaseClass::OnRestoreSettings(settings);

	bool showGrid = settings.value("ShowGrid", true).toBool();
	m_gridShape.SetVisible(showGrid);
	GetShowGridCommand().setChecked(showGrid);

	bool showAxis = settings.value("ShowAxis", true).toBool();
	m_axisShape.SetVisible(showAxis);
	GetShowAxisCommand().setChecked(showAxis);
}


void CMeshViewComp::OnSaveSettings(QSettings& settings) const
{
	BaseClass::OnSaveSettings(settings);

	settings.setValue("ShowGrid", m_gridShape.IsVisible());
	settings.setValue("ShowAxis", m_axisShape.IsVisible());
}


} // namespace eaglegui


