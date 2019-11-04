#include <imt3dgui/CPointCloudViewComp.h>


namespace imt3dgui
{


// protected methods

// reimplemented (iqtui::CComponentBase)

void CPointCloudViewComp::OnGuiCreated()
{
	BaseClass::OnGuiCreated();

	COpenGLWidget* widgetPtr = GetQtWidget();
	Q_ASSERT(widgetPtr != NULL);

	widgetPtr->SetSceneEventHandler(this);

	imt3dview::IScene3d* scenePtr = GetScene();
	if (scenePtr != NULL){
		CPointCloudShape* cloudShapePtr = dynamic_cast<CPointCloudShape*>(this);
		Q_ASSERT(cloudShapePtr != NULL);

		cloudShapePtr->SetPointSize(*m_pointSizeAttrPtr);

		scenePtr->AddShapeToScene(cloudShapePtr);

		m_gridShape.SetGridValue(0.2);
		m_gridShape.SetCount(15);
		scenePtr->AddShapeToScene(&m_gridShape);

		m_axisShape.SetAxisLength(3.0);
		scenePtr->AddShapeToScene(&m_axisShape);

		m_targetPointerShape.SetVisible(false);
		scenePtr->AddShapeToScene(&m_targetPointerShape);

		m_crossTargetShape.SetVisible(false);
		scenePtr->AddShapeToScene(&m_crossTargetShape);
	}
}


void CPointCloudViewComp::OnGuiDestroyed()
{
	COpenGLWidget* widgetPtr = GetQtWidget();
	Q_ASSERT(widgetPtr != NULL);

	imt3dview::IScene3d* scenePtr = GetScene();
	if (scenePtr != NULL){
		scenePtr->RemoveShapeFromScene(&m_gridShape);
		scenePtr->RemoveShapeFromScene(&m_axisShape);
		scenePtr->RemoveShapeFromScene(&m_targetPointerShape);
		scenePtr->RemoveShapeFromScene(&m_crossTargetShape);

		CPointCloudShape* cloudShapePtr = dynamic_cast<CPointCloudShape*>(this);
		Q_ASSERT(cloudShapePtr != NULL);
		scenePtr->RemoveShapeFromScene(cloudShapePtr);
	}

	BaseClass::OnGuiDestroyed();
}


// reimplemented (imt3dgui::ISceneEventHandler

void CPointCloudViewComp::OnShowGrid(bool show)
{
	m_gridShape.SetVisible(show);
}


void CPointCloudViewComp::OnShowAxis(bool show)
{
	m_axisShape.SetVisible(show);
}


void CPointCloudViewComp::OnBoxSelection(const QRect& rect, bool clearPreviousSelection)
{
	CPointCloudShape* cloudShapePtr = dynamic_cast<CPointCloudShape*>(this);
	Q_ASSERT(cloudShapePtr != NULL);

	COpenGLWidget* widgetPtr = GetQtWidget();
	Q_ASSERT(widgetPtr != NULL);

	cloudShapePtr->SetBoxSelection(rect, clearPreviousSelection, widgetPtr->rect());
}


void CPointCloudViewComp::OnCircleSelection(const QRect& rect, bool clearPreviousSelection)
{
	CPointCloudShape* cloudShapePtr = dynamic_cast<CPointCloudShape*>(this);
	Q_ASSERT(cloudShapePtr != NULL);

	COpenGLWidget* widgetPtr = GetQtWidget();
	Q_ASSERT(widgetPtr != NULL);

	cloudShapePtr->SetCircleSelection(rect, clearPreviousSelection, widgetPtr->rect());
}


void CPointCloudViewComp::OnClearSelection()
{
	CPointCloudShape* cloudShapePtr = dynamic_cast<CPointCloudShape*>(this);
	Q_ASSERT(cloudShapePtr != NULL);

	cloudShapePtr->ClearSelection();
}


void CPointCloudViewComp::OnInvertSelection()
{
	CPointCloudShape* cloudShapePtr = dynamic_cast<CPointCloudShape*>(this);
	Q_ASSERT(cloudShapePtr != NULL);

	cloudShapePtr->InvertSelection();
}


void CPointCloudViewComp::OnDeleteSelection()
{
	CPointCloudShape* cloudShapePtr = dynamic_cast<CPointCloudShape*>(this);
	Q_ASSERT(cloudShapePtr != NULL);

	cloudShapePtr->DeleteSelection();
}


// reimplemented (iqtgui::TRestorableGuiWrap)

void CPointCloudViewComp::OnRestoreSettings(const QSettings& settings)
{
	BaseClass::OnRestoreSettings(settings);

	bool showGrid = settings.value("ShowGrid", true).toBool();
	m_gridShape.SetVisible(showGrid);
	GetShowGridCommand().setChecked(showGrid);

	bool showAxis = settings.value("ShowAxis", true).toBool();
	m_axisShape.SetVisible(showAxis);
	GetShowAxisCommand().setChecked(showAxis);
}


void CPointCloudViewComp::OnSaveSettings(QSettings& settings) const
{
	BaseClass::OnSaveSettings(settings);

	settings.setValue("ShowGrid", m_gridShape.IsVisible());
	settings.setValue("ShowAxis", m_axisShape.IsVisible());
}


// reimplemented (imt3dview::ITargetPointer)

void CPointCloudViewComp::SetTargetPointer(QVector3D vectorFrom, QVector3D vectorTo)
{
	bool isExists = (vectorTo - vectorFrom).length() > 0.001;

	m_crossTargetShape.SetVisible(isExists);
	m_targetPointerShape.SetVisible(isExists);

	if (isExists){
		m_targetPointerShape.SetFromPoint(vectorFrom);
		m_targetPointerShape.SetToPoint(vectorTo);
		m_crossTargetShape.SetPosition(vectorTo);
		m_crossTargetShape.SetRotation(QQuaternion::fromDirection(vectorTo - vectorFrom, QVector3D(0.0, 1.0, 0.0)));
	}
}


} // namespace imt3dgui


