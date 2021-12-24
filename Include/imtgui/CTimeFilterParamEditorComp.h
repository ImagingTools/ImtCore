#pragma once


// ACF includes
#include <iqtgui/TDesignerGuiObserverCompBase.h>
#include <ibase/ICommandsProvider.h>
#include <iqtgui/CHierarchicalCommand.h>

// Acula includes
#include <imtbase/ITimeFilterParam.h>
#include <GeneratedFiles/imtgui/ui_CTimeFilterParamEditorComp.h>


namespace imtgui
{


class CTimeFilterParamEditorComp:
			public iqtgui::TDesignerGuiObserverCompBase<Ui::CTimeFilterParamEditorComp, imtbase::ITimeFilterParam>,
			virtual public ibase::ICommandsProvider
{
	Q_OBJECT

public:
	typedef iqtgui::TDesignerGuiObserverCompBase<Ui::CTimeFilterParamEditorComp, imtbase::ITimeFilterParam> BaseClass;

	I_BEGIN_COMPONENT(CTimeFilterParamEditorComp);
		I_REGISTER_INTERFACE(ibase::ICommandsProvider);
		I_ASSIGN(m_timeSelectionModeAttrPtr, "TimeSelectionMode", "Statistics filter time selection mode:\r\n0 - show last statistics until current time\r\n1 - show statistics for the selected time range", true, 0);
	I_END_COMPONENT;

	CTimeFilterParamEditorComp();

	// reimplemented (ibase::ICommandsProvider)
	virtual const ibase::IHierarchicalCommand* GetCommands() const override;

protected:
	// reimplemented (iqtgui::CGuiComponentBase)
	void OnGuiCreated() override;
	void OnGuiRetranslate() override;

private Q_SLOTS:
	void OnShowCompleteHistory();
	void OnShow1Hour();
	void OnShow4Hours();
	void OnShow8Hours();
	void OnShowDay();
	void OnShowWeek();
	void OnShowMonth();
	void OnShowQuorter();
	void OnShowYear();
	void OnTimePeriodsChanged(int index);
	void OnCalendarSelectionChanged();

private:
	enum CommandGroupId
	{
		CDI_TIME_SELECTION = 1001
	};

	enum TimeSelectionMode
	{
		TSM_UNTIL_NOW = 0,
		TSM_SELECTED,
		TSM_FIRST = TSM_UNTIL_NOW,
		TSM_LAST = TSM_SELECTED
	};

private:
	void UpdateStatisticsGui();
	void SetFilterTimeRange(const QDateTime& beginTime, const QDateTime& endTime);
	TimeSelectionMode GetTimeSelectionMode() const;

private:
	// commands
	iqtgui::CHierarchicalCommand m_rootCommands;
	iqtgui::CHierarchicalCommand m_viewCommands;
	iqtgui::CHierarchicalCommand m_showCompleteHistoryCommand;
	iqtgui::CHierarchicalCommand m_show1HourCommand;
	iqtgui::CHierarchicalCommand m_show4HoursCommand;
	iqtgui::CHierarchicalCommand m_show8HoursCommand;
	iqtgui::CHierarchicalCommand m_showDayCommand;
	iqtgui::CHierarchicalCommand m_showWeekCommand;
	iqtgui::CHierarchicalCommand m_showMonthCommand;
	iqtgui::CHierarchicalCommand m_showQuorterCommand;
	iqtgui::CHierarchicalCommand m_showYearCommand;

	I_ATTR(int, m_timeSelectionModeAttrPtr);
};


} // namespace imtgui


