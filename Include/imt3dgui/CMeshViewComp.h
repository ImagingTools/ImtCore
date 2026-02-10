// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <iqtgui/TGuiObserverWrap.h>

// ImtCore includes
#include <imt3dgui/CGridShape.h>
#include <imt3dgui/CMeshShape.h>
#include <imt3dgui/CRulerShape.h>
#include <imt3dgui/CAxisShape.h>
#include <imt3dgui/CView3dProviderComp.h>


namespace imt3dgui
{


class CMeshViewComp: public iqtgui::TGuiObserverWrap<CView3dProviderComp, CMeshShape>
{
public:
	typedef iqtgui::TGuiObserverWrap<CView3dProviderComp, CMeshShape> BaseClass;

	I_BEGIN_COMPONENT(CMeshViewComp);
		I_REGISTER_INTERFACE(imod::IObserver);
		I_REGISTER_INTERFACE(imod::IModelEditor);
		I_ASSIGN(m_showInfoBoxAttrPtr, "ShowInfoBox", "Show info box if enabled", true, false);
		I_ASSIGN(m_defaultShowAxisAttrPtr, "ShowAxis", "If enabled, the axes are shown", true, true);
		I_ASSIGN(m_defaultShowGridAttrPtr, "ShowGrid", "If enabled, the grid is shown", true, true);
	I_END_COMPONENT

protected:
	// reimplemented (iqtui::CGuiComponentBase)
	virtual void OnGuiCreated() override;
	virtual void OnGuiDestroyed() override;

	// reimplemented (imt3dgui::ISceneEventHandler
	virtual void OnShowGrid(bool show) override;
	virtual void OnShowAxis(bool show) override;
	virtual void OnShowRuler(bool show) override;
	virtual void OnPointSelection(const QPoint& point, bool clearPreviousSelection) override;
	virtual void OnBoxSelection(const QRect& rect, bool clearPreviousSelection) override;
	virtual void OnCircleSelection(const QRect& rect, bool clearPreviousSelection) override;
	virtual void OnClearSelection() override;
	virtual void OnAllSelection() override;
	virtual void OnInvertSelection() override;
	virtual void OnDeleteSelection() override;
	virtual bool OnMousePress(QMouseEvent& e) override;
	virtual bool OnMouseMove(QMouseEvent& e) override;
	virtual bool OnMouseRelease(QMouseEvent& e) override;

	// reimplemented (iqtgui::TRestorableGuiWrap)
	virtual void OnRestoreSettings(const QSettings& settings) override;
	virtual void OnSaveSettings(QSettings& settings) const override;

private:
	CGridShape m_gridShape;
	CAxisShape m_axisShape;
	CRulerShape m_rulerShape;

	I_ATTR(bool, m_showInfoBoxAttrPtr);
	I_ATTR(bool, m_defaultShowAxisAttrPtr);
	I_ATTR(bool, m_defaultShowGridAttrPtr);
};


} // namespace imt3dgui


