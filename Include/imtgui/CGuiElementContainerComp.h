#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtgui/IGuiElementContainer.h>
#include <imtgui/IGuiElementModel.h>


namespace imtgui
{


class CGuiElementContainerComp: public icomp::CComponentBase,
		virtual public imtgui::IGuiElementContainer
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CGuiElementContainerComp);
		I_REGISTER_INTERFACE(istd::IChangeable);
		I_REGISTER_INTERFACE(imtgui::IGuiElementContainer);
		I_ASSIGN_MULTI_0(m_guiElementModelsCompPtr, "GuiElementModels", "List of gui element models", true);
	I_END_COMPONENT;

protected:
	virtual QByteArrayList GetElementIds() const override;
	virtual const imtgui::IGuiElementModel* GetGuiElementModel(const QByteArray& elementId) const override;
protected:
	I_MULTIREF(imtgui::IGuiElementModel, m_guiElementModelsCompPtr);
};


} // namespace imtgui


