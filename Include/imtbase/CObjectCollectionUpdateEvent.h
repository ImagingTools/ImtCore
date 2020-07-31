#pragma once


// ImtCore includes
#include <imtbase/CObjectCollectionEventBase.h>


namespace imtbase
{


class CObjectCollectionUpdateEvent: public CObjectCollectionEventBase
{
public:
	enum UpdateType
	{
		UT_DATA = 0,
		UT_NAME,
		UT_DESCRIPTION,
		UT_ENABLED
	};

	typedef CObjectCollectionEventBase BaseClass;

	explicit CObjectCollectionUpdateEvent(const QByteArray& itemId, UpdateType updateType);
	UpdateType GetUpdateType() const;

private:
	UpdateType m_updateType;
};


} // namespace imtbase


