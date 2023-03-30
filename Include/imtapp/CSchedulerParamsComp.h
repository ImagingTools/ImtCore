#pragma once


// ACF includes
#include <icomp/CComponentBase.h>
#include <ifile/IFileNameParam.h>

// ImtCore includes
#include <imtapp/CSchedulerParams.h>


namespace imtapp
{


class CSchedulerParamsComp:
			public icomp::CComponentBase,
			virtual public CSchedulerParams
{
public:
	typedef icomp::CComponentBase BaseClass;
	typedef CSchedulerParams BaseClass2;

	I_BEGIN_COMPONENT(CSchedulerParamsComp);
		I_REGISTER_INTERFACE(ISchedulerParams);
		I_REGISTER_INTERFACE(iser::IObject);
		I_REGISTER_INTERFACE(iser::ISerializable);
		I_REGISTER_INTERFACE(istd::IChangeable);
		I_ASSIGN(m_startTimeAttrPtr, "StartTime", "The time at which backup will start", true, "dd-MM-yyyy HH:mm");
		I_ASSIGN(m_intervalAttrPtr, "Interval", "Interval (in seconds)", true, 0);
	I_END_COMPONENT;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

private:
	I_ATTR(QString, m_startTimeAttrPtr);
	I_ATTR(int, m_intervalAttrPtr);
};


} // namespace imtapp


