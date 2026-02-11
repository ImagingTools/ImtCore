// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <iattr/IAttributesProvider.h>
#include <iqtgui/TDesignerGuiObserverCompBase.h>

// ImtCore includes
#include <GeneratedFiles/imtgui/ui_CSimpleAttributeManagerViewComp.h>


namespace imtgui
{


class CSimpleAttributeManagerViewComp
	: public iqtgui::TDesignerGuiObserverCompBase<Ui::CSimpleAttributeManagerViewComp, iattr::IAttributesProvider>
{
	Q_OBJECT
public:
	typedef iqtgui::TDesignerGuiObserverCompBase<Ui::CSimpleAttributeManagerViewComp, iattr::IAttributesProvider> BaseClass;

	I_BEGIN_COMPONENT(CSimpleAttributeManagerViewComp)
	I_END_COMPONENT;

protected:
	// reimplemented (iqtgui::TGuiObserverWrap)
	void UpdateGui(const istd::IChangeable::ChangeSet& changeSet) override;
};


} // namespace imtgui
