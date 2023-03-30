#include <imtapp/CSchedulerParamsComp.h>


namespace imtapp
{


// protected methods

// reimplemented (icomp::CComponentBase)

void CSchedulerParamsComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	QDateTime time = QDateTime::fromString(*m_startTimeAttrPtr, "dd-MM-yyyy HH:mm");
	if (time.isValid()){
		BaseClass2::SetStartTime(time);
	}

	BaseClass2::SetInterval(*m_intervalAttrPtr);
}


} // namespace imtapp


