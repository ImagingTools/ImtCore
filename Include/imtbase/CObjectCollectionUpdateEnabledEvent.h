#pragma once


// ImtCore includes
#include <imtbase/CObjectCollectionEventBase.h>


namespace imtbase
{


class CObjectCollectionUpdateEnabledEvent: public CObjectCollectionEventBase
{
public:
	typedef CObjectCollectionEventBase BaseClass;

	explicit CObjectCollectionUpdateEnabledEvent(const QByteArray& itemId, bool enabled);
	bool IsEnabled();

private:
	bool m_enabled;
};


} // namespace imtbase


