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
	virtual const iprm::IParamsSet* GetParams() const override;
	virtual void SetParams(iprm::IParamsSet* paramsPtr) override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (iser::IChangeable)
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual bool IsEqual(const IChangeable& object) const override;
	virtual istd::IChangeable* CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

private:
	UserInfo m_userInfo;
	ActionTypeInfo m_actionTypeInfo;
	TargetInfo m_targetInfo;
	QDateTime m_timestamp;
	istd::TSmartPtr<iprm::IParamsSet> m_paramsPtr;
};


typedef imtbase::TIdentifiableWrap<CUserRecentAction> CIdentifiableUserRecentAction;


} // namespace imtauth


