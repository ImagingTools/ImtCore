#pragma once


// ImtCore includes
#include <imtbase/IObjectCollectionEvent.h>


namespace imtbase
{


/**
	Basic implementation for object collection events.
*/
class CObjectCollectionEventBase: virtual public IObjectCollectionEvent
{
public:
	CObjectCollectionEventBase(const QByteArray& itemId = QByteArray());

	// reimplemented (imtbase::IObjectCollectionEvent)
	virtual QByteArray GetItemId() const override;

	// reimplemented (istd::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual bool IsEqual(const IChangeable& object) const override;

private:
	QByteArray m_itemId;
};


} // namespace imtbase


