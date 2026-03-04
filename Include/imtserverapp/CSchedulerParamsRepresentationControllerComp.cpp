// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtserverapp/CSchedulerParamsRepresentationControllerComp.h>


// ImtCore includes
#include <imtapp/ISchedulerParams.h>


namespace imtserverapp
{


// protected methods

// reimplemented (imtserverapp::TJsonRepresentationControllerCompWrap<sdl::imtbase::ImtBaseTypes::CSchedulerParam::V1_0>)

QByteArray CSchedulerParamsRepresentationControllerComp::GetTypeId() const
{
	return sdl::imtbase::ImtBaseTypes::CParamTypeIds::V1_0::ParamTypeIdsFields::SchedulerParam.toUtf8();
}


bool CSchedulerParamsRepresentationControllerComp::IsModelSupported(const istd::IChangeable &dataModel) const
{
	const imtapp::ISchedulerParams* schedulerParamsPtr = dynamic_cast<const imtapp::ISchedulerParams*>(&dataModel);

	return schedulerParamsPtr != nullptr;
}


bool CSchedulerParamsRepresentationControllerComp::GetSdlRepresentationFromDataModel(
			sdl::imtbase::ImtBaseTypes::CSchedulerParam::V1_0& sdlRepresentation,
			const istd::IChangeable& dataModel,
			const iprm::IParamsSet* /*paramsPtr*/) const
{
	const imtapp::ISchedulerParams* schedulerParamsPtr = dynamic_cast<const imtapp::ISchedulerParams*>(&dataModel);
	Q_ASSERT(schedulerParamsPtr != nullptr);
	if (schedulerParamsPtr == nullptr){
		return false;
	}

	sdlRepresentation.interval = schedulerParamsPtr->GetInterval();

	QDateTime startTime = schedulerParamsPtr->GetStartTime();

	QString format = m_dateFormatAttrPtr.IsValid() ? *m_dateFormatAttrPtr : "dd-MM-yyyy HH:mm";
	sdlRepresentation.startTime = startTime.toString(format);

	return true;
}


bool CSchedulerParamsRepresentationControllerComp::GetDataModelFromSdlRepresentation(
			istd::IChangeable& dataModel,
			const sdl::imtbase::ImtBaseTypes::CSchedulerParam::V1_0& sdlRepresentation) const
{
	imtapp::ISchedulerParams* schedulerParamsPtr = dynamic_cast<imtapp::ISchedulerParams*>(&dataModel);
	Q_ASSERT(schedulerParamsPtr != nullptr);
	if (schedulerParamsPtr == nullptr){
		return false;
	}

	if (!sdlRepresentation.interval || !sdlRepresentation.startTime){
		return false;
	}

	int interval = *sdlRepresentation.interval;
	schedulerParamsPtr->SetInterval(interval);

	QString startTime = *sdlRepresentation.startTime;

	QString format = m_dateFormatAttrPtr.IsValid() ? *m_dateFormatAttrPtr : "dd-MM-yyyy HH:mm";
	QDateTime startDateTime = QDateTime::fromString(startTime, format);

	schedulerParamsPtr->SetStartTime(startDateTime);

	return true;
}


} // namespace imtserverapp


