#pragma once


// ACF includes
#include <imod/CModelUpdateBridge.h>

// ImtCore includes
#include <imtbase/TIdentifiableWrap.h>
#include <imtauth/ICompanyInfo.h>


namespace imtauth
{


class CCompanyInfo: virtual public ICompanyInfo
{
public:
	// reimplemented (IAccountBaseInfo)
	virtual QString GetAccountName() const override;
	virtual void SetAccountName(const QString& accountName) override;
	virtual QString GetAccountDescription() const override;
	virtual void SetAccountDescription(const QString& accountDescription) override;
	virtual CAddress GetAddress() const override;
	virtual void SetAddress(const CAddress& address) override;
	virtual QString GetMail() const override;
	virtual void SetMail(const QString& mail) override;
	virtual QByteArrayList GetGroups() const override;
	virtual bool AddGroup(const QByteArray& groupId) override;
	virtual bool RemoveGroup(const QByteArray& groupId) override;

	// reimplemented (ICompanyInfo)
	virtual QString GetCompanyName() const override;
	virtual void SetCompanyName(const QString& companyName) override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (iser::IChangeable)
	virtual bool CopyFrom(const IChangeable &object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual istd::IChangeable *CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

private:
	QString m_accountName;
	QString m_accountDescription;
	CAddress m_address;
	QString m_mail;
	QString m_companyName;

	QByteArrayList m_groupIds;
};


typedef imtbase::TIdentifiableWrap<CCompanyInfo> CIdentifiableCompanyInfo;


}
