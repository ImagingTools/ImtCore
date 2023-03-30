#include <imtapp/CSchedulerParamsRepresentationControllerComp.h>


// ImtCore includes
#include <imtapp/ISchedulerParams.h>


namespace imtapp
{


// public methods

// reimplemented (IRepresentationController)

bool CSchedulerParamsRepresentationControllerComp::IsModelSupported(const istd::IChangeable& dataModel) const
{
	const imtapp::ISchedulerParams* schedulerParamsPtr = dynamic_cast<const imtapp::ISchedulerParams*>(&dataModel);
	if (schedulerParamsPtr != nullptr){
		return true;
	}

	return false;
}


bool CSchedulerParamsRepresentationControllerComp::GetRepresentationFromDataModel(const istd::IChangeable& dataModel, imtbase::CTreeItemModel& representation, const iprm::IParamsSet* paramsPtr) const
{
	if (!IsModelSupported(dataModel)){
		return false;
	}

	const imtapp::ISchedulerParams* schedulerParamsPtr = dynamic_cast<const imtapp::ISchedulerParams*>(&dataModel);
	if (schedulerParamsPtr != nullptr){
		representation.SetData("Id", *m_paramIdAttrPtr);

		if (m_paramNameAttrPtr.IsValid()){
			representation.SetData("Name", *m_paramNameAttrPtr);
		}

		int interval = schedulerParamsPtr->GetInterval();
		representation.SetData("Interval", interval);

		QDateTime startTime = schedulerParamsPtr->GetStartTime();
		representation.SetData("StartTime", startTime.toString("dd-MM-yyyy HH:mm"));

		if (m_qmlPathAttrPtr.IsValid()){
			representation.SetData("Source", *m_qmlPathAttrPtr);
		}

		return true;
	}

	return false;
}


bool CSchedulerParamsRepresentationControllerComp::GetDataModelFromRepresentation(const imtbase::CTreeItemModel& representation, istd::IChangeable& dataModel) const
{
	if (!IsModelSupported(dataModel)){
		return false;
	}

	imtapp::ISchedulerParams* schedulerParamsPtr = dynamic_cast<imtapp::ISchedulerParams*>(&dataModel);
	if (schedulerParamsPtr != nullptr){
		int interval = representation.GetData("Interval").toInt();
		schedulerParamsPtr->SetInterval(interval);

		QString startTime = representation.GetData("StartTime").toString();
		schedulerParamsPtr->SetStartTime(QDateTime::fromString(startTime, "dd-MM-yyyy HH:mm"));

		return true;
	}

	return false;
}


} // namespace imtapp


