// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
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
		meshShapePtr->SetInfoBoxEnabled(*m_showInfoBoxAttrPtr);

		scenePtr->AddShapeToScene(meshShapePtr);

		m_gridShape.SetGridValue(0.1);
		m_gridShape.SetCount(10);
		m_gridShape.SetPlanePosition(0.0);
		m_gridShape.SetVisible(*m_defaultShowGridAttrPtr);
		scenePtr->AddShapeToScene(&m_gridShape);

		m_axisShape.SetAxisLength(CAxisShape::AT_X, 2.0);
		m_axisShape.SetAxisLength(CAxisShape::AT_Y, 2.0);
		m_axisShape.SetAxisLength(CAxisShape::AT_Z, 2.0);

		m_axisShape.SetVisible(*m_defaultShowAxisAttrPtr);
		scenePtr->AddShapeToScene(&m_axisShape);

		m_rulerShape.SetVisible(false);
		m_rulerShape.SetSlaveShape(meshShapePtr);
		scenePtr->AddShapeToScene(&m_rulerShape);
	}
}


void CMeshViewComp::OnGuiDestroyed()
{
	imt3dview::IScene3d* scenePtr = GetScene();
	if (scenePtr != NULL){
		scenePtr->RemoveShapeFromScene(&m_gridShape);
		scenePtr->RemoveShapeFromScene(&m_axisShape);
		scenePtr->RemoveShapeFromScene(&m_rulerShape);

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


void CMeshViewComp::OnShowRuler(bool show)
{
	m_rulerShape.SetVisible(show);
}


void CMeshViewComp::OnPointSelection(const QPoint& point, bool clearPreviousSelection)
{
	CMeshShape* meshShapePtr = dynamic_cast<CMeshShape*>(this);
	Q_ASSERT(meshShapePtr != NULL);

	meshShapePtr->SetPointSelection(point, clearPreviousSelection);
}


void CMeshViewComp::OnBoxSelection(const QRect& rect, bool clearPreviousSelection)
{
	CMeshShape* meshShapePtr = dynamic_cast<CMeshShape*>(this);
	Q_ASSERT(meshShapePtr != NULL);

	meshShapePtr->SetBoxSelection(rect, clearPreviousSelection);
}


void CMeshViewComp::OnCircleSelection(const QRect& rect, bool clearPreviousSelection)
{
	CMeshShape* meshShapePtr = dynamic_cast<CMeshShape*>(this);
	Q_ASSERT(meshShapePtr != NULL);

	meshShapePtr->SetCircleSelection(rect, clearPreviousSelection);
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


bool CMeshViewComp::OnMousePress(QMouseEvent& e)
{
	return m_rulerShape.OnMousePress(e);
}

bool CMeshViewComp::OnMouseMove(QMouseEvent& e)
{
	return m_rulerShape.OnMouseMove(e);
}


bool CMeshViewComp::OnMouseRelease(QMouseEvent& e)
{
	return m_rulerShape.OnMouseRelease(e);
}


// reimplemented (iqtgui::TRestorableGuiWrap)

void CMeshViewComp::OnRestoreSettings(const QSettings& settings)
{
	BaseClass::OnRestoreSettings(settings);

	if (settings.contains("ShowGrid")){
		bool showGrid = settings.value("ShowGrid").toBool();
		m_gridShape.SetVisible(showGrid);
		GetShowGridCommand().setChecked(showGrid);
	}

	if (settings.contains("ShowAxis")){
		bool showAxis = settings.value("ShowAxis").toBool();
		m_axisShape.SetVisible(showAxis);
		GetShowAxisCommand().setChecked(showAxis);
	}
}


void CMeshViewComp::OnSaveSettings(QSettings& settings) const
{
	BaseClass::OnSaveSettings(settings);

	settings.setValue("ShowGrid", m_gridShape.IsVisible());
	settings.setValue("ShowAxis", m_axisShape.IsVisible());
}


} // namespace imt3dgui


