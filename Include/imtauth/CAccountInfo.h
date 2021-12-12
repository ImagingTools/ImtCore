#pragma once


// ACF includes
#include <iimg/CBitmap.h>
#include <imod/TModelWrap.h>
#include <istd/TSmartPtr.h>
#include <imod/CModelUpdateBridge.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtauth/IAccountInfo.h>
#include <imtauth/CContactInfo.h>


namespace imtauth
{


class CAccountInfo: virtual public IAccountInfo, virtual public iauth::IRightsProvider
{
public:
	static QByteArray GetTypeId();

	CAccountInfo();
	~CAccountInfo();

	// reimplemented (IAccountInfo)
	virtual AccountType GetAccountType() const override;
	virtual void SetAccountType(AccountType accountType) override;
	virtual QString GetAccountName() const override;
	virtual void SetAccountName(QString accountName) override;
	virtual QString GetAccountDescription() const override;
	virtual void SetAccountDescription(QString accountDescription) override;
	virtual const iimg::IBitmap& GetAccountPicture() const override;
	virtual void SetAccountPicture(const iimg::IBitmap& picture) override;
	virtual const IContactInfo* GetAccountOwner() const override;
	virtual void SetAccountOwner(const IContactInfo& ownerInfo) override;
	virtual const iauth::IRightsProvider& GetAccountRights() const override;

	// reimplemented (iauth::IRightsProvider)
	virtual bool HasRight(const QByteArray& operationId, bool beQuiet = false) const override;

	// reimplemented (iser::IObject)
	virtual QByteArray GetFactoryId() const override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual IChangeable* CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

private:
	AccountType m_accountType;
	QString m_accountName;
	QString m_accountDescription;
	imod::TModelWrap<iimg::CBitmap> m_accountPicture;

	imod::TModelWrap<CContactInfo> m_contact;
	imod::CModelUpdateBridge m_contactUpdateBridge;
};


} // namespace imtauth


