#pragma once


// Qt includes
#include <QtCore/QString>

// ACF includes
#include <istd/IInformationProvider.h>
#include <ilog/IMessageConsumer.h>


namespace imtbase
{


class IOperationStatusHandler: virtual public istd::IPolymorphic
{
public:
	struct ProcessingStatus
	{
		istd::IInformationProvider::InformationCategory status;
		bool accepted = false;
		QString operationSummary;
	};

	virtual ProcessingStatus OnOperationFinished(const ilog::IMessageConsumer* logPtr) = 0;
};


} // namespace imtbase


