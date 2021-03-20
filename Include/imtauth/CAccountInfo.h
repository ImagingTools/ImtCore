#pragma once


// ACF includes
#include <imod/TModelWrap.h>
#include <iimg/CBitmap.h>

// ImtCore includes
#include <imtauth/IAccountInfo.h>


namespace imtauth
{


class CAccountInfo: virtual public IAccountInfo, virtual public iauth::IRightsProvider
{
public:
	static QByteArray GetTypeId();

	CAccountInfo();

	// reimplemented (IAccountInfo)
	virtual AccountType GetAccountType() const override;
	virtual QString GetAccountName() const override;
	virtual QString GetAccountDescription() const override;
	virtual const IPersonInfo* GetAccountOwner() const override;
	virtual const iauth::IRightsProvider& GetAccountRights() const override;
	virtual const iimg::IBitmap& GetAccountPicture() const override;

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
	AccountType m_accountType;
	QString m_accountName;
	QString m_accountDescription;
	const IPersonInfo* m_accountOwnerPtr;
	imod::TModelWrap<iimg::CBitmap> m_accountPicture;
};


} // namespace imtauth


