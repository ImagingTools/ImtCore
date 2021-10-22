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
		cloudShapePtr->SetInfoBoxEnabled(*m_showInfoBoxAttrPtr);

		scenePtr->AddShapeToScene(cloudShapePtr);

		m_gridShape.SetGridValue(0.2);
		m_gridShape.SetCount(15);
		m_gridShape.SetVisible(*m_defaultShowGridAttrPtr);
		scenePtr->AddShapeToScene(&m_gridShape);

		m_axisShape.SetAxisLength(3.0);
		m_axisShape.SetVisible(*m_defaultShowAxisAttrPtr);
		scenePtr->AddShapeToScene(&m_axisShape);

		m_rulerShape.SetVisible(false);
		m_rulerShape.SetSlaveShape(cloudShapePtr);
		scenePtr->AddShapeToScene(&m_rulerShape);
	}
}


void CPointCloudViewComp::OnGuiDestroyed()
{
	imt3dview::IScene3d* scenePtr = GetScene();
	if (scenePtr != NULL){
		scenePtr->RemoveShapeFromScene(&m_gridShape);
		scenePtr->RemoveShapeFromScene(&m_axisShape);
		scenePtr->RemoveShapeFromScene(&m_rulerShape);

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


void CPointCloudViewComp::OnShowRuler(bool show)
{
	m_rulerShape.SetVisible(show);
}


void CPointCloudViewComp::OnPointSelection(const QPoint& point, bool clearPreviousSelection)
{
	CPointCloudShape* cloudShapePtr = dynamic_cast<CPointCloudShape*>(this);
	Q_ASSERT(cloudShapePtr != NULL);

	cloudShapePtr->SetPointSelection(point, clearPreviousSelection);
}


void CPointCloudViewComp::OnBoxSelection(const QRect& rect, bool clearPreviousSelection)
{
	CPointCloudShape* cloudShapePtr = dynamic_cast<CPointCloudShape*>(this);
	Q_ASSERT(cloudShapePtr != NULL);

	cloudShapePtr->SetBoxSelection(rect, clearPreviousSelection);
}


void CPointCloudViewComp::OnCircleSelection(const QRect& rect, bool clearPreviousSelection)
{
	CPointCloudShape* cloudShapePtr = dynamic_cast<CPointCloudShape*>(this);
	Q_ASSERT(cloudShapePtr != NULL);

	cloudShapePtr->SetCircleSelection(rect, clearPreviousSelection);
}


void CPointCloudViewComp::OnClearSelection()
{
	CPointCloudShape* cloudShapePtr = dynamic_cast<CPointCloudShape*>(this);
	Q_ASSERT(cloudShapePtr != NULL);

	cloudShapePtr->ClearSelection();
}


void CPointCloudViewComp::OnAllSelection()
{
	CPointCloudShape* cloudShapePtr = dynamic_cast<CPointCloudShape*>(this);
	Q_ASSERT(cloudShapePtr != NULL);

	cloudShapePtr->AllSelection();
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


bool CPointCloudViewComp::OnMousePress(QMouseEvent& e)
{
	return m_rulerShape.OnMousePress(e);
}


bool CPointCloudViewComp::OnMouseMove(QMouseEvent& e)
{
	return m_rulerShape.OnMouseMove(e);
}


bool CPointCloudViewComp::OnMouseRelease(QMouseEvent & e)
{
	return m_rulerShape.OnMouseRelease(e);
}


// reimplemented (iqtgui::TRestorableGuiWrap)

void CPointCloudViewComp::OnRestoreSettings(const QSettings& settings)
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


void CPointCloudViewComp::OnSaveSettings(QSettings& settings) const
{
	BaseClass::OnSaveSettings(settings);

	settings.setValue("ShowGrid", m_gridShape.IsVisible());
	settings.setValue("ShowAxis", m_axisShape.IsVisible());
}


} // namespace imt3dgui


