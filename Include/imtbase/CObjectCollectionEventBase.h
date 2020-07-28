#pragma once


// ACF includes
#include <imtbase/IObjectCollectionEvent.h>


namespace imtbase
{

/**
	Interface for object collection events
*/
class CObjectCollectionEventBase: virtual public IObjectCollectionEvent
{
public:
	explicit CObjectCollectionEventBase(const QByteArray& itemId);

	// reimplemented (imtbase::IObjectCollectionEvent)
	virtual QByteArray GetItemId() const override;

private:

	QByteArray m_itemId;
};


} // namespace imtbase


