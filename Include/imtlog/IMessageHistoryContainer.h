#pragma once


// Qt includes
//#include <QtCore/QList>

// ACF includes
#include <istd/IInformationProvider.h>
#include <istd/TIHierarchical.h>
#include <iser/ISerializable.h>

// ImtCore includes
#include <imtlog/IMessageHistoryConsumer.h>


namespace imtlog
{


class IMessageHistoryContainer: virtual public iser::ISerializable
{
public:
	typedef QList<IMessageHistoryConsumer::Message> Messages;

	virtual int GetWorstCategory() const = 0;
	virtual Messages GetMessages() const = 0;
	virtual void ClearMessages() = 0;
};


} // namespace imtlog

