// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QPointer>
#include <QtWidgets/QCheckBox>

// ACF includes
#include <iqtgui/TDesignerGuiObserverCompBase.h>
#include <imod/CMultiModelDispatcherBase.h>

// ImtCore includes
#include <imtbase/ISelection.h>
#include <imtbase/ICollectionInfo.h>
#include <GeneratedFiles/imtgui/ui_CMultiSelectionEditorComp.h>


namespace imtgui
{


class CMultiSelectionEditorComp:
			public iqtgui::TDesignerGuiObserverCompBase<
						Ui::CMultiSelectionEditorComp, imtbase::ISelection>,
			protected imod::CMultiModelDispatcherBase

{
	Q_OBJECT

public:
	typedef iqtgui::TDesignerGuiObserverCompBase<
				Ui::CMultiSelectionEditorComp, imtbase::ISelection> BaseClass;
	typedef imod::CMultiModelDispatcherBase BaseClass2;

	I_BEGIN_COMPONENT(CMultiSelectionEditorComp);
		I_ASSIGN(m_horizontalOrientationFlagAttrPtr, "HorizontalOrientation", "Arrange selectors in horizontal orientation", true, false);
	I_END_COMPONENT;

	CMultiSelectionEditorComp();

protected:
	// reimplemented (imod::CMultiModelDispatcherBase)
	virtual void OnModelChanged(int modelId, const istd::IChangeable::ChangeSet& changeSet) override;

	// reimplemented (iqtgui::TGuiObserverWrap)
	virtual void OnGuiModelAttached() override;
	virtual void OnGuiModelDetached() override;
	virtual void UpdateGui(const istd::IChangeable::ChangeSet& changeSet) override;

	// reimplemented (iqtgui::CGuiComponentBase)
	virtual void OnGuiCreated() override;
	virtual void OnGuiRetranslate() override;

private Q_SLOTS:
	void OnCheckBoxStateChanged(int checkState);
	void on_SelectAllButton_clicked();
	void on_DeselectAllButton_clicked();
	void on_InvertButton_clicked();
	void on_ExclusiveModeButton_toggled(bool toggled);

private:
	I_ATTR(bool, m_horizontalOrientationFlagAttrPtr);

	QMap<imtbase::ICollectionInfo::Id, QPointer<QCheckBox>> m_checkBoxMap;
};


} // namespace imtgui


