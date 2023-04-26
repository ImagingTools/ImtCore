#pragma once


// ACF includes
#include <iser/ISerializable.h>
#include <iimg/IBitmap.h>

// ImtCore includes
#include <imtauth/IAddressProvider.h>


namespace imtauth
{


class IContactBaseInfo: virtual public iser::ISerializable
{
public:
	virtual QString GetName() const = 0;
	virtual void SetName(const QString& name) = 0;
	virtual QString GetDescription() const = 0;
	virtual void SetDescription(const QString& description) = 0;
	virtual QString GetEmail() const = 0;
	virtual void SetEmail(const QString& email) = 0;
	virtual const iimg::IBitmap& GetPicture() const = 0;
	virtual void SetPicture(const iimg::IBitmap& picture) = 0;
	virtual QByteArrayList GetGroups() const = 0;
	virtual bool AddGroup(const QByteArray& groupId) = 0;
	virtual bool RemoveGroup(const QByteArray& groupId) = 0;
};


} // namespace imtauth


