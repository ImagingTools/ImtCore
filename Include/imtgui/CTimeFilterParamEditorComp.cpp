#include <imtgui/CTimeFilterParamEditorComp.h>


namespace imtgui
{


// public methods

CTimeFilterParamEditorComp::CTimeFilterParamEditorComp()
{
}


// protected methods

// reimplemented (iqtgui::TGuiObserverWrap)

void CTimeFilterParamEditorComp::UpdateGui(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	const imtbase::ITimeFilterParam* objectPtr = GetObservedObject();
	Q_ASSERT(objectPtr != nullptr);

	int multiplier = objectPtr->GetUnitMultiplier();

	CustomRangeFrame->setVisible(false);

	imtbase::ITimeFilterParam::TimeUnit timeUnit = objectPtr->GetTimeUnit();
	switch (timeUnit){
	case imtbase::ITimeFilterParam::TU_HOUR:
		switch (multiplier){
		case 1:
			TimeCombo->setCurrentIndex(1);
			break;
		case 4:
			TimeCombo->setCurrentIndex(2);
			break;
		case 8:
			TimeCombo->setCurrentIndex(3);
			break;
		}
		break;
	case imtbase::ITimeFilterParam::TU_DAY:
		switch (multiplier) {
		case 1:
			TimeCombo->setCurrentIndex(4);
			break;
		case 7:
			TimeCombo->setCurrentIndex(5);
			break;
		}
		break;
	case imtbase::ITimeFilterParam::TU_MONTH:
		switch (multiplier){
		case 1:
			TimeCombo->setCurrentIndex(6);
			break;
		case 3:
			TimeCombo->setCurrentIndex(7);
			break;
		}
		break;
	case imtbase::ITimeFilterParam::TU_YEAR:
		TimeCombo->setCurrentIndex(8);
		break;
	case imtbase::ITimeFilterParam::TU_CUSTOM:
		if (objectPtr->GetTimeRange().IsNull()){
			TimeCombo->setCurrentIndex(0);
		}
		else{
			CustomRangeFrame->setVisible(true);
			CustomBeginDate->setDateTime(objectPtr->GetTimeRange().GetBeginTime());
			CustomEndDate->setDateTime(objectPtr->GetTimeRange().GetEndTime());
			TimeCombo->setCurrentIndex(9);
		}
		break;
	}
}


// reimplemented (iqtgui::CGuiComponentBase)

void CTimeFilterParamEditorComp::OnGuiCreated()
{
	QBoxLayout* layoutPtr = nullptr;
	if(*m_horizontalOrientationFlag){
		layoutPtr = new QHBoxLayout();
	}
	else{
		layoutPtr = new QVBoxLayout();
	}

	layoutPtr->setContentsMargins(0, 0, 0, 0);

	GetWidget()->setLayout(layoutPtr);

	layoutPtr->addWidget(TimeCombo);

	layoutPtr->addWidget(CustomRangeFrame);
	CustomRangeFrame->setVisible(false);
	CustomBeginDate->setDateTime(QDateTime::currentDateTime().addMonths(-6));
	CustomEndDate->setDateTime(QDateTime::currentDateTime());

	BaseClass::OnGuiCreated();
}


void CTimeFilterParamEditorComp::OnGuiRetranslate()
{
	BaseClass::OnGuiRetranslate();

	UpdateGui(istd::IChangeable::GetAllChanges());
}


void CTimeFilterParamEditorComp::on_CustomBeginDate_dateTimeChanged(const QDateTime& /*dateTime*/)
{
	if (TimeCombo->currentIndex() == 9){
		SetFilterTimeRange(CustomBeginDate->dateTime(), CustomEndDate->dateTime());
	}
}


void CTimeFilterParamEditorComp::on_CustomEndDate_dateTimeChanged(const QDateTime & /*dateTime*/)
{
	if (TimeCombo->currentIndex() == 9){
		SetFilterTimeRange(CustomBeginDate->dateTime(), CustomEndDate->dateTime());
	}
}


// private slots

void CTimeFilterParamEditorComp::on_TimeCombo_currentIndexChanged(int index)
{
	CustomRangeFrame->setVisible(false);

	switch (index){
	case 0:
		SetTimeUnit(imtbase::ITimeFilterParam::TU_CUSTOM, imtbase::ITimeFilterParam::IM_FOR, 1);

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

		SetTimeUnit(imtbase::ITimeFilterParam::TU_CUSTOM, imtbase::ITimeFilterParam::IM_FOR, 1);

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


void CTimeFilterParamEditorComp::SetTimeUnit(
			imtbase::ITimeFilterParam::TimeUnit timeUnit,
			imtbase::ITimeFilterParam::InterpretationMode mode,
			int mulitplier)
{
	imtbase::ITimeFilterParam* objectPtr = GetObjectPtr();
	Q_ASSERT(objectPtr != NULL);

	objectPtr->SetTimeUnit(timeUnit, mode, mulitplier);
}


} // namespace imtgui


