#pragma once


// Qt includes
#include <QtCore/QThread>

// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtbase/ISystemStatus.h>
#include <imtcom/IConnectionStatusProvider.h>
#include <imtdb/IDatabaseServerConnectionChecker.h>


namespace imtbase
{


class CSystemStatusComp:
			public QObject,
			public ilog::CLoggerComponentBase,
			virtual public imtbase::ISystemStatus
{
	Q_OBJECT
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CSystemStatusComp);
		I_REGISTER_INTERFACE(imtbase::ISystemStatus);
		I_ASSIGN(m_connectionStatusProviderCompPtr, "ConnectionStatusProvider", "Server connection status provider", true, "ConnectionStatusProvider");
		I_ASSIGN(m_dbServerConnectionCheckerCompPtr, "DatabaseServerConnectionChecker", "Database server connection status provider", true, "DatabaseServerConnectionChecker");
	I_END_COMPONENT;

	CSystemStatusComp();

	// reimplemented (ISystemStatus)
	virtual SystemStatus GetSystemStatus(QString& errorMessage) const override;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

private:
	void SetStatus(SystemStatus status);
	void CheckStatus();
	void OnCheckStatusFinished();

	class CheckStatusThread: public QThread
	{
	public:
		CheckStatusThread(CSystemStatusComp& parent);

		void Start();
		SystemStatus GetStatus();
		QString GetErrorMessage();

	protected:
		// reimplemented (QThread)
		virtual void run() override;

	private:
		CSystemStatusComp& m_parent;
		SystemStatus m_status;
		QString m_errorMessage;
	};

private:
	SystemStatus m_status;
	QString m_errorMessage;
	CheckStatusThread m_checkStatusThread;

private:
	I_REF(imtcom::IConnectionStatusProvider, m_connectionStatusProviderCompPtr);
	I_REF(imtdb::IDatabaseServerConnectionChecker, m_dbServerConnectionCheckerCompPtr);
};


} // namespace imtbase


