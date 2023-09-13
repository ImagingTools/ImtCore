#include <imtapp/CSchedulerParamsRepresentationControllerComp.h>


// Qt includes
#include <QtCore/QTime>

// ImtCore includes
#include <imtapp/ISchedulerParams.h>


namespace imtapp
{


// protected methods

// reimplemented (imtbase::CObjectRepresentationControllerCompBase)

bool CSchedulerParamsRepresentationControllerComp::GetRepresentationFromValue(
		const istd::IChangeable& dataModel,
		imtbase::CTreeItemModel& representation,
		const iprm::IParamsSet* /*paramsPtr*/) const
{
	const imtapp::ISchedulerParams* schedulerParamsPtr = dynamic_cast<const imtapp::ISchedulerParams*>(&dataModel);
	Q_ASSERT(schedulerParamsPtr != nullptr);

	int interval = schedulerParamsPtr->GetInterval();
	representation.SetData("Interval", interval);

	QDateTime startTime = schedulerParamsPtr->GetStartTime();
	representation.SetData("StartTime", startTime.toString("dd-MM-yyyy HH:mm"));

	if (m_qmlPathAttrPtr.IsValid()){
		representation.SetData("Source", *m_qmlPathAttrPtr);
	}

	return true;
}


// reimplemented (IRepresentationController)

bool CSchedulerParamsRepresentationControllerComp::IsModelSupported(const istd::IChangeable& dataModel) const
{
	const imtapp::ISchedulerParams* schedulerParamsPtr = dynamic_cast<const imtapp::ISchedulerParams*>(&dataModel);
	if (schedulerParamsPtr != nullptr){
		return true;
	}

	return false;
}


bool CSchedulerParamsRepresentationControllerComp::GetDataModelFromRepresentation(const imtbase::CTreeItemModel& representation, istd::IChangeable& dataModel) const
{
	imtapp::ISchedulerParams* schedulerParamsPtr = dynamic_cast<imtapp::ISchedulerParams*>(&dataModel);
	Q_ASSERT(schedulerParamsPtr != nullptr);

	int interval = representation.GetData("Interval").toInt();
	schedulerParamsPtr->SetInterval(interval);

	QString startTime = representation.GetData("StartTime").toString();
	schedulerParamsPtr->SetStartTime(QDateTime::fromString(startTime, "dd-MM-yyyy HH:mm"));

	return true;
}


} // namespace imtapp


