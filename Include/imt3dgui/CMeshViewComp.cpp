/********************************************************************************
**
**	Copyright (C) 2017-2020 ImagingTools GmbH
**
**	This file is part of the ImagingTools SDK.
**
**	This file may be used under the terms of the GNU Lesser
**	General Public License version 2.1 as published by the Free Software
**	Foundation and appearing in the file LicenseLGPL.txt included in the
**	packaging of this file.  Please review the following information to
**	ensure the GNU Lesser General Public License version 2.1 requirements
**	will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
**	If you are unsure which license is appropriate for your use, please
**	contact us at info@imagingtools.de.
**
**
********************************************************************************/


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

		m_gridShape.SetGridValue(0.2);
		m_gridShape.SetCount(15);
		m_gridShape.SetVisible(*m_defaultShowGridAttrPtr);
		scenePtr->AddShapeToScene(&m_gridShape);

		m_axisShape.SetAxisLength(3.0);
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


