#pragma once


// ACF includes
#include <icomp/CComponentBase.h>
#include <iqtgui/TGuiComponentBase.h>
#include <iqtgui/TGuiObserverWrap.h>
#include <imod/CMultiModelDispatcherBase.h>

// Eagle includes
#include <eaglegui/CGridShape.h>
#include <eaglegui/CPointCloudShape.h>
#include <eaglegui/CEagleViewProviderComp.h>
#include <eaglegui/CAxisShape.h>
#include <eaglegui/CTargetPointerShape.h>
#include <eaglegui/CCrossTargetShape.h>
#include <eagle/ITargetPointer.h>


namespace eaglegui
{


class CPointCloudViewComp:
			public iqtgui::TGuiObserverWrap<CEagleViewProviderComp, CPointCloudShape>,
			public eagle::ITargetPointer

{
	Q_OBJECT

public:
	typedef iqtgui::TGuiObserverWrap<CEagleViewProviderComp, CPointCloudShape> BaseClass;

	I_BEGIN_COMPONENT(CPointCloudViewComp);
		I_REGISTER_INTERFACE(imod::IObserver);
		I_REGISTER_INTERFACE(imod::IModelEditor);
		I_REGISTER_INTERFACE(eagle::ITargetPointer);
		I_ASSIGN(m_pointSizeAttrPtr, "PointSize", "Size of the point used for rendering of the 3D-points in the cloud view", true, 2.0);
	I_END_COMPONENT

protected:
	// reimplemented (iqtui::CGuiComponentBase)
	void OnGuiCreated() override;
	void OnGuiDestroyed() override;

	// reimplemented (eaglegui::ISceneEventHandler)
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

	// reimplemented (eagle::ITargetPointer)
	virtual void SetTargetPointer(QVector3D vectorFrom, QVector3D vectorTo) override;

private:
	CGridShape m_gridShape;
	CAxisShape m_axisShape;
	CTargetPointerShape m_targetPointerShape;
	CCrossTargetShape m_crossTargetShape;

	I_ATTR(double, m_pointSizeAttrPtr);
};


} // namespace eaglegui


