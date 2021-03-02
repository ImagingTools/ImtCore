#pragma once


// Qt includes
#include <QtCore/QDebug>
#include <QtCore/QThread>
#include <QtCore/QQueue>
#include <QtCore/QTimer>
#include <QtCore/QMultiMap>
#include <QtCore/QMutex>

// ACF includes
#include <iser/IVersionInfo.h>
#include <ifile/IFileNameParam.h>
#include <ilog/TLoggerCompWrap.h>
#include <ilog/TMessageDelegatorComp.h>
#include <ibase/TRuntimeStatusHanderCompWrap.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtfile/IFileCompression.h>
#include <imtlog/IGroupMessageIdsProvider.h>
#include <imtlog/ITimeRangeProvider.h>
#include <imtlog/IEventProvider.h>
#include <imtlog/IMessagesControllerParamsProvider.h>
#include <imtlog/CEventContainer.h>
#include <imtlog/CEventHistoryResultContainer.h>
#include <imtlog/CMessagesController.h>


namespace imtlog
{


class CEventHistoryControllerComp:
			public QObject,
			public ibase::TRuntimeStatusHanderCompWrap<
						ilog::TMessageDelegatorComp<ilog::CLoggerComponentBase>>,
			virtual public ilog::IMessageConsumer,
			virtual public IEventProvider,
			virtual public ITimeRangeProvider
{
	Q_OBJECT
public:
	typedef ibase::TRuntimeStatusHanderCompWrap<
				ilog::TMessageDelegatorComp<ilog::CLoggerComponentBase>> BaseClass;

	I_BEGIN_COMPONENT(CEventHistoryControllerComp)
		I_REGISTER_INTERFACE(ilog::IMessageConsumer);
		I_REGISTER_INTERFACE(IEventProvider);
		I_REGISTER_INTERFACE(ITimeRangeProvider);
		I_REGISTER_INTERFACE(IGroupMessageIdsProvider);
		I_ASSIGN(m_defaultMaxContainerMessageCountAttrPtr, "DefaultMaxContainerMessageCount", "Default max message count stored in container", true, 1000);
		I_ASSIGN(m_defaultContainerSavingPeriodAttrPtr, "DefaultContainerSavingPeriod", "Default period in seconds of saving the container until the maximum number of messages is reached", true, 60);
		I_ASSIGN(m_defaultContainerCachingTimeAttrPtr, "ContainerCachingTime", "The time the container was in the cache after closing", true, 3600);
		I_ASSIGN(m_logFolderCompPtr, "LogFolder", "Path to the event history folder", true, "");
		I_ASSIGN(m_compressorCompPtr, "FileCompressor", "File compressor", false, "");
		I_ASSIGN(m_versionInfoCompPtr, "VersionInfo", "Version info", true, "VersionInfo");
		I_ASSIGN(m_messagesControllerParamsProviderCompPtr, "MessagesControllerParamsProvider", "Message controller params provider", false, "");
	I_END_COMPONENT;

	CEventHistoryControllerComp();

	// reimplemented (imtlog::ITimeRangeProvider)
	virtual CTimeRange GetTimeRange() const override;

	// reimplemented (imtlog::IEventProvider)
	virtual IEventProvider::EventContainerPtr GetEvents(
				const IEventFilter* filterPtr = nullptr,
				const IMessageFilterParams* filterParamsPtr = nullptr) const override;

	// reimplemented (ilog::IMessageConsumer)
	virtual bool IsMessageSupported(
				int messageCategory = -1,
				int messageId = -1,
				const istd::IInformationProvider* messagePtr = NULL) const override;
	virtual void AddMessage(const MessagePtr& messagePtr) override;

	// reimplemented (ibase::TRuntimeStatusHanderCompWrap)
	virtual void OnSystemShutdown() override;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

private:
	void InitializeHistoryController();
	void InitializeMessagesController(int id);

private Q_SLOTS:
	void OnReadFinished(QByteArray requestId);

private:
	typedef istd::TSmartPtr<CMessagesController> MessagesControllerPtr;

	enum ControllerState
	{
		CS_INIT = 0,
		CS_OK,
		CS_SHUTDOWN,
		CS_FAILED
	};

	struct ReadRequest
	{
		IEventProvider::EventContainerPtr containerPtr;
		QMultiMap<qint64, ilog::IMessageConsumer::MessagePtr> events;
		QSet<QByteArray> readIds;
	};

private:
	I_ATTR(int, m_defaultMaxContainerMessageCountAttrPtr);
	I_ATTR(int, m_defaultContainerSavingPeriodAttrPtr);
	I_ATTR(int, m_defaultContainerCachingTimeAttrPtr);
	I_REF(ifile::IFileNameParam, m_logFolderCompPtr);
	I_REF(imtfile::IFileCompression, m_compressorCompPtr);
	I_REF(iser::IVersionInfo, m_versionInfoCompPtr);
	I_REF(IMessagesControllerParamsProvider, m_messagesControllerParamsProviderCompPtr);
	QDateTime m_systemStartTime;
	ControllerState m_controllerState;

	CTimeRange m_archiveTimeRange;

	QMap<int, MessagesControllerPtr> m_controllers;

	mutable QList<ReadRequest> m_requests;
	mutable QMutex m_requestMutex;

	imtlog::CEventContainer m_log;
	mutable QMutex m_logMutex;
};


} // namespace imtlog


