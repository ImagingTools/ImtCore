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

	// reimplemented (imtlog::IEventHistoryProvider)
	virtual IEventProvider::EventContainerPtr GetEvents(IEventProvider::EventFilterPtr filterPtr) const override;

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
	enum ControllerState
	{
		CS_INIT = 0,
		CS_OK,
		CS_SHUTDOWN,
		CS_FAILED
	};

	typedef istd::TSmartPtr<CEventHistoryGroupController> EventHistoryGroupControllerPtr;

	class Job:
				public imod::CMultiModelDispatcherBase,
				public imod::IModel
	{
		

	};

private:


private:
	I_REF(ifile::IFileNameParam, m_logFolderCompPtr);
	I_REF(imtfile::IFileCompression, m_compressorCompPtr);
	I_REF(iser::IVersionInfo, m_versionInfoCompPtr);

	QDateTime m_systemStartTime;
	ControllerState m_controllerState;

	CTimeRange m_archiveTimeRange;

	QMap<int, EventHistoryGroupControllerPtr> m_groupsMap;
	QList<EventHistoryGroupControllerPtr> m_groups;
	EventHistoryGroupControllerPtr m_generalGroup;
};


} // namespace imtlog


