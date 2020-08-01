#pragma once


// ImtCore includes
#include <imtbase/IObjectCollectionEvent.h>


namespace imtbase
{

/**
	Interface for object collection events
*/
class CObjectCollectionEventBase: virtual public IObjectCollectionEvent
{
public:
	CObjectCollectionEventBase();
	CObjectCollectionEventBase(const QByteArray& itemId);

	// reimplemented (imtbase::IObjectCollectionEvent)
	virtual QByteArray GetItemId() const override;

	// reimplemented (istd::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual bool IsEqual(const IChangeable& object) const override;
	virtual IChangeable* CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;

private:
	QByteArray m_itemId;
};


} // namespace imtbase


