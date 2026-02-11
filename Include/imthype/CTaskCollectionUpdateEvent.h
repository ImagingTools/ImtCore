// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IChangeable.h>


namespace imthype
{


class CTaskCollectionUpdateEvent: public istd::IChangeable
{
public:
	enum UpdateType
	{
		UT_USER_TASK_ID = 0,
		UT_TASK_INPUT,
	};

	CTaskCollectionUpdateEvent();
	CTaskCollectionUpdateEvent(
				const QByteArray& itemId,
				UpdateType updateType, const QVariant& oldValue = QVariant(), const QVariant& newValue = QVariant());
	UpdateType GetUpdateType() const;
	QVariant GetOldValue() const;
	QVariant GetNewValue() const;

	// reimplemented (istd::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual bool IsEqual(const IChangeable& object) const override;
	virtual istd::IChangeableUniquePtr CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;

private:
	QByteArray m_itemId;
	UpdateType m_updateType;
	QVariant m_oldValue;
	QVariant m_newValue;
};


} // namespace imthype


