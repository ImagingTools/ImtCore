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
		SetTimeUnit(imtbase::ITimeFilterParam::TU_HOUR, imtbase::ITimeFilterParam::IM_FOR, 1);
		break;
	case 2:
		SetTimeUnit(imtbase::ITimeFilterParam::TU_HOUR, imtbase::ITimeFilterParam::IM_FOR, 4);
		break;
	case 3:
		SetTimeUnit(imtbase::ITimeFilterParam::TU_HOUR, imtbase::ITimeFilterParam::IM_FOR, 8);
		break;
	case 4:
		SetTimeUnit(imtbase::ITimeFilterParam::TU_DAY, imtbase::ITimeFilterParam::IM_FOR, 1);
		break;
	case 5:
		SetTimeUnit(imtbase::ITimeFilterParam::TU_WEEK, imtbase::ITimeFilterParam::IM_FOR, 1);
		break;
	case 6:
		SetTimeUnit(imtbase::ITimeFilterParam::TU_MONTH, imtbase::ITimeFilterParam::IM_FOR, 1);
		break;
	case 7:
		SetTimeUnit(imtbase::ITimeFilterParam::TU_MONTH, imtbase::ITimeFilterParam::IM_FOR, 3);
		break;
	case 8:
		SetTimeUnit(imtbase::ITimeFilterParam::TU_YEAR, imtbase::ITimeFilterParam::IM_FOR, 1);
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


void CTimeFilterParamEditorComp::SetTimeUnit(imtbase::ITimeFilterParam::TimeUnit timeUnit, imtbase::ITimeFilterParam::InterpretationMode mode, int mulitplier)
{
	imtbase::ITimeFilterParam* objectPtr = GetObjectPtr();
	Q_ASSERT(objectPtr != NULL);

	objectPtr->SetTimeUnit(timeUnit, mode, mulitplier);
}


} // namespace imtgui


