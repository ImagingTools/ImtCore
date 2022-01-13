#pragma once


// ImtCore includes
#include <imtbase/CObjectCollectionEventBase.h>


namespace imtbase
{


class CObjectCollectionInsertEvent: public CObjectCollectionEventBase
{
public:
	typedef CObjectCollectionEventBase BaseClass;

	CObjectCollectionInsertEvent(const QByteArray& itemId = QByteArray());

	// reimplemented (IObjectCollectionEvent)
	virtual EventType GetEventType() const override;

	// reimplemented (istd::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual IChangeable* CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
};


} // namespace imtbase


