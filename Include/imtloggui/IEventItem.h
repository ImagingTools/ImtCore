#pragma once


// ACF includes
#include <idoc/IDocumentMetaInfo.h>
#include <ilog/IMessageConsumer.h>


namespace imtloggui
{


/**
	Interface for event item
*/
class IEventItem: virtual public idoc::IDocumentMetaInfo
{
public:
	/**
		Get pointer to source message
	*/
	virtual const istd::IInformationProvider* GetInformationProvider() const = 0;
	virtual const ilog::IMessageConsumer::MessagePtr GetMessage() const = 0;
};


} // namespace imtloggui


