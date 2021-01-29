#pragma once


// Qt includes
#include <QtCore/QThread>

// Acf includes
#include <imod/TModelWrap.h>
#include <imod/TSingleModelObserverBase.h>
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtlog/IEventFilter.h>
#include <imtlog/IEventProvider.h>
#include <imtlog/IMessageFilterParams.h>
#include <imtlog/ITimeRangeProvider.h>
#include <imtloggui/CRepresentationControllerWorkerBase.h>


namespace imtloggui
{


class CRepresentationControllerCompBase:
			public QObject,
			public icomp::CComponentBase,
			protected imod::TSingleModelObserverBase<imtlog::ITimeRangeProvider>
{
	Q_OBJECT
public:
	typedef CComponentBase BaseClass;

	I_BEGIN_BASE_COMPONENT(CRepresentationControllerCompBase)
		I_REGISTER_INTERFACE(imod::IObserver);
		I_ASSIGN(m_eventProviderCompPtr, "EventProvider", "Event provider", true, "EventProvider");
		I_ASSIGN_TO(m_eventProviderModelCompPtr, m_eventProviderCompPtr, true);
		I_ASSIGN(m_timeRangeProviderCompPtr, "TimeRangeProvider", "TimeRangeProvider", false, "TimeRangeProvider");
		I_ASSIGN_TO(m_timeRangeProviderModelCompPtr, m_timeRangeProviderCompPtr, true);
		I_ASSIGN(m_representationCompPtr, "RepresentationData", "Representation data model", true, "RepresentationData");
		I_ASSIGN(m_eventFilterCompPtr, "EventFilter", "Event filter", false, "EventFilter");
		I_ASSIGN(m_messageFilterParamsCompPtr, "MessageFilterParams", "Message filter params", false, "MessageFilterParams");
	I_END_COMPONENT

	CRepresentationControllerCompBase();

protected:
	virtual void BuildRepresentation(
				istd::IChangeable& representation,
				imtlog::IEventProvider::EventContainerPtr containerPtr,
				const imtlog::CTimeRange& timeRange) const = 0;

	// reimplemented (imod::CSingleModelObserverBase)
	virtual void OnUpdate(const istd::IChangeable::ChangeSet& changeSet) override;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated();
	virtual void OnComponentDestroyed();

protected:
	I_REF(imtlog::IEventProvider, m_eventProviderCompPtr);
	I_REF(imod::IModel, m_eventProviderModelCompPtr);
	I_REF(imtlog::ITimeRangeProvider, m_timeRangeProviderCompPtr);
	I_REF(imod::IModel, m_timeRangeProviderModelCompPtr);
	I_REF(istd::IChangeable, m_representationCompPtr);
	I_REF(imtlog::IEventFilter, m_eventFilterCompPtr);
	I_REF(imtlog::IMessageFilterParams, m_messageFilterParamsCompPtr);

private Q_SLOTS:
	void OnTaskFinished(QByteArray taskId);

private:
	class EventProviderObserver: public imod::TSingleModelObserverBase<imtlog::IEventProvider>
	{
	public:
		EventProviderObserver(CRepresentationControllerCompBase& parent);

	protected:
		// reimplemented (imod::CSingleModelObserverBase)
		virtual void OnUpdate(const istd::IChangeable::ChangeSet& changeSet) override;
	private:
		CRepresentationControllerCompBase& m_parent;
	};

	class CreateRepresentationTask:
				public imod::TSingleModelObserverBase<ilog::IMessageContainer>,
				public istd::IChangeable
	{
	public:
		imtlog::CTimeRange timeRange;
		imtlog::IEventProvider* eventProviderPtr;
		imtlog::IEventFilter* eventFilterPtr;
		imtlog::IMessageFilterParams* messageFilterParamsPtr;
		CRepresentationControllerCompBase* controllerPtr;
		istd::IChangeable* representationPtr;
		uint8_t updateCount;

	protected:
		virtual void OnUpdate(const istd::IChangeable::ChangeSet& changeSet) override
		{
			updateCount++;
		}
	};

	class Worker: public CWorker
	{
	public:
		Worker(CRepresentationControllerCompBase& parent);

	protected:
		virtual void run() override;

	private:
		CRepresentationControllerCompBase& m_parent;
	};

private:
	EventProviderObserver m_eventProviderObserver;

	Worker m_worker;
	imtlog::CTimeRange m_workerJob;
	QMutex m_workerJobMutex;
};


} // namespace imtloggui


