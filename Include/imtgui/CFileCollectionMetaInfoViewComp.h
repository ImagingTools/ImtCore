#pragma once


// ACF includes
#include <iqtgui/TDesignerGuiObserverCompBase.h>
#include <idoc/IDocumentMetaInfo.h>

// ImtCore includes
#include <GeneratedFiles/imtgui/ui_CFileCollectionMetaInfoViewComp.h>


namespace imtgui
{


class CFileCollectionMetaInfoViewComp:
			public iqtgui::TDesignerGuiObserverCompBase<
						Ui::CFileCollectionMetaInfoViewComp,
						idoc::IDocumentMetaInfo>
{
	Q_OBJECT
public:
	typedef iqtgui::TDesignerGuiObserverCompBase<
				Ui::CFileCollectionMetaInfoViewComp,
				idoc::IDocumentMetaInfo> BaseClass;

	I_BEGIN_COMPONENT(CFileCollectionMetaInfoViewComp);
	I_END_COMPONENT;

	CFileCollectionMetaInfoViewComp();

protected:
	// reimplemented (iqtgui::TGuiObserverWrap)
	virtual void UpdateGui(const istd::IChangeable::ChangeSet& changeSet) override;

	// reimplemented (iqtgui::CGuiComponentBase)
	virtual void OnGuiCreated() override;
	virtual void OnGuiRetranslate() override;
 };


} // namespace imtgui


