#pragma once


// ImtCore includes
#include <imtbase/CObjectCollectionEventBase.h>


namespace imtbase
{


class CObjectCollectionUpdateDescriptionEvent: public CObjectCollectionEventBase
{
public:
	typedef CObjectCollectionEventBase BaseClass;

	explicit CObjectCollectionUpdateDescriptionEvent(const QByteArray& itemId, const QString& description);
	const QString& GetDescription() const;

private:
	QString m_description;
};


} // namespace imtbase


