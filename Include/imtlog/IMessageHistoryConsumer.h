#pragma once


// Qt includes
//#include <QtCore/QList>

// ACF includes
#include <istd/IInformationProvider.h>
#include <istd/TIHierarchical.h>
#include <iser/ISerializable.h>
#include <ilog/IMessageConsumer.h>


namespace imtlog
{


class IMessageHistoryConsumer: virtual public istd::IPolymorphic
{
public:
	struct Message
	{
		uint64_t id;
		istd::TSmartPtr<const istd::IInformationProvider> messagePtr;
	};

	typedef istd::TSmartPtr<Message> MessagePtr;

	virtual bool IsMessageSupported(
				int messageCategory = -1,
				int messageId = -1,
				const istd::IInformationProvider* messagePtr = NULL) const = 0;
	virtual void AddMessage(const MessagePtr& messagePtr) = 0;
};


} // namespace imtlog

