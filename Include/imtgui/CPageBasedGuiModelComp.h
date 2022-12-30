#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <iqtgui/IVisualStatus.h>

// ImtCore includes
#include <imtgui/IGuiElementContainer.h>


namespace imtgui
{


class CPageBasedGuiModelComp: public ilog::CLoggerComponentBase,
		virtual public imtgui::IGuiElementContainer
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CPageBasedGuiModelComp);
		I_REGISTER_INTERFACE(istd::IChangeable);
		I_REGISTER_INTERFACE(iprm::IOptionsList);
		I_REGISTER_INTERFACE(iqtgui::IMultiVisualStatusProvider);
		I_REGISTER_INTERFACE(imtgui::IGuiElementContainer);
		I_ASSIGN_MULTI_0(m_pageNamesAttrPtr, "Names", "List of the page titles", true);
		I_ASSIGN_MULTI_0(m_pageIdsAttrPtr, "PageIds", "List of unique page IDs", true);
		I_ASSIGN_MULTI_0(m_pageModesAttrPtr, "PageModes", "List of page mode states", true);
		I_ASSIGN_MULTI_0(m_pageVisualStatusesCompPtr, "PageVisualStatuses", "List of page visual statuses", true);
	I_END_COMPONENT;

	// reimplemented (iprm::IOptionsList)
	virtual int GetOptionsFlags() const override;
	virtual int GetOptionsCount() const override;
	virtual QString GetOptionName(int index) const override;
	virtual QString GetOptionDescription(int index) const override;
	virtual QByteArray GetOptionId(int index) const override;
	virtual bool IsOptionEnabled(int index) const override;

	// reimplemented (iqtgui::IMultiVisualStatusProvider)
	virtual int GetStatusesCount() const override;
	virtual const iqtgui::IVisualStatus* GetVisualStatus(int statusIndex) const override;

	// reimplemented (imtgui::IGuiElementContainer)
	virtual const IGuiElementContainer* GetElementCommands(const QByteArray& elementId) const override;

protected:
	I_MULTITEXTATTR(m_pageNamesAttrPtr);
	I_MULTIATTR(QByteArray, m_pageIdsAttrPtr);
	I_MULTIATTR(bool, m_pageModesAttrPtr);
	I_MULTIREF(iqtgui::IVisualStatus, m_pageVisualStatusesCompPtr);
};


} // namespace imtgui


