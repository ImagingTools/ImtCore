#pragma once


// ImtCore includes
#include <imtbase/CObjectCollectionEventBase.h>


namespace imtbase
{


class CObjectCollectionUpdateEvent: public CObjectCollectionEventBase
{
public:
	typedef CObjectCollectionEventBase BaseClass;

	enum UpdateType
	{
		UT_DATA = 0,
		UT_NAME,
		UT_DESCRIPTION,
		UT_ENABLED
	};

	CObjectCollectionUpdateEvent();
	CObjectCollectionUpdateEvent(const QByteArray& itemId, UpdateType updateType);
	UpdateType GetUpdateType() const;

	// reimplemented (IObjectCollectionEvent)
	virtual int GetEventType() const override;

	// reimplemented (istd::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual bool IsEqual(const IChangeable& object) const override;
	virtual IChangeable* CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;

private:
	UpdateType m_updateType;
};


} // namespace imtbase


