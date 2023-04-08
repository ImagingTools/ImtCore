#pragma once


// ImtCore includes
#include <imtauth/IUserGroupInfo.h>
#include <imtauth/CUserBaseInfo.h>
#include <imtbase/TIdentifiableWrap.h>


namespace imtauth
{


class CUserGroupInfo: virtual public IUserGroupInfo, public CUserBaseInfo
{
	typedef CUserBaseInfo BaseClass;

public:
	CUserGroupInfo();

	// reimplemented (IUserGroupInfo)
	virtual QString GetDescription() const override;
	virtual void SetDescription(const QString& description) override;
	virtual UserIds GetUsers() const override;
	virtual void SetUsers(const UserIds& users) override;
	virtual const imtauth::IUserInfoProvider* GetUserProvider() const override;
	virtual const IUserGroupInfo* GetParentGroup() const override;
	virtual const istd::TPointerVector<const IUserGroupInfo>& GetSubGroups() const  override;
	virtual bool InsertSubGroup(const IUserGroupInfo* subGroupInfo) override;
	virtual void DeleteSubGroup(const QByteArray& subGroupId) override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive &archive) override;

	// reimplemented (iser::IChangeable)
	virtual bool CopyFrom(const IChangeable &object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual istd::IChangeable *CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

private:
	QString m_description;
	IUserGroupInfo::UserIds m_userIds;

	UserGroupInfoList m_subGroups;
	IUserGroupInfo* m_parentGroupInfoPtr;
};


typedef imtbase::TIdentifiableWrap<CUserGroupInfo> CIdentifiableUserGroupInfo;


} // namespace imtauth



