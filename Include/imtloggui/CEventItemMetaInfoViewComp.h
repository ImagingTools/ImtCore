// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <iqtgui/TDesignerGuiObserverCompBase.h>
#include <idoc/IDocumentMetaInfo.h>

// ImtCore includes
#include <GeneratedFiles/imtloggui/ui_CEventItemMetaInfoViewComp.h>


namespace imtloggui
{


class CEventItemMetaInfoViewComp:
			public iqtgui::TDesignerGuiObserverCompBase<
						Ui::CEventItemMetaInfoViewComp,
						idoc::IDocumentMetaInfo>
{
	Q_OBJECT
public:
	typedef iqtgui::TDesignerGuiObserverCompBase<
				Ui::CEventItemMetaInfoViewComp,
				idoc::IDocumentMetaInfo> BaseClass;

	I_BEGIN_COMPONENT(CEventItemMetaInfoViewComp);
	I_END_COMPONENT;

protected:
	// reimplemented (iqtgui::TGuiObserverWrap)
	virtual void UpdateGui(const istd::IChangeable::ChangeSet& changeSet) override;
	virtual void OnGuiModelDetached() override;

	// reimplemented (iqtgui::CGuiComponentBase)
	virtual void OnGuiCreated() override;
	virtual void OnGuiRetranslate() override;
};


} // namespace imtloggui


