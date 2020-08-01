#pragma once


// ACF includes
#include <ibase/TModelObserverCompWrap.h>
#include <iqtgui/TGuiObserverWrap.h>
#include <iqtgui/TGuiComponentBase.h>
#include <iprm/ISelectionParam.h>
#include <iprm/IEnableableParam.h>
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
		I_ASSIGN(m_mouseOverTextColorAttrPtr, "MouseOverTextColor", "Item under mouse text color", true, "#808080");
		I_ASSIGN(m_mouseOverSelectedTextColorAttrPtr, "MouseOverSelectedTextColor", "Selected item under mouse text color ", true, "#808080");
		I_ASSIGN(m_animationDelayAttrPtr, "AnimationDelay", "Animation delay", true, 800);
		I_ASSIGN(m_animationDurationAttrPtr, "AnimationDuration", "Animation duration", true, 150);
	I_END_COMPONENT;

protected:
	I_ATTR(QByteArray, m_textColorAttrPtr);
	I_ATTR(QByteArray, m_selectedColorAttrPtr);
	I_ATTR(QByteArray, m_selectedContourColorAttrPtr);
	I_ATTR(QByteArray, m_mouseOverColorAttrPtr);
	I_ATTR(QByteArray, m_mouseOverSelectedColorAttrPtr);
	I_ATTR(QByteArray, m_mouseOverTextColorAttrPtr);
	I_ATTR(QByteArray, m_mouseOverSelectedTextColorAttrPtr);

	I_ATTR(int, m_animationDelayAttrPtr);
	I_ATTR(int, m_animationDurationAttrPtr);
};


class CMenuPanelComp: public CMenuPanelCompBase
{
public:
	typedef CMenuPanelCompBase BaseClass;

	I_BEGIN_COMPONENT(CMenuPanelComp);
		I_ASSIGN(m_menuPanelVisibilityCompPtr, "MenuPanelVisibility", "Menu panel visibility status", false, "MenuPanelVisibility");
		I_ASSIGN_TO(m_menuPanelVisibilityModelCompPtr, m_menuPanelVisibilityCompPtr, false);
		I_ASSIGN(m_widgetProviderCompPtr, "WidgetProvider", "Widget provider for parent widget", false, "WidgetProvider");
		I_ASSIGN(m_monitorInfoProviderPtr, "MonitorInfoProvider", "Monitor info provider (count, size, resolution, etc.)", false, "MonitorInfoProvider");
		I_ASSIGN_TO(m_monitorInfoProviderModelPtr, m_monitorInfoProviderPtr, false);
		I_ASSIGN(m_isShowOverAttrPtr, "ShowOver", "Show expanded menu over the underlaying widget", true, true);
		I_ASSIGN(m_physicalResolutionAttrPtr, "PhysicalResolution", "Physical monitor resolution", true, 3.5);
		I_ASSIGN(m_indentAttrPtr, "Indent", "Item indentation", true, 6);
		I_ASSIGN(m_itemHeightAttrPtr, "ItemHeight", "Item Height", true, 15);
		I_ASSIGN(m_iconSizeRatioAttrPtr, "IconSizeRatio", "Icon size ratio relative to item height", true, 1);
		I_ASSIGN(m_iconSizeHoverRatioAttrPtr, "IconSizeHoverRatio", "Icon size ratio under mouse (relative to item normal size)", true, 1.2);
		I_ASSIGN(m_fontSizeRatioAttrPtr, "FontSizeRatio", "Font pixel size ratio relative to icon size", true, 0.3);
		I_ASSIGN(m_topPaddingAttrPtr, "TopPadding", "Top panel padding", true, 1);
		I_ASSIGN(m_leftPaddingAttrPtr, "LeftPadding", "Left item padding", true, 0);
		I_ASSIGN(m_rightPaddingAttrPtr, "RightPadding", "Right item padding", true, 20);
		I_ASSIGN(m_iconToTextPaddingAttrPtr, "IconToTextPadding", "Icon to text item padding", true, 1);
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

	class MenuPanelVisibility: public imod::TSingleModelObserverBase<iprm::IEnableableParam>
	{
	public:
		explicit MenuPanelVisibility(CMenuPanelComp& parent);

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
	MenuPanelVisibility m_menuPanelVisibilityObserver;

	I_REF(iprm::IEnableableParam, m_menuPanelVisibilityCompPtr);
	I_REF(imod::IModel, m_menuPanelVisibilityModelCompPtr);
	I_REF(imtgui::IWidgetProvider, m_widgetProviderCompPtr);
	I_REF(imtgui::IMonitorInfoProvider, m_monitorInfoProviderPtr);
	I_REF(imod::IModel, m_monitorInfoProviderModelPtr);

	I_ATTR(bool, m_isShowOverAttrPtr);

	// Widget attributes
	I_ATTR(double, m_physicalResolutionAttrPtr);

	// Size attributes
	I_ATTR(double, m_indentAttrPtr);
	I_ATTR(double, m_itemHeightAttrPtr);
	I_ATTR(double, m_iconSizeRatioAttrPtr);
	I_ATTR(double, m_iconSizeHoverRatioAttrPtr);
	I_ATTR(double, m_fontSizeRatioAttrPtr);

	// Size attributes
	I_ATTR(double, m_topPaddingAttrPtr);
	I_ATTR(double, m_leftPaddingAttrPtr);
	I_ATTR(double, m_rightPaddingAttrPtr);
	I_ATTR(double, m_iconToTextPaddingAttrPtr);

	double m_resolutionX;
	double m_resolutionY;
	double m_scale;

private:
	bool IsPageEnabled(const QByteArray& pageId) const;
	void UpdateSelection(const iprm::ISelectionParam& selection, const QByteArray& parentId);
	void CreatePageTree(const iprm::ISelectionParam& selection, const QByteArray& parentId);
	QByteArray FindSelectedItem();
	void UpdatePageSubselection();
	void UpdatePageState();
	void UpdateMonitorsInfo();
	void UpdateWidgetSizeAttributes();
	void UpdateMenuPanelVisibility();
};


} // namespace imtgui


