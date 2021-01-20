#pragma once


// Qt includes
#include <QtCore/QDebug>
#include <QtCore/QThread>
#include <QtCore/QQueue>
#include <QtCore/QTimer>

// ACF includes
#include <iser/IVersionInfo.h>
#include <ifile/IFileNameParam.h>
#include <ilog/TLoggerCompWrap.h>
#include <ilog/TMessageDelegatorComp.h>
#include <ibase/TRuntimeStatusHanderCompWrap.h>

// ImtCore includes
#include <imtfile/IFileCompression.h>
#include <imtlog/ITimeRangeProvider.h>
#include <imtlog/IEventProvider.h>
#include <imtlog/CEventContainer.h>
#include <imtlog/CEventHistoryGroupController.h>


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
		I_ASSIGN(m_logFolderCompPtr, "LogFolder", "Path to the event history folder", true, "");
		I_ASSIGN(m_compressorCompPtr, "FileCompressor", "File compressor", false, "");
		I_ASSIGN(m_versionInfoCompPtr, "VersionInfo", "Version info", true, "VersionInfo");
	I_END_COMPONENT;

	CEventHistoryControllerComp();

	// reimplemented (imtlog::ITimeRangeProvider)
	virtual CTimeRange GetTimeRange() const override;

	// reimplemented (imtlog::IEventProvider)
	virtual IEventProvider::EventContainerPtr GetEvents(
				const IEventFilter* filterPtr = nullptr,
				const imtlog::CTimeRange* timeRangePtr = nullptr,
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

public Q_SLOTS:
	void OnRequestFinished(QByteArray requestId);

private:
	typedef istd::TSmartPtr<CEventHistoryGroupController> EventHistoryGroupControllerPtr;

	EventHistoryGroupControllerPtr GetGroupForMessageId(int messageId) const;

private:
	enum ControllerState
	{
		CS_INIT = 0,
		CS_OK,
		CS_SHUTDOWN,
		CS_FAILED
	};

	struct GroupListItem
	{
		QList<int> messageIds;
		EventHistoryGroupControllerPtr groupPtr;
	};

	struct RequestMapItem
	{
		IEventProvider::EventContainerPtr resultEventsPtr;
		EventHistoryGroupControllerPtr groupPtr;
	};

private:
	I_REF(ifile::IFileNameParam, m_logFolderCompPtr);
	I_REF(imtfile::IFileCompression, m_compressorCompPtr);
	I_REF(iser::IVersionInfo, m_versionInfoCompPtr);

	QDateTime m_systemStartTime;
	ControllerState m_controllerState;

	CTimeRange m_archiveTimeRange;

	// First item should be GeneralGroup
	QList<GroupListItem> m_groups;

	mutable QMap<QByteArray, RequestMapItem> m_requests;
	mutable QMutex m_requestMutex;
};


} // namespace imtlog


