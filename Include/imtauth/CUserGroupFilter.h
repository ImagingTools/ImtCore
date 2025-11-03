#pragma once


// ImtCore includes
#include <imtauth/IUserGroupFilter.h>


namespace imtauth
{


class CUserGroupFilter: virtual public IUserGroupFilter
{
public:
	// reimplemented (imtauth::IUserGroupFilter)
	virtual QByteArray GetUserId() const override;
	virtual void SetUserId(const QByteArray& userId) override;
	virtual QByteArrayList GetGroupIds() const override;
	virtual void SetGroupIds(const QByteArrayList& groupIds) override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual IChangeable* CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

private:
	QByteArray m_userId;
	QByteArrayList m_groupIds;
};


} // namespace imtauth


