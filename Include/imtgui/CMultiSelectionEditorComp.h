#pragma once


// ACF includes
#include <iqtgui/TDesignerGuiObserverCompBase.h>
#include <imod/CMultiModelDispatcherBase.h>

// ImtCore includes
#include <imtbase/ISelection.h>
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
	void OnGuiCreated() override;
	void OnGuiRetranslate() override;

private Q_SLOTS:

private:
	I_ATTR(bool, m_horizontalOrientationFlagAttrPtr);
};


} // namespace imtgui


