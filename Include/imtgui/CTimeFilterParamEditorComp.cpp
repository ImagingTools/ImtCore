#include <imtgui/CTimeFilterParamEditorComp.h>


// ACF includes
#include <istd/CChangeGroup.h>

// ImtCore includes
#include <imtbase/DateTimeUtils.h>


namespace imtgui
{


// public methods

CTimeFilterParamEditorComp::CTimeFilterParamEditorComp()
	:m_viewCommands("&View", 100),
	m_showCompleteHistoryCommand("Complete", 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR | ibase::ICommand::CF_ONOFF | ibase::ICommand::CF_EXCLUSIVE, CDI_TIME_SELECTION),
	m_show1HourCommand("1 hour", 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR | ibase::ICommand::CF_ONOFF | ibase::ICommand::CF_EXCLUSIVE, CDI_TIME_SELECTION),
	m_show4HoursCommand("4 hours", 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR | ibase::ICommand::CF_ONOFF | ibase::ICommand::CF_EXCLUSIVE, CDI_TIME_SELECTION),
	m_show8HoursCommand("8 hours", 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR | ibase::ICommand::CF_ONOFF | ibase::ICommand::CF_EXCLUSIVE, CDI_TIME_SELECTION),
	m_showDayCommand("Day", 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR | ibase::ICommand::CF_ONOFF | ibase::ICommand::CF_EXCLUSIVE, CDI_TIME_SELECTION),
	m_showWeekCommand("Week", 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR | ibase::ICommand::CF_ONOFF | ibase::ICommand::CF_EXCLUSIVE, CDI_TIME_SELECTION),
	m_showMonthCommand("Month", 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR | ibase::ICommand::CF_ONOFF | ibase::ICommand::CF_EXCLUSIVE, CDI_TIME_SELECTION),
	m_showQuorterCommand("Quorter", 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR | ibase::ICommand::CF_ONOFF | ibase::ICommand::CF_EXCLUSIVE, CDI_TIME_SELECTION),
	m_showYearCommand("Year", 100, ibase::ICommand::CF_GLOBAL_MENU | ibase::ICommand::CF_TOOLBAR | ibase::ICommand::CF_ONOFF | ibase::ICommand::CF_EXCLUSIVE, CDI_TIME_SELECTION)
{
	connect(&m_showCompleteHistoryCommand, SIGNAL(triggered()), this, SLOT(OnShowCompleteHistory()));
	connect(&m_show1HourCommand, SIGNAL(triggered()), this, SLOT(OnShow1Hour()));
	connect(&m_show4HoursCommand, SIGNAL(triggered()), this, SLOT(OnShow4Hours()));
	connect(&m_show8HoursCommand, SIGNAL(triggered()), this, SLOT(OnShow8Hours()));
	connect(&m_showDayCommand, SIGNAL(triggered()), this, SLOT(OnShowDay()));
	connect(&m_showWeekCommand, SIGNAL(triggered()), this, SLOT(OnShowWeek()));
	connect(&m_showMonthCommand, SIGNAL(triggered()), this, SLOT(OnShowMonth()));
	connect(&m_showQuorterCommand, SIGNAL(triggered()), this, SLOT(OnShowQuorter()));
	connect(&m_showYearCommand, SIGNAL(triggered()), this, SLOT(OnShowYear()));

	m_viewCommands.InsertChild(&m_showCompleteHistoryCommand);
	m_viewCommands.InsertChild(&m_show1HourCommand);
	m_viewCommands.InsertChild(&m_show4HoursCommand);
	m_viewCommands.InsertChild(&m_show8HoursCommand);
	m_viewCommands.InsertChild(&m_showDayCommand);
	m_viewCommands.InsertChild(&m_showWeekCommand);
	m_viewCommands.InsertChild(&m_showMonthCommand);
	m_viewCommands.InsertChild(&m_showQuorterCommand);
	m_viewCommands.InsertChild(&m_showYearCommand);

	m_rootCommands.InsertChild(&m_viewCommands);

	m_showCompleteHistoryCommand.setChecked(true);
}


// protected methods

// reimplemented (ibase::ICommandsProvider)

const ibase::IHierarchicalCommand* CTimeFilterParamEditorComp::GetCommands() const
{
	return &m_rootCommands;
}


// reimplemented (iqtgui::CGuiComponentBase)

void CTimeFilterParamEditorComp::OnGuiCreated()
{
	BaseClass::OnGuiCreated();

	connect(TimePeriods, SIGNAL(currentIndexChanged(int)), this, SLOT(OnTimePeriodsChanged(int)));
	connect(Calendar, SIGNAL(selectionChanged()), this, SLOT(OnCalendarSelectionChanged()));

	if (GetTimeSelectionMode() == TimeSelectionMode::TSM_SELECTED){
		m_show1HourCommand.setVisible(false);
		m_show4HoursCommand.setVisible(false);
		m_show8HoursCommand.setVisible(false);
	}

	TimeFilterFrame->hide();
}


void CTimeFilterParamEditorComp::OnGuiRetranslate()
{
	BaseClass::OnGuiRetranslate();

	m_showCompleteHistoryCommand.SetVisuals(tr("Show complete"), tr("Complete"), tr("Show complete data statistics"), QIcon(":/Icons/StatisticsAll"));
	m_show1HourCommand.SetVisuals(tr("Show 1 hour"), tr("Hour"), tr("Show statistics for 1 hour"), QIcon(":/Icons/Statistics1h"));
	m_show4HoursCommand.SetVisuals(tr("Show 4 hours"), tr("4 Hours"), tr("Show statistics for the 4 hours"), QIcon(":/Icons/Statistics4h"));
	m_show8HoursCommand.SetVisuals(tr("Show 8 hours"), tr("8 Hours"), tr("Show statistics for the 8 hours"), QIcon(":/Icons/Statistics8h"));
	m_showDayCommand.SetVisuals(tr("Show day"), tr("Day"), tr("Show statistics for the day"), QIcon(":/Icons/Statistics1d"));
	m_showWeekCommand.SetVisuals(tr("Show week"), tr("Week"), tr("Show statistics for the week"), QIcon(":/Icons/Statistics1w"));
	m_showMonthCommand.SetVisuals(tr("Show month"), tr("Month"), tr("Show statistics for the month"), QIcon(":/Icons/Statistics1m"));
	m_showQuorterCommand.SetVisuals(tr("Show quorter"), tr("Quorter"), tr("Show statistics for the quorter"), QIcon(":/Icons/Statistics1q"));
	m_showYearCommand.SetVisuals(tr("Show year"), tr("Year"), tr("Show statistics for the year"), QIcon(":/Icons/Statistics1y"));
}


// private slots

void CTimeFilterParamEditorComp::OnShowCompleteHistory()
{
	TimeFilterFrame->hide();

	SetFilterTimeRange(QDateTime(), QDateTime::currentDateTime());
}


void CTimeFilterParamEditorComp::OnShow1Hour()
{
	TimeFilterFrame->hide();

	SetFilterTimeRange(QDateTime::currentDateTime().addSecs(-60 * 60), QDateTime::currentDateTime());
}


void CTimeFilterParamEditorComp::OnShow4Hours()
{
	TimeFilterFrame->hide();

	SetFilterTimeRange(QDateTime::currentDateTime().addSecs(-4 * 60 * 60), QDateTime::currentDateTime());
}


void CTimeFilterParamEditorComp::OnShow8Hours()
{
	TimeFilterFrame->hide();

	SetFilterTimeRange(QDateTime::currentDateTime().addSecs(-8 * 60 * 60), QDateTime::currentDateTime());
}


void CTimeFilterParamEditorComp::OnShowDay()
{
	if (GetTimeSelectionMode() == TimeSelectionMode::TSM_SELECTED){
		TimeFilterFrame->show();
		TimeFilterWidget->setCurrentIndex(0);

		CalendarLabel->setText("Choose statistics day:");

		imtbase::ITimeFilterParam* objectPtr = GetObjectPtr();
		Q_ASSERT(objectPtr != NULL);

		QDateTime statisticsDayBegin = imtbase::GetDayBegin(objectPtr->GetTimeRange().GetBeginTime());
		QDateTime statisticsDayEnd = imtbase::GetDayEnd(objectPtr->GetTimeRange().GetEndTime());

		Calendar->blockSignals(true);

		Calendar->setDateRange(statisticsDayBegin.date(), statisticsDayEnd.date());
		Calendar->setSelectedDate(statisticsDayBegin.date());

		Calendar->blockSignals(false);

		OnCalendarSelectionChanged();
	}
	else{
		SetFilterTimeRange(QDateTime::currentDateTime().addDays(-1), QDateTime::currentDateTime());
	}
}


void CTimeFilterParamEditorComp::OnShowWeek()
{
	if (GetTimeSelectionMode() == TimeSelectionMode::TSM_SELECTED){
		TimeFilterFrame->show();
		TimeFilterWidget->setCurrentIndex(0);

		CalendarLabel->setText("Choose statistics week:");

		imtbase::ITimeFilterParam* objectPtr = GetObjectPtr();
		Q_ASSERT(objectPtr != NULL);

		QDateTime statisticsWeekBegin = imtbase::GetWeekBegin(objectPtr->GetTimeRange().GetBeginTime());
		QDateTime statisticsWeekEnd = imtbase::GetWeekEnd(objectPtr->GetTimeRange().GetEndTime());

		Calendar->blockSignals(true);

		Calendar->setDateRange(statisticsWeekBegin.date(), statisticsWeekEnd.date());
		Calendar->setSelectedDate(statisticsWeekBegin.date());

		Calendar->blockSignals(false);

		OnCalendarSelectionChanged();
	}
	else{
		SetFilterTimeRange(QDateTime::currentDateTime().addDays(-7), QDateTime::currentDateTime());
	}
}


void CTimeFilterParamEditorComp::OnShowMonth()
{
	if (GetTimeSelectionMode() == TimeSelectionMode::TSM_SELECTED){
		TimeFilterFrame->show();
		TimeFilterWidget->setCurrentIndex(1);

		TimePeriodsLabel->setText("Choose statistics month:");

		imtbase::ITimeFilterParam* objectPtr = GetObjectPtr();
		Q_ASSERT(objectPtr != NULL);

		QDateTime statisticsMonthBegin = imtbase::GetMonthBegin(objectPtr->GetTimeRange().GetBeginTime());
		QDateTime statisticsMonthEnd = imtbase::GetMonthEnd(objectPtr->GetTimeRange().GetEndTime());

		TimePeriods->blockSignals(true);
		TimePeriods->clear();

		for (QDateTime dt = statisticsMonthBegin; dt <= statisticsMonthEnd; dt = dt.addMonths(1)){
			QString strMonth = dt.toString("MMMM yyyy");

			TimePeriods->addItem(strMonth, QVariant(dt));
		}

		TimePeriods->blockSignals(false);

		if (TimePeriods->count() > 0){
			OnTimePeriodsChanged(0);
		}
	}
	else{
		SetFilterTimeRange(QDateTime::currentDateTime().addMonths(-1), QDateTime::currentDateTime());
	}
}


void CTimeFilterParamEditorComp::OnShowQuorter()
{
	if (GetTimeSelectionMode() == TimeSelectionMode::TSM_SELECTED){
		TimeFilterFrame->show();
		TimeFilterWidget->setCurrentIndex(1);

		TimePeriodsLabel->setText("Choose statistics quorter:");

		imtbase::ITimeFilterParam* objectPtr = GetObjectPtr();
		Q_ASSERT(objectPtr != NULL);

		QDateTime statisticsQuorterBegin = imtbase::GetQuorterBegin(objectPtr->GetTimeRange().GetBeginTime());
		QDateTime statisticsQuorterEnd = imtbase::GetQuorterEnd(objectPtr->GetTimeRange().GetEndTime());

		TimePeriods->blockSignals(true);
		TimePeriods->clear();

		for (QDateTime dt = statisticsQuorterBegin; dt <= statisticsQuorterEnd; dt = dt.addMonths(3)){
			int quorter = imtbase::GetQuorter(dt);
			QString strQuorter = QString("Q%1 %2").arg(quorter).arg(statisticsQuorterEnd.date().year());

			TimePeriods->addItem(strQuorter, QVariant(dt));
		}

		TimePeriods->blockSignals(false);

		if (TimePeriods->count() > 0){
			OnTimePeriodsChanged(0);
		}
	}
	else{
		SetFilterTimeRange(QDateTime::currentDateTime().addMonths(-3), QDateTime::currentDateTime());
	}
}


void CTimeFilterParamEditorComp::OnShowYear()
{
	if (GetTimeSelectionMode() == TimeSelectionMode::TSM_SELECTED){
		TimeFilterFrame->show();
		TimeFilterWidget->setCurrentIndex(1);

		TimePeriodsLabel->setText("Choose statistics year:");

		TimePeriods->blockSignals(true);
		TimePeriods->clear();

		imtbase::ITimeFilterParam* objectPtr = GetObjectPtr();
		Q_ASSERT(objectPtr != NULL);

		int beginYear = objectPtr->GetTimeRange().GetBeginTime().date().year();
		int endYear = objectPtr->GetTimeRange().GetEndTime().date().year();

		for (int year = beginYear; year <= endYear; ++year){
			TimePeriods->addItem(QString::number(year), QVariant(year));
		}

		TimePeriods->blockSignals(false);

		if (TimePeriods->count() > 0){
			OnTimePeriodsChanged(0);
		}
	}
	else{
		SetFilterTimeRange(QDateTime::currentDateTime().addYears(-1), QDateTime::currentDateTime());
	}
}


void CTimeFilterParamEditorComp::OnTimePeriodsChanged(int index)
{
	if (index < 0){
		return;
	}

	QDateTime beginTime, endTime;

	if (m_showMonthCommand.isChecked()){
		beginTime = TimePeriods->currentData(Qt::UserRole).toDateTime();
		endTime = imtbase::GetMonthEnd(beginTime);
	}
	else if (m_showQuorterCommand.isChecked()){
		beginTime = TimePeriods->currentData(Qt::UserRole).toDateTime();
		endTime = imtbase::GetQuorterEnd(beginTime);
	}
	else if (m_showYearCommand.isChecked()){
		int year = TimePeriods->currentData(Qt::UserRole).toInt();

		beginTime = QDateTime(QDate(year, 1, 1), QTime(0, 0));
		endTime = QDateTime(QDate(year, 12, 31), QTime(0, 0));
	}

	SetFilterTimeRange(beginTime, endTime);
}


void CTimeFilterParamEditorComp::OnCalendarSelectionChanged()
{
	QDateTime beginTime, endTime;

	if (m_showDayCommand.isChecked()){
		beginTime.setDate(Calendar->selectedDate());
		endTime = imtbase::GetDayEnd(beginTime);
	}
	else if (m_showWeekCommand.isChecked()){
		beginTime = imtbase::GetWeekBegin(QDateTime(Calendar->selectedDate()));
		endTime = imtbase::GetWeekEnd(beginTime);
	}

	SetFilterTimeRange(beginTime, endTime);
}


// private methods

void CTimeFilterParamEditorComp::UpdateStatisticsGui()
{
	if (m_showCompleteHistoryCommand.isChecked()){
		OnShowCompleteHistory();
	}
	else if (m_show1HourCommand.isChecked()){
		OnShow1Hour();
	}
	else if (m_show4HoursCommand.isChecked()){
		OnShow4Hours();
	}
	else if (m_show8HoursCommand.isChecked()){
		OnShow8Hours();
	}
	else if (m_showDayCommand.isChecked()){
		OnShowDay();
	}
	else if (m_showWeekCommand.isChecked()){
		OnShowWeek();
	}
	else if (m_showMonthCommand.isChecked()){
		OnShowMonth();
	}
	else if (m_showQuorterCommand.isChecked()){
		OnShowQuorter();
	}
	else if (m_showYearCommand.isChecked()){
		OnShowYear();
	}
}


void CTimeFilterParamEditorComp::SetFilterTimeRange(const QDateTime& beginTime, const QDateTime& endTime)
{
	imtbase::ITimeFilterParam* objectPtr = GetObjectPtr();
	Q_ASSERT(objectPtr != NULL);

	objectPtr->SetTimeRange(imtbase::CTimeRange(beginTime, endTime));
}


CTimeFilterParamEditorComp::TimeSelectionMode CTimeFilterParamEditorComp::GetTimeSelectionMode() const
{
	if (m_timeSelectionModeAttrPtr.IsValid() &&
		*m_timeSelectionModeAttrPtr >= TimeSelectionMode::TSM_FIRST &&
		*m_timeSelectionModeAttrPtr <= TimeSelectionMode::TSM_LAST){
		return static_cast<TimeSelectionMode>(*m_timeSelectionModeAttrPtr);
	}
	else{
		return TimeSelectionMode::TSM_UNTIL_NOW;
	}
}


} // namespace imtgui


