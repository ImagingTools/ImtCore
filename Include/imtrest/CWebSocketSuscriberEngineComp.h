#pragma once


// ACF includes
#include <icomp/CComponentBase.h>
#include <ilog/TLoggerCompWrap.h>
#include <istd/TPointerVector.h>

// ImtCore includes
#include <imtauth/IContactInfo.h>
#include <imtrest/IRepresentationDataProvider.h>
#include <imtrest/ISuscriberEngine.h>
#include <imtrest/ISubscriber.h>


namespace imtrest
{


class CWebSocketSubscriberEngineComp:
			public ilog::CLoggerComponentBase,
			virtual public ISubscriberEngine
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CWebSocketSubscriberEngineComp);
		I_REGISTER_INTERFACE(imtrest::ISubscriberEngine);
//		I_ASSIGN(m_itemBasedRepresentationDataProvider, "ItemBasedRepresentationDataProvider", "Item based representation data provider", true, "ItemBasedRepresentationDataProvider");
	I_END_COMPONENT;

	CWebSocketSubscriberEngineComp();

	// reimplemented (ISubscriberEngine)
	virtual ISubscriber* RegisterSubscriber(
				QObject* socketPtr,
				const IRequestServlet& requestHandler,
				const IProtocolEngine& engine) override;
	virtual void UnRegisterSubscriber(QObject* socketObjectPtr) override;
	virtual const ISubscriber* GetSubscriber(const IRequest* requestPtr) const override;
	virtual int GetSubscriberCount() const override;
	virtual const ISubscriber* GetSubscriber(int index) const override;
	virtual void OnModelUpdate(QByteArray modelId, const istd::IChangeable::ChangeSet& changeSet) const override;

private:
	typedef istd::TPointerVector<ISubscriber> Suscribers;
	Suscribers m_subscribers;
};


} // namespace imtauthgui


