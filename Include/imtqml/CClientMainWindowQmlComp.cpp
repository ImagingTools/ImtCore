#include <imtqml/CClientMainWindowQmlComp.h>


namespace imtqml
{


CClientMainWindowQmlComp::CClientMainWindowQmlComp()
	:m_systemStatusObserver(*this),
	m_systemStatus(imtbase::ISystemStatus::SystemStatus::SS_UNKNOWN_ERROR)
{
}


// protected methods

// reimplemented (icomp::CComponentBase)

void CClientMainWindowQmlComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_checkSystemStatusAttrPtr.IsValid() && *m_checkSystemStatusAttrPtr){
		if (m_systemStatusCompPtr.IsValid()){
			m_systemStatusObserver.RegisterObject(m_systemStatusCompPtr.GetPtr(), &CClientMainWindowQmlComp::OnSystemStatusUpdate);
		}
	}
}


void CClientMainWindowQmlComp::OnComponentDestroyed()
{
	m_systemStatusObserver.UnregisterAllObjects();

	BaseClass::OnComponentDestroyed();
}


// private methods

void CClientMainWindowQmlComp::OnSystemStatusUpdate(
			const istd::IChangeable::ChangeSet& /*changeSet*/,
			const imtbase::ISystemStatus* objectPtr)
{
	if (objectPtr == nullptr){
		return;
	}

	if (!m_quickObjectCompPtr.IsValid()){
		return;
	}

	QQuickItem* quickItem = m_quickObjectCompPtr->GetQuickItem();
	if (quickItem == nullptr){
		return;
	}

	QString error;
	imtbase::ISystemStatus::SystemStatus status = objectPtr->GetSystemStatus(error);

	if (m_systemStatus != status){
		m_systemStatus = status;

		QString statusStr;

		switch(status){
		case imtbase::ISystemStatus::SystemStatus::SS_NO_ERROR:
			statusStr = "NO_ERROR";

			break;
		case imtbase::ISystemStatus::SystemStatus::SS_UNKNOWN_ERROR:
			statusStr = "UNKNOWN_ERROR";

			break;
		case imtbase::ISystemStatus::SystemStatus::SS_SERVER_CONNECTION_ERROR:
			statusStr = "CONNECTION_ERROR";

			break;
		case imtbase::ISystemStatus::SystemStatus::SS_DATABASE_CONNECTION_ERROR:
			statusStr = "DATABASE_CONNECTION_ERROR";

			break;
		}

		QMetaObject::invokeMethod(quickItem, "setSystemStatus",
					Q_ARG(QVariant, QVariant::fromValue(statusStr)),
					Q_ARG(QVariant, QVariant::fromValue(error)));
	}
}


} // namespace imtqml


