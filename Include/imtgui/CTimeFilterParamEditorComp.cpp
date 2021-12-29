#include <imtgui/CTimeFilterParamEditorComp.h>


// ImtCore includes
#include <imtbase/DateTimeUtils.h>


namespace imtgui
{


// public methods

CTimeFilterParamEditorComp::CTimeFilterParamEditorComp()
{
}


// protected methods

// reimplemented (iqtgui::CGuiComponentBase)

void CTimeFilterParamEditorComp::OnGuiCreated()
{
	BaseClass::OnGuiCreated();

	CustomRangeFrame->setVisible(false);

	CustomBeginDate->setDateTime(QDateTime::currentDateTime().addMonths(-6));
	CustomEndDate->setDateTime(QDateTime::currentDateTime());
}


void CTimeFilterParamEditorComp::OnGuiRetranslate()
{
	BaseClass::OnGuiRetranslate();
}


void CTimeFilterParamEditorComp::on_CustomBeginDate_dateTimeChanged(const QDateTime& /*dateTime*/)
{
	SetFilterTimeRange(CustomBeginDate->dateTime(), CustomEndDate->dateTime());
}


void CTimeFilterParamEditorComp::on_CustomEndDate_dateTimeChanged(const QDateTime & /*dateTime*/)
{
	SetFilterTimeRange(CustomBeginDate->dateTime(), CustomEndDate->dateTime());
}


// private slots

void CTimeFilterParamEditorComp::on_TimeCombo_currentIndexChanged(int index)
{
	CustomRangeFrame->setVisible(false);

	switch (index){
	case 0:
		SetFilterTimeRange(QDateTime(), QDateTime());
		break;
	case 1:
		SetFilterTimeRange(QDateTime::currentDateTime().addSecs(-60 * 60), QDateTime::currentDateTime());
		break;
	case 2:
		SetFilterTimeRange(QDateTime::currentDateTime().addSecs(-4 * 60 * 60), QDateTime::currentDateTime());
		break;
	case 3:
		SetFilterTimeRange(QDateTime::currentDateTime().addSecs(-8 * 60 * 60), QDateTime::currentDateTime());
		break;
	case 4:
		SetFilterTimeRange(QDateTime::currentDateTime().addDays(-1), QDateTime::currentDateTime());
		break;
	case 5:
		SetFilterTimeRange(QDateTime::currentDateTime().addDays(-7), QDateTime::currentDateTime());
		break;
	case 6:
		SetFilterTimeRange(QDateTime::currentDateTime().addMonths(-1), QDateTime::currentDateTime());
		break;
	case 7:
		SetFilterTimeRange(QDateTime::currentDateTime().addMonths(-3), QDateTime::currentDateTime());
		break;
	case 8:
		SetFilterTimeRange(QDateTime::currentDateTime().addYears(-1), QDateTime::currentDateTime());
		break;
	case 9:
		CustomRangeFrame->setVisible(true);

		SetFilterTimeRange(CustomBeginDate->dateTime(), CustomEndDate->dateTime());
		break;
	}
}


// private methods

void CTimeFilterParamEditorComp::SetFilterTimeRange(const QDateTime& beginTime, const QDateTime& endTime)
{
	imtbase::ITimeFilterParam* objectPtr = GetObjectPtr();
	Q_ASSERT(objectPtr != NULL);

	objectPtr->SetTimeRange(imtbase::CTimeRange(beginTime, endTime));
}


} // namespace imtgui


