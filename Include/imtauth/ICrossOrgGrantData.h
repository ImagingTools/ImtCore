// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <iser/IObject.h>
#include <istd/TSmartPtr.h>

// ImtCore includes
#include <imtauth/ICrossOrgGrant.h>


namespace imtauth
{


/**
	Serializable data holder for a single cross-org grant.
	Used as the stored representation of a CrossOrgGrantInfo inside an
	object collection (e.g. an SQL-backed repository).
*/
class ICrossOrgGrantData: virtual public iser::IObject
{
public:
	/**
		Get the full grant description.
	*/
	virtual CrossOrgGrantInfo GetGrantInfo() const = 0;

	/**
		Set the full grant description.
	*/
	virtual void SetGrantInfo(const CrossOrgGrantInfo& grantInfo) = 0;

	/**
		Convenience accessor for the grant ID, which is used as the
		collection element key.
	*/
	virtual QByteArray GetGrantId() const = 0;
};


typedef istd::TUniqueInterfacePtr<ICrossOrgGrantData> ICrossOrgGrantDataUniquePtr;
typedef istd::TSharedInterfacePtr<ICrossOrgGrantData> ICrossOrgGrantDataSharedPtr;


} // namespace imtauth
