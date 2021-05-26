#pragma once


// Qt includes
#include <QtNetwork/QAbstractSocket>

// ACF includes
#include <iser/IVersionInfo.h>


namespace imtrest
{


class IRequest;
class IProtocolEngine;
class IRequestHandler;
class IResponse;
class ISender;
class ISubscriber;
//class IResponder;


/**
	Interface for core-level protocol definition.
*/
class ISubscriberEngine: virtual public istd::IPolymorphic
{
public:

	/**
		Create and register subscriber based on the incomming data.
		\param socketPtr		Socket instance for reading the incommming data
		\param engine
	*/
	virtual ISubscriber* RegisterSubscriber(
				QObject* socketPtr,
				const IRequestHandler& requestHandler,
				const IProtocolEngine& engine) = 0;

	/**
		Unregister subscriber based on the incomming data.
	*/
	virtual void UnRegisterSubscriber(
				QObject* socketPtr) = 0;

	/**
		Get sender subscriber using for sending responses to the clients.
	*/
	virtual const ISubscriber* GetSubscriber(const IRequest* requestPtr) const = 0;

	/**
		Get number of subscriber.
	*/
	virtual int GetCount() const = 0;

	/**
		Get sender subscriber using index
	*/
	virtual const ISubscriber* GetSubscriber(int index) const = 0;

	virtual void OnModelUpdate(QByteArray modelId, const istd::IChangeable::ChangeSet& changeSet) const = 0;


};


} // namespace imtrest


