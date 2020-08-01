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

	CObjectCollectionUpdateEvent();
	CObjectCollectionUpdateEvent(const QByteArray& itemId, UpdateType updateType);
	UpdateType GetUpdateType() const;

	// reimplemented (istd::IChangeable)
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual bool IsEqual(const IChangeable& object) const override;
	virtual IChangeable* CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;

private:
	UpdateType m_updateType;
};


} // namespace imtbase


