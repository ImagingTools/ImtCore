#pragma once


// ACF includes
#include <iqtgui/TDesignerGuiObserverCompBase.h>
#include <idoc/IDocumentMetaInfo.h>

// ImtCore includes
#include <GeneratedFiles/imtgui/ui_CObjectCollectionMetaInfoViewComp.h>


namespace imtgui
{


class CObjectCollectionMetaInfoViewComp:
			public iqtgui::TDesignerGuiObserverCompBase<
						Ui::CObjectCollectionMetaInfoViewComp,
						idoc::IDocumentMetaInfo>
{
	Q_OBJECT
public:
	typedef iqtgui::TDesignerGuiObserverCompBase<
				Ui::CObjectCollectionMetaInfoViewComp,
				idoc::IDocumentMetaInfo> BaseClass;

	I_BEGIN_COMPONENT(CObjectCollectionMetaInfoViewComp);
		I_ASSIGN_MULTI_0(m_ignoreIdsAttrPtr, "IgnoreIds", "List of IDs to be ignored", false);
	I_END_COMPONENT;

protected:
	virtual void FillWidget(QGridLayout* layoutPtr);

	// reimplemented (iqtgui::TGuiObserverWrap)
	virtual void UpdateGui(const istd::IChangeable::ChangeSet& changeSet) override;
	virtual void OnGuiModelDetached() override;

	// reimplemented (iqtgui::CGuiComponentBase)
	virtual void OnGuiCreated() override;
	virtual void OnGuiRetranslate() override;

private:
	I_MULTIATTR(int, m_ignoreIdsAttrPtr);
};


} // namespace imtgui


