// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QDateTime>

// ImtCore includes
#include <imtbase/TIdentifiableWrap.h>
#include <imtauth/IUserRecentAction.h>


namespace imtauth
{


class CUserRecentAction: virtual public IUserRecentAction
{
public:
	// reimplemented (imtauth::IUserRecentAction)
	virtual UserInfo GetUserInfo() const override;
	virtual void SetUserInfo(UserInfo userInfo) override;
	virtual ActionTypeInfo GetActionTypeInfo() const override;
	virtual void SetActionTypeInfo(ActionTypeInfo actionTypeInfo) override;
	virtual TargetInfo GetTargetInfo() const override;
	virtual void SetTargetInfo(TargetInfo targetInfo) override;
	virtual QDateTime GetTimestamp() const override;
	virtual void SetTimestamp(const QDateTime& timestamp) override;
	virtual iser::ISerializableSharedPtr GetActionData() const override;
	virtual void SetActionData(const iser::ISerializableSharedPtr& actionDataPtr) override;
	virtual void SetActionDataFactory(const ActionDataFactoryFunction& factory) override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (iser::IChangeable)
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual bool IsEqual(const IChangeable& object) const override;
	virtual istd::IChangeableUniquePtr CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

private:
	UserInfo m_userInfo;
	ActionTypeInfo m_actionTypeInfo;
	TargetInfo m_targetInfo;
	QDateTime m_timestamp;
	iser::ISerializableSharedPtr m_actionDataPtr;
	ActionDataFactoryFunction m_actionDataFactory;
};


typedef imtbase::TIdentifiableWrap<CUserRecentAction> CIdentifiableUserRecentAction;


} // namespace imtauth


