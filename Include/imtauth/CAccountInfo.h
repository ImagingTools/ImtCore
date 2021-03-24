#pragma once


// ACF includes
#include <iimg/CBitmap.h>
#include <imod/TModelWrap.h>
#include <istd/TSmartPtr.h>

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

	// reimplemented (IAccountInfo)
	virtual AccountType GetAccountType() const override;
	virtual void SetAccountType(AccountType accountType) override;
	virtual QString GetAccountName() const override;
	virtual void SetAccountName(QString accountName) override;
	virtual QString GetAccountDescription() const override;
	virtual void SetAccountDescription(QString accountDescription) override;
	virtual const iimg::IBitmap& GetAccountPicture() const override;
	virtual void SetAccountPicture(const iimg::IBitmap& picture) override;
	virtual QString GetAccountOwnerEMail() const override;
	virtual ContactInfoPtr GetAccountOwner() const override;
	virtual void SetAccountOwner(const QString& contactInfo) override;
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

protected:
	void OnContactCollectionUpdate(
				const istd::IChangeable::ChangeSet& changeSet,
				const imtbase::IObjectCollection* productCollectionPtr);

protected:
	imtbase::IObjectCollection* m_contactCollectionPtr;
	QString m_contactEMail;

private:
	AccountType m_accountType;
	QString m_accountName;
	QString m_accountDescription;
	imod::TModelWrap<iimg::CBitmap> m_accountPicture;
};


} // namespace imtauth


