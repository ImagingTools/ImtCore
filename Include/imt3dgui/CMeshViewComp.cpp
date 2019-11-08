#include <imt3dgui/CMeshViewComp.h>


namespace imt3dgui
{


// protected methods

// reimplemented (iqtui::CComponentBase)

void CMeshViewComp::OnGuiCreated()
{
	BaseClass::OnGuiCreated();

	COpenGLWidget* widgetPtr = GetQtWidget();
	Q_ASSERT(widgetPtr != NULL);

	widgetPtr->SetSceneEventHandler(this);

	imt3dview::IScene3d* scenePtr = GetScene();
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
	COpenGLWidget* widgetPtr = GetQtWidget();
	Q_ASSERT(widgetPtr != NULL);

	imt3dview::IScene3d* scenePtr = GetScene();
	if (scenePtr != NULL){
		scenePtr->RemoveShapeFromScene(&m_gridShape);
		scenePtr->RemoveShapeFromScene(&m_axisShape);

		CMeshShape* meshShapePtr = dynamic_cast<CMeshShape*>(this);
		Q_ASSERT(meshShapePtr != NULL);
		scenePtr->RemoveShapeFromScene(meshShapePtr);
	}

	BaseClass::OnGuiDestroyed();
}


// reimplemented (imt3dgui::ISceneEventHandler

void CMeshViewComp::OnShowGrid(bool show)
{
	m_gridShape.SetVisible(show);
}


void CMeshViewComp::OnShowAxis(bool show)
{
	m_axisShape.SetVisible(show);
}


void CMeshViewComp::OnPointSelection(const QPoint& point, bool clearPreviousSelection)
{
	CMeshShape* meshShapePtr = dynamic_cast<CMeshShape*>(this);
	Q_ASSERT(meshShapePtr != NULL);

	COpenGLWidget* widgetPtr = GetQtWidget();
	Q_ASSERT(widgetPtr != NULL);

	meshShapePtr->SetPointSelection(point, clearPreviousSelection, widgetPtr->rect());
}


void CMeshViewComp::OnBoxSelection(const QRect& rect, bool clearPreviousSelection)
{
	CMeshShape* meshShapePtr = dynamic_cast<CMeshShape*>(this);
	Q_ASSERT(meshShapePtr != NULL);

	COpenGLWidget* widgetPtr = GetQtWidget();
	Q_ASSERT(widgetPtr != NULL);

	meshShapePtr->SetBoxSelection(rect, clearPreviousSelection, widgetPtr->rect());
}


void CMeshViewComp::OnCircleSelection(const QRect& rect, bool clearPreviousSelection)
{
	CMeshShape* meshShapePtr = dynamic_cast<CMeshShape*>(this);
	Q_ASSERT(meshShapePtr != NULL);

	COpenGLWidget* widgetPtr = GetQtWidget();
	Q_ASSERT(widgetPtr != NULL);

	meshShapePtr->SetCircleSelection(rect, clearPreviousSelection, widgetPtr->rect());
}


void CMeshViewComp::OnClearSelection()
{
	CMeshShape* meshShapePtr = dynamic_cast<CMeshShape*>(this);
	Q_ASSERT(meshShapePtr != NULL);

	meshShapePtr->ClearSelection();
}


void CMeshViewComp::OnAllSelection()
{
	CMeshShape* meshShapePtr = dynamic_cast<CMeshShape*>(this);
	Q_ASSERT(meshShapePtr != NULL);

	meshShapePtr->AllSelection();
}


void CMeshViewComp::OnInvertSelection()
{
	CMeshShape* meshShapePtr = dynamic_cast<CMeshShape*>(this);
	Q_ASSERT(meshShapePtr != NULL);

	meshShapePtr->InvertSelection();
}


void CMeshViewComp::OnDeleteSelection()
{
	CMeshShape* meshShapePtr = dynamic_cast<CMeshShape*>(this);
	Q_ASSERT(meshShapePtr != NULL);

	meshShapePtr->DeleteSelection();
}


void CMeshViewComp::OnBoxFromSelection()
{
	CMeshShape* meshShapePtr = dynamic_cast<CMeshShape*>(this);
	Q_ASSERT(meshShapePtr != NULL);

	COpenGLWidget* widgetPtr = GetQtWidget();
	Q_ASSERT(widgetPtr != NULL);

	meshShapePtr->BoxFromSelection();
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


} // namespace imt3dgui


