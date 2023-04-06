#pragma once


// ACF includes
#include <imod/CModelUpdateBridge.h>

// ImtBase includes
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

	// reimplemented (ICompanyInfo)
	virtual QString GetCompanyName() const override;
	virtual void SetCompanyName(const QString& companyName) override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

private:
	QString m_accountName;
	QString m_accountDescription;
	CAddress m_address;
	QString m_mail;

	QString m_companyName;
};

}
