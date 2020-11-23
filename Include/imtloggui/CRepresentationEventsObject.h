#pragma once


// Qt includes
#include <istd/IChangeable.h>

// ImtCore includes
#include <imtlog/IMessageHistoryContainer.h>


namespace imtloggui
{


class CRepresentationEventsObject: public imtlog::IMessageHistoryContainer::Messages, virtual public istd::IChangeable
{

};


} // namespace imtloggui


