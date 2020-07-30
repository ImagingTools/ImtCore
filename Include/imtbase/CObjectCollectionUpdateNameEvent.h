#pragma once


// ImtCore includes
#include <imtbase/CObjectCollectionEventBase.h>


namespace imtbase
{


class CObjectCollectionUpdateNameEvent: public CObjectCollectionEventBase
{
public:
	typedef CObjectCollectionEventBase BaseClass;

	explicit CObjectCollectionUpdateNameEvent(const QByteArray& itemId, const QString& name);
	const QString& GetName() const;

private:
	QString m_name;
};


} // namespace imtbase


