#pragma once


// Qt includes
#include <istd/IChangeable.h>

// ImtCore includes
#include <imtlog/CMessageHistoryContainer.h>


namespace imtloggui
{


class CRepresentationEventsObject: public imtlog::CMessageHistoryContainer::Messages, virtual public istd::IChangeable
{

};


} // namespace imtloggui


