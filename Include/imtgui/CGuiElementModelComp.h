#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <iqtgui/IVisualStatus.h>

// ImtCore includes
#include <imtbase/CCollectionInfo.h>
#include <imtgui/IGuiElementContainer.h>


namespace imtgui
{


class CGuiElementModelComp: public icomp::CComponentBase,
		virtual public imtgui::IGuiElementContainer
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CGuiElementModelComp);
		I_REGISTER_INTERFACE(istd::IChangeable);
		I_REGISTER_INTERFACE(imtbase::ICollectionInfo);
		I_REGISTER_INTERFACE(imtgui::IGuiElementContainer);
		I_ASSIGN_MULTI_0(m_itemNamesAttrPtr, "Names", "List of the page titles", true);
		I_ASSIGN_MULTI_0(m_itemIdsAttrPtr, "ItemIds", "List of unique page IDs", true);
		I_ASSIGN_MULTI_0(m_itemModesAttrPtr, "ItemModes", "List of page mode states", true);
		I_ASSIGN_MULTI_0(m_elementItemPathAttrPtr, "ElementItemPath", "List of the element item path", true);
		I_ASSIGN_MULTI_0(m_elementItemStatusesAttrPtr, "ElementItemStatuses", "List of the element item statuses", true);
	I_END_COMPONENT;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

	// reimplemented (imtgui::IGuiElementContainer)
	virtual const IGuiElementContainer* GetElementCommands(const QByteArray& elementId) const override;
	virtual QString GetElementItemPath(const QByteArray& elementId) const override;
	virtual QString GetElementStatus(const QByteArray& elementId) const override;
	virtual const imtbase::ICollectionInfo& GetElementList() const override;

protected:
	I_MULTITEXTATTR(m_itemNamesAttrPtr);
	I_MULTIATTR(QByteArray, m_itemIdsAttrPtr);
	I_MULTIATTR(bool, m_itemModesAttrPtr);
	I_MULTITEXTATTR(m_elementItemPathAttrPtr);
	I_MULTITEXTATTR(m_elementItemStatusesAttrPtr);

	imtbase::CCollectionInfo m_collectionInfo;
};


} // namespace imtgui


