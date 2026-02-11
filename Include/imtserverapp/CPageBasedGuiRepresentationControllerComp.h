// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtserverapp/CGuiElementRepresentationControllerComp.h>


namespace imtserverapp
{


class CPageBasedGuiRepresentationControllerComp: public imtserverapp::CGuiElementRepresentationControllerComp
{
public:
	typedef imtserverapp::CGuiElementRepresentationControllerComp BaseClass;

	I_BEGIN_COMPONENT(CPageBasedGuiRepresentationControllerComp)
	I_END_COMPONENT;

protected:
	// reimplemented (imtserverapp::CGuiElementRepresentationControllerComp)
	virtual bool SetupItemModel(
				const imtserverapp::IGuiElementModel& guiElementModel,
				imtbase::CTreeItemModel& representation,
				int index,
				const iprm::IParamsSet* paramsPtr) const override;
};


} // namespace imtserverapp


