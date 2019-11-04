#pragma once


// ACF includes
#include <icomp/CComponentBase.h>
#include <iqtgui/TGuiComponentBase.h>
#include <iqtgui/TGuiObserverWrap.h>
#include <imod/CMultiModelDispatcherBase.h>

// ImtCore includes
#include <imtgui3d/CGridShape.h>
#include <imtgui3d/CPointCloudShape.h>
#include <imtgui3d/CView3dProviderComp.h>
#include <imtgui3d/CAxisShape.h>
#include <imtgui3d/CTargetPointerShape.h>
#include <imtgui3d/CCrossTargetShape.h>
#include <imtview3d/ITargetPointer.h>


namespace imtgui3d
{


class CPointCloudViewComp:
			public iqtgui::TGuiObserverWrap<CView3dProviderComp, CPointCloudShape>,
			public imtview3d::ITargetPointer

{
	Q_OBJECT

public:
	typedef iqtgui::TGuiObserverWrap<CView3dProviderComp, CPointCloudShape> BaseClass;

	I_BEGIN_COMPONENT(CPointCloudViewComp);
		I_REGISTER_INTERFACE(imod::IObserver);
		I_REGISTER_INTERFACE(imod::IModelEditor);
		I_REGISTER_INTERFACE(imtview3d::ITargetPointer);
		I_ASSIGN(m_pointSizeAttrPtr, "PointSize", "Size of the point used for rendering of the 3D-points in the cloud view", true, 2.0);
	I_END_COMPONENT

protected:
	// reimplemented (iqtui::CGuiComponentBase)
	void OnGuiCreated() override;
	void OnGuiDestroyed() override;

	// reimplemented (imtgui3d::ISceneEventHandler)
	void OnShowGrid(bool show) override;
	void OnShowAxis(bool show) override;
	void OnBoxSelection(const QRect& rect, bool clearPreviousSelection) override;
	void OnCircleSelection(const QRect& rect, bool clearPreviousSelection) override;
	void OnClearSelection() override;
	void OnInvertSelection() override;
	void OnDeleteSelection() override;

	// reimplemented (iqtgui::TRestorableGuiWrap)
	void OnRestoreSettings(const QSettings& settings) override;
	void OnSaveSettings(QSettings& settings) const override;

	// reimplemented (imtview3d::ITargetPointer)
	virtual void SetTargetPointer(QVector3D vectorFrom, QVector3D vectorTo) override;

private:
	CGridShape m_gridShape;
	CAxisShape m_axisShape;
	CTargetPointerShape m_targetPointerShape;
	CCrossTargetShape m_crossTargetShape;

	I_ATTR(double, m_pointSizeAttrPtr);
};


} // namespace imtgui3d


