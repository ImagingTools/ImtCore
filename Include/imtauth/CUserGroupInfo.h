#pragma once


// ImtCore includes
#include <imtauth/IUserGroupInfo.h>
#include <imtauth/CUserBaseInfo.h>


namespace imtauth
{


class CUserGroupInfo: virtual public IUserGroupInfo, public CUserBaseInfo
{
	typedef CUserBaseInfo BaseClass;

public:
	// reimplemented (IUserGroupInfo)
	virtual QString GetDescription() const override;
	virtual void SetDescription(const QString& description) override;
	virtual UserIds GetUsers() const override;
	virtual void SetUsers(const UserIds& users) override;
	virtual const imtauth::IUserInfoProvider* GetUserProvider() const override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive &archive) override;

	// reimplemented (iser::IChangeable)
	virtual bool CopyFrom(const IChangeable &object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual istd::IChangeable *CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

private:
	QString m_description;
	IUserGroupInfo::UserIds m_userIds;
};


} // namespace imtauth



