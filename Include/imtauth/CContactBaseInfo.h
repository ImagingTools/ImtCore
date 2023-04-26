#pragma once


// ACF includes
#include <iimg/CBitmap.h>
#include <imod/TModelWrap.h>
#include <istd/TSmartPtr.h>
#include <imod/CModelUpdateBridge.h>
#include <iser/ISerializable.h>

// ImtCore includes
#include <imtauth/IContactBaseInfo.h>
#include <imtauth/CAddressCollection.h>


namespace imtauth
{


class CContactBaseInfo: virtual public IContactBaseInfo
{
public:
	// reimplemented (IContactBaseInfo)
	virtual QString GetName() const override;
	virtual void SetName(const QString& name) override;
	virtual QString GetDescription() const override;
	virtual void SetDescription(const QString& description) override;
	virtual QString GetEmail() const override;
	virtual void SetEmail(const QString& email) override;
	virtual const iimg::IBitmap& GetPicture() const override;
	virtual void SetPicture(const iimg::IBitmap& picture) override;
	virtual QByteArrayList GetGroups() const override;
	virtual bool AddGroup(const QByteArray& groupId) override;
	virtual bool RemoveGroup(const QByteArray& groupId) override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual IChangeable* CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

private:
	QString m_name;
	QString m_description;
	QString m_email;
	imod::TModelWrap<iimg::CBitmap> m_picture;
	imod::TModelWrap<CAddressCollection> m_addresses;
	QByteArrayList m_groupIds;
};


} // namespace imtauth


