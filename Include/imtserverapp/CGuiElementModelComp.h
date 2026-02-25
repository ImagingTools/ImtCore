// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtserverapp/IGuiElementModel.h>
#include <imtserverapp/IGuiElementContainer.h>


namespace imtserverapp
{


class CGuiElementModelComp:
			public icomp::CComponentBase,
			virtual public imtserverapp::IGuiElementModel
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CGuiElementModelComp);
		I_REGISTER_INTERFACE(istd::IChangeable);
		I_REGISTER_INTERFACE(imtserverapp::IGuiElementModel);
		I_ASSIGN(m_elementIdAttrPtr, "Id", "Element ID", false, "");
		I_ASSIGN(m_elementNameAttrPtr, "Name", "Element name", false, "");
		I_ASSIGN(m_elementDescriptionAttrPtr, "Description", "Element description", false, "");
		I_ASSIGN(m_elementItemPathAttrPtr, "IconPath", "Path to icon", false, "");
		I_ASSIGN(m_elementStatusAttrPtr, "Status", "Element status", false, "");
		I_ASSIGN(m_enabledAttrPtr, "IsEnabled", "Element is enabled", false, true);
		I_ASSIGN(m_visibleAttrPtr, "IsVisible", "Element visibility", false, true);
		I_ASSIGN(m_priorityAttrPtr, "Priority", "Element priority", false, 0);
		I_ASSIGN(m_alignmentAttrPtr, "Alignment", "1 - Left alignment, 2 - Right alignment, 4 - HCenter alignment, 8 - Justify alignment, 32 - Top alignment, 64 - Bottom alignment, 128 - VCenter alignment", false, 1);
		I_ASSIGN(m_subElementsCompPtr, "SubElementsContainer", "Container of the sub elements", false, "SubElementsContainer");
	I_END_COMPONENT;

	// reimplemented (imtserverapp::IGuiElementModel)
	virtual QByteArray GetElementId() const override;
	virtual QString GetElementName() const override;
	virtual QString GetElementDescription() const override;
	virtual QString GetElementItemPath() const override;
	virtual QString GetElementStatus() const override;
	virtual bool IsEnabled() const override;
	virtual bool IsVisible() const override;
	virtual int GetPriority() const override;
	virtual int GetAlignment() const override;
	virtual const IGuiElementContainer* GetSubElements() const override;

protected:
	I_ATTR(QByteArray, m_elementIdAttrPtr);
	I_ATTR(QString, m_elementItemPathAttrPtr);
	I_TEXTATTR(m_elementNameAttrPtr);
	I_TEXTATTR(m_elementDescriptionAttrPtr);
	I_TEXTATTR(m_elementStatusAttrPtr);
	I_ATTR(bool, m_enabledAttrPtr);
	I_ATTR(bool, m_visibleAttrPtr);
	I_ATTR(int, m_priorityAttrPtr);
	I_ATTR(int, m_alignmentAttrPtr);
	I_REF(imtserverapp::IGuiElementContainer, m_subElementsCompPtr);
};


} // namespace imtserverapp


