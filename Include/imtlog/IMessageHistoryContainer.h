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


class IMessageHistoryContainer: virtual public iser::ISerializable
{
public:
	struct Message
	{
		uint64_t id;
		istd::TSmartPtr<const istd::IInformationProvider> messagePtr;
	};

	typedef QList<Message> Messages;

	virtual int GetWorstCategory() const = 0;
	virtual Messages GetMessages() const = 0;
	virtual void ClearMessages() = 0;
};


} // namespace imtlog

