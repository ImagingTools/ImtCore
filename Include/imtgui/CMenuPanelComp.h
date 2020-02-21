#pragma once


// ACF includes
#include <ibase/TModelObserverCompWrap.h>
#include <iqtgui/TGuiObserverWrap.h>
#include <iqtgui/TGuiComponentBase.h>
#include <iprm/ISelectionParam.h>
#include <imod/CMultiModelDispatcherBase.h>

// ImtCore includes
#include <imtgui/IWidgetProvider.h>
#include <imtgui/IMonitorInfoProvider.h>
#include <imtwidgets/CMenuPanel.h>


namespace imtgui
{


class CMenuPanelCompBase:
			public ibase::TModelObserverCompBaseWrap<
						iqtgui::TGuiObserverWrap<
									iqtgui::TGuiComponentBase<imtwidgets::CMenuPanel>,
									imod::TSingleModelObserverBase<iprm::ISelectionParam>>>
{
public:
	typedef ibase::TModelObserverCompBaseWrap<
				iqtgui::TGuiObserverWrap<
							iqtgui::TGuiComponentBase<imtwidgets::CMenuPanel>,
							imod::TSingleModelObserverBase<iprm::ISelectionParam>>> BaseClass;

	I_BEGIN_COMPONENT(CMenuPanelCompBase);
		I_ASSIGN(m_textColorAttrPtr, "TextColor", "Item text color", true, "#000000");
		I_ASSIGN(m_selectedColorAttrPtr, "SelectedColor", "Selected item background color", true, "#808080");
		I_ASSIGN(m_selectedContourColorAttrPtr, "SelectedContourColor", "Selected item contour color", true, "#000000");
		I_ASSIGN(m_mouseOverColorAttrPtr, "MouseOverColor", "Item under mouse background color", true, "#808080");
		I_ASSIGN(m_mouseOverSelectedColorAttrPtr, "MouseOverSelectedColor", "Selected item under mouse background color ", true, "#808080");
	
		I_ASSIGN(m_animationDelayAttrPtr, "AnimationDelay", "Animation delay", true, 500);
		I_ASSIGN(m_animationDurationAttrPtr, "AnimationDuration", "Animation duration", true, 300);
	I_END_COMPONENT;

protected:
	I_ATTR(QByteArray, m_textColorAttrPtr);
	I_ATTR(QByteArray, m_selectedColorAttrPtr);
	I_ATTR(QByteArray, m_selectedContourColorAttrPtr);
	I_ATTR(QByteArray, m_mouseOverColorAttrPtr);
	I_ATTR(QByteArray, m_mouseOverSelectedColorAttrPtr);

	I_ATTR(int, m_animationDelayAttrPtr);
	I_ATTR(int, m_animationDurationAttrPtr);
};


class CMenuPanelComp: public CMenuPanelCompBase
{
public:
	typedef CMenuPanelCompBase BaseClass;

	I_BEGIN_COMPONENT(CMenuPanelComp);
		I_ASSIGN(m_widgetProviderCompPtr, "WidgetProvider", "Widget provider for parent widget", false, "WidgetProvider");
		I_ASSIGN(m_monitorInfoProviderPtr, "MonitorInfoProvider", "Monitor info provider (count, size, resolution, etc.)", false, "MonitorInfoProvider");
		I_ASSIGN(m_isShowOverAttrPtr, "ShowOver", "Show expanded menu over the underlaying widget", false, true);

		I_ASSIGN(m_physicalResolutionAttrPtr, "PhysicalResolution", "Physical monitor resolution", true, 3.5);

		I_ASSIGN(m_indentAttrPtr, "Indent", "Item indentation", true, 20);
		I_ASSIGN(m_iconSizeAttrPtr, "IconSize", "Icon size", true, 24);
		I_ASSIGN(m_selectionSizeRatioAttrPtr, "SelectionSizeRatio", "Selection height ratio relative to icon size", true, 1.2);

		I_ASSIGN(m_verticalPaddingAttrPtr, "VerticalPadding", "Vertical item padding", true, 5);
		I_ASSIGN(m_leftPaddingAttrPtr, "LeftPadding", "Left item padding", true, 5);
		I_ASSIGN(m_rightPaddingAttrPtr, "RightPadding", "Right item padding", true, 20);
		I_ASSIGN(m_iconToTextPaddingAttrPtr, "IconToTextPadding", "Icon to text item padding", true, 5);
	I_END_COMPONENT;

	CMenuPanelComp();

protected:
	void OnPageIdChanged(const QByteArray& selectedPageId, const QByteArray& deselectedPageId);

	// reimplemented (iqtgui::CGuiComponentBase)
	virtual void OnGuiCreated() override;

	// reimplemented (iqtgui::TGuiObserverWrap)
	virtual void UpdateGui(const istd::IChangeable::ChangeSet& changeSet) override;
	virtual void OnGuiModelAttached() override;
	virtual void OnGuiModelDetached() override;

	// reimplemented (iqtgui::CGuiComponentBase)
	virtual void OnGuiRetranslate() override;

private:
	class PageSubselectionObserver: public imod::CMultiModelDispatcherBase
	{
	public:
		explicit PageSubselectionObserver(CMenuPanelComp& parent);

		// reimplemented (imod::CMultiModelDispatcherBase)
		void OnModelChanged(int modelId, const istd::IChangeable::ChangeSet& changeSet) override;

	private:
		CMenuPanelComp& m_parent;
	};

	class PageVisualStatusObserver: public imod::CMultiModelDispatcherBase
	{
	public:
		explicit PageVisualStatusObserver(CMenuPanelComp& parent);

		// reimplemented (imod::CMultiModelDispatcherBase)
		void OnModelChanged(int modelId, const istd::IChangeable::ChangeSet& changeSet) override;

	private:
		CMenuPanelComp& m_parent;
	};

	class MonitorsInfoObserver: public imod::TSingleModelObserverBase<imtgui::IMonitorInfoProvider>
	{
	public:
		explicit MonitorsInfoObserver(CMenuPanelComp& parent);

		// reimplemented (imod::CSingleModelObserverBase)
		virtual void OnUpdate(const istd::IChangeable::ChangeSet& changeSet);

	private:
		CMenuPanelComp& m_parent;
	};

	struct PageInfo
	{
		QByteArray parentPageId;
		const iprm::ISelectionParam* selectionPtr;
		int pageIndex;
	};

	QMap<QByteArray, PageInfo> m_pagesInfoMap;
	int m_subselectionModelIndex;
	int m_visualStatusModelIndex;
	PageSubselectionObserver m_pageSubselectionObserver;
	PageVisualStatusObserver m_pageVisualStatusObserver;
	MonitorsInfoObserver m_monitorInfoObserver;

	I_REF(imtgui::IWidgetProvider, m_widgetProviderCompPtr);
	I_REF(imtgui::IMonitorInfoProvider, m_monitorInfoProviderPtr);
	I_ATTR(bool, m_isShowOverAttrPtr);

	// Widget attributes:
	I_ATTR(double, m_physicalResolutionAttrPtr);

	I_ATTR(int, m_indentAttrPtr);
	I_ATTR(int, m_iconSizeAttrPtr);
	I_ATTR(double, m_selectionSizeRatioAttrPtr);

	I_ATTR(int, m_verticalPaddingAttrPtr);
	I_ATTR(int, m_leftPaddingAttrPtr);
	I_ATTR(int, m_rightPaddingAttrPtr);
	I_ATTR(int, m_iconToTextPaddingAttrPtr);

	int m_indent;
	int m_iconSize;

	int m_verticalPadding;
	int m_leftPadding;
	int m_rightPadding;
	int m_iconToTextPadding;

	double m_resolutionX;
	double m_resolutionY; 

private:
	bool IsPageEnabled(const QByteArray& pageId) const;
	void UpdateSelection(const iprm::ISelectionParam& selection, const QByteArray& parentId);
	void CreatePageTree(const iprm::ISelectionParam& selection, const QByteArray& parentId);
	QByteArray FindSelectedItem();
	void UpdatePageSubselection();
	void UpdatePageState();
	void UpdateMonitorsInfo();
	void UpdateWidgetSizeAttributes();
};


} // namespace imtgui


