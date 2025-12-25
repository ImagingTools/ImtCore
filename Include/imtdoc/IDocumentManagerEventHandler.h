#pragma once


// ImtCore includes
#include <imtdoc/CEventBase.h>


namespace imtdoc
{


class IDocumentManagerEventHandler : virtual public istd::IPolymorphic
{
public:
	virtual bool ProcessEvent(CEventBase* eventPtr) = 0;
};


} // namespace imtdoc


