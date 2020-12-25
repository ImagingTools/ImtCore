#pragma once


// Qt includes
#include <QtCore/QDateTime.h>

// Acf includes
#include <ilog/IMessageContainer.h>
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtloggui/IRepresentationFactory.h>
#include <imtlog/IMessageFilter.h>
#include <imtlog/IMessageHistoryProvider.h>
#include <imtlog/IStorage.h>
#include <imtloggui/CRepresentationProductionModel.h>


namespace imtloggui
{


class CRepresentationProductionFactoryComp:
			public icomp::CComponentBase,
			virtual public IRepresentationFactory,
			virtual public imtlog::IMessageHistoryConsumer
{
public:
	typedef CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CRepresentationProductionFactoryComp)
		I_REGISTER_INTERFACE(IRepresentationFactory);
		I_REGISTER_INTERFACE(IMessageHistoryConsumer);
		I_ASSIGN(m_granularityAttrPtr, "Granularity", "Statistics time granularity in seconds", true, 60);
		I_ASSIGN(m_messageHistoryProviderCompPtr, "MessageHistoryProvider", "Message history provider", true, "");
		I_ASSIGN(m_storageCompPtr, "Storage", "Storage", true, "Storage");
	I_END_COMPONENT

	// reimplemented (imtlog::IMessageHistoryConsumer)
	virtual bool IsMessageSupported(const istd::IInformationProvider* messagePtr = nullptr) const override;
	virtual void AddMessage(const MessagePtr& messagePtr) override;

	// reimplemented (imtloggui::IRepresentationViewFactory)
	virtual RepresentationObjectPtr CreateRepresentationObject(
				const imtlog::CTimeRange& timeRange,
				const QList<int>& messageIdList,
				imtlog::IMessageFilter::FilterMode filterMode) const override;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

private:
	I_ATTR(int, m_granularityAttrPtr);
	I_REF(imtlog::IMessageHistoryProvider, m_messageHistoryProviderCompPtr);
	I_REF(imtlog::IStorage, m_storageCompPtr);

	istd::TSmartPtr<istd::IChangeable> m_modelPtr;
};


} // namespace imtloggui


