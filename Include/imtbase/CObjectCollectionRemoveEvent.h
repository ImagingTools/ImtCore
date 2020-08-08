#pragma once


// ImtCore includes
#include <imtbase/CObjectCollectionEventBase.h>


namespace imtbase
{


class CObjectCollectionRemoveEvent: public CObjectCollectionEventBase
{
public:
	typedef CObjectCollectionEventBase BaseClass;

	CObjectCollectionRemoveEvent(const QByteArray& itemId = QByteArray());

	// reimplemented (istd::IChangeable)
	virtual IChangeable* CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
};


} // namespace imtbase


