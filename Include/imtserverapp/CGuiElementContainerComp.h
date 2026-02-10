// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtserverapp/IGuiElementContainer.h>
#include <imtserverapp/IGuiElementModel.h>


namespace imtserverapp
{


class CGuiElementContainerComp:
			public icomp::CComponentBase,
			virtual public imtserverapp::IGuiElementContainer
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CGuiElementContainerComp);
		I_REGISTER_INTERFACE(istd::IChangeable);
		I_REGISTER_INTERFACE(imtserverapp::IGuiElementContainer);
		I_ASSIGN_MULTI_0(m_guiElementModelsCompPtr, "GuiElementModels", "List of gui element models", false);
		I_ASSIGN(m_slaveGuiElementContainerCompPtr, "SlaveGuiElementContainer", "Slave GUI element container", false, "");
	I_END_COMPONENT;

protected:
	virtual QByteArrayList GetElementIds() const override;
	virtual const imtserverapp::IGuiElementModel* GetGuiElementModel(const QByteArray& elementId) const override;

protected:
	I_MULTIREF(imtserverapp::IGuiElementModel, m_guiElementModelsCompPtr);
	I_REF(imtserverapp::IGuiElementContainer, m_slaveGuiElementContainerCompPtr);
};


} // namespace imtserverapp


