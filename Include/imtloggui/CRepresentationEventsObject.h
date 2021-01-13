#pragma once


// Qt includes
#include <istd/IChangeable.h>

// ImtCore includes
#include <imtlog/CEventContainer.h>


namespace imtloggui
{


class CRepresentationEventsObject: public imtlog::CEventContainer::Messages, virtual public istd::IChangeable
{

};


} // namespace imtloggui


