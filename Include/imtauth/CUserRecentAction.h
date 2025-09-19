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
	CUserRecentAction();

	// reimplemented (imtauth::IUserRecentAction)
	virtual QByteArray GetUserId() const override;
	virtual void SetUserId(const QByteArray& userId) override;
	virtual ActionType GetActionType() const override;
	virtual void SetActionType(ActionType actionType) override;
	virtual TargetInfo GetTargetInfo() const override;
	virtual void SetTargetInfo(TargetInfo targetInfo) override;
	virtual QDateTime GetTimestamp() const override;
	virtual void SetTimestamp(const QDateTime& timestamp) override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (iser::IChangeable)
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual bool IsEqual(const IChangeable& object) const override;
	virtual istd::IChangeable* CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

private:
	QByteArray m_userId;
	ActionType m_actionType;
	TargetInfo m_targetInfo;
	QDateTime m_timestamp;
};


typedef imtbase::TIdentifiableWrap<CUserRecentAction> CIdentifiableUserRecentAction;


} // namespace imtauth


