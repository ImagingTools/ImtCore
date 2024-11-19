#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtrest/IGuiElementContainer.h>
#include <imtrest/IGuiElementModel.h>


namespace imtrest
{


class CGuiElementContainerComp:
			public icomp::CComponentBase,
			virtual public imtrest::IGuiElementContainer
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CGuiElementContainerComp);
		I_REGISTER_INTERFACE(istd::IChangeable);
		I_REGISTER_INTERFACE(imtrest::IGuiElementContainer);
		I_ASSIGN_MULTI_0(m_guiElementModelsCompPtr, "GuiElementModels", "List of gui element models", false);
		I_ASSIGN(m_slaveGuiElementContainerCompPtr, "SlaveGuiElementContainer", "Slave GUI element container", false, "");
	I_END_COMPONENT;

protected:
	virtual QByteArrayList GetElementIds() const override;
	virtual const imtrest::IGuiElementModel* GetGuiElementModel(const QByteArray& elementId) const override;

protected:
	I_MULTIREF(imtrest::IGuiElementModel, m_guiElementModelsCompPtr);
	I_REF(imtrest::IGuiElementContainer, m_slaveGuiElementContainerCompPtr);
};


} // namespace imtrest


