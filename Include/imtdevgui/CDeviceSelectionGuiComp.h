// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <iqtgui/TDesignerGuiObserverCompBase.h>
#include <iwidgets/CTextElider.h>

// ImtCore includes
#include <imtbase/ISelection.h>
#include <imtbase/ICollectionInfo.h>
#include <imtbase/TModelUpdateBinder.h>
#include <imtdev/IDeviceStateProvider.h>
#include <GeneratedFiles/imtdevgui/ui_CDeviceSelectionGuiComp.h>


namespace imtdevgui
{


class CDeviceSelectionGuiComp
	: public iqtgui::TDesignerGuiObserverCompBase<Ui::CDeviceSelectionGuiComp, imtbase::ISelection>
{
	Q_OBJECT
public:
	typedef iqtgui::TDesignerGuiObserverCompBase<Ui::CDeviceSelectionGuiComp, imtbase::ISelection>
		BaseClass;

	enum LabelPosition
	{
		/**
			Left from the name field
		*/
		LP_LEFT,

		/**
			On top of the name field
		*/
		LP_TOP
	};

	I_BEGIN_COMPONENT(CDeviceSelectionGuiComp)
		I_ASSIGN(m_measurementDeviceStateCompPtr, "MeasurementDeviceState", "MeasurementDeviceState to highlight current device state in selector", false, "MeasurementDeviceState");
		I_ASSIGN(m_labelTextAttrPtr, "LabelText", "Selection label text", true, "Measurement Device:");
		I_ASSIGN(m_labelPositionAttrPtr, "LabelPosition", "Selection label position.\n0 - Left from the selector,\n1 - On top of the selector", true, LP_LEFT);
	I_END_COMPONENT;

	CDeviceSelectionGuiComp();

Q_SIGNALS:
	void EmitAutoSelect();

protected:
	void OnDeviceListUpdated(const istd::IChangeable::ChangeSet& changeSet, const imtbase::ICollectionInfo* objectPtr);
	void OnDeviceStateUpdated(
		const istd::IChangeable::ChangeSet& changeSet, const imtdev::IDeviceStateProvider* objectPtr);

	void UpdateDeviceStateIcons();

	// reimplemented (iqtgui::TGuiObserverWrap)
	void UpdateGui(const istd::IChangeable::ChangeSet& changeSet) override;

	// reimplemented (imod::IModelEditor)
	void UpdateModel() const override;

	// reimplemented (iqtgui::CGuiComponentBase)
	void OnGuiCreated() override;
	void OnGuiDestroyed() override;
	void OnGuiDesignChanged() override;

protected Q_SLOTS:
	void OnAutoSelect();
	void OnDataChanged();

protected:
	I_REF(imtdev::IDeviceStateProvider, m_measurementDeviceStateCompPtr);
	I_TEXTATTR(m_labelTextAttrPtr);
	I_ATTR(int, m_labelPositionAttrPtr);

	iwidgets::CTextElider m_deviceListElider;

	imtbase::TModelUpdateBinder<imtbase::ICollectionInfo, CDeviceSelectionGuiComp> m_deviceListObserver;
	imtbase::TModelUpdateBinder<imtdev::IDeviceStateProvider, CDeviceSelectionGuiComp>
		m_deviceStateObserver;

	QIcon m_iconConnected;
	QIcon m_iconNotConnected;
};


} // namespace imtdevgui
