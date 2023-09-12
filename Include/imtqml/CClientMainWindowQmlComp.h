#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtbase/ISystemStatus.h>
#include <imtbase/TModelUpdateBinder.h>
#include <imtqml/IQuickObject.h>


namespace imtqml
{


class CClientMainWindowQmlComp: public QObject, public ilog::CLoggerComponentBase
{
	Q_OBJECT
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CClientMainWindowQmlComp);
		I_ASSIGN(m_systemStatusCompPtr, "SystemStatus", "System status", true, "SystemStatus");
		I_ASSIGN(m_quickObjectCompPtr, "QuickObject", "Main QML Component", true, "QuickObject");
	I_END_COMPONENT;

	CClientMainWindowQmlComp();

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

private:
	void OnSystemStatusUpdate(const istd::IChangeable::ChangeSet& changeSet, const imtbase::ISystemStatus* objectPtr);

private Q_SLOTS:
	void UpdateSystemStatus();

private:
	I_REF(imtbase::ISystemStatus, m_systemStatusCompPtr);
	I_REF(imtqml::IQuickObject, m_quickObjectCompPtr);

private:
	imtbase::TModelUpdateBinder<imtbase::ISystemStatus, CClientMainWindowQmlComp> m_systemStatusObserver;
	imtbase::ISystemStatus::SystemStatus m_systemStatus;
};


} // namespace imtqml


