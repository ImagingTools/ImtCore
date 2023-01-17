#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtgui/IGuiElementModel.h>


namespace imtgui
{


class CGuiElementModelComp: public icomp::CComponentBase,
		virtual public imtgui::IGuiElementModel
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CGuiElementModelComp);
		I_REGISTER_INTERFACE(istd::IChangeable);
		I_REGISTER_INTERFACE(imtgui::IGuiElementModel);
		I_ASSIGN(m_elementIdAttrPtr, "Id", "Element ID", false, "");
		I_ASSIGN(m_elementNameAttrPtr, "Name", "Element name", false, "");
		I_ASSIGN(m_elementDescriptionAttrPtr, "Description", "Element description", false, "");
		I_ASSIGN(m_elementItemPathAttrPtr, "ItemPath", "Element item path", false, "");
		I_ASSIGN(m_elementStatusAttrPtr, "Status", "Element status", false, "");
		I_ASSIGN(m_enabledAttrPtr, "IsEnabled", "Element is enabled", false, true);
	I_END_COMPONENT;

	// reimplemented (imtgui::IGuiElementContainer)
	virtual QByteArray GetElementId() const override;
	virtual QString GetElementName() const override;
	virtual QString GetElementDescription() const override;
	virtual QString GetElementItemPath() const override;
	virtual QString GetElementStatus() const override;
	virtual bool IsEnabled() const override;
	virtual const IGuiElementModel* GetSubElements() const override;

protected:
	I_ATTR(QByteArray, m_elementIdAttrPtr);
	I_TEXTATTR(m_elementNameAttrPtr);
	I_TEXTATTR(m_elementDescriptionAttrPtr);
	I_TEXTATTR(m_elementItemPathAttrPtr);
	I_TEXTATTR(m_elementStatusAttrPtr);
	I_ATTR(bool, m_enabledAttrPtr);
	I_MULTIREF(imtgui::IGuiElementModel, m_subElementsCompPtr);
};


} // namespace imtgui


