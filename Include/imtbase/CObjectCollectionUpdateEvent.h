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
	CObjectCollectionUpdateEvent(const QByteArray& itemId, UpdateType updateType, const QVariant oldValue = QVariant(), const QVariant newValue = QVariant());
	UpdateType GetUpdateType() const;
	QVariant GetOldValue() const;
	QVariant GetNewValue() const;

	// reimplemented (IObjectCollectionEvent)
	virtual EventType GetEventType() const override;

	// reimplemented (istd::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual bool IsEqual(const IChangeable& object) const override;
	virtual IChangeable* CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;

private:
	UpdateType m_updateType;
	QVariant m_oldValue;
	QVariant m_newValue;
};


} // namespace imtbase


