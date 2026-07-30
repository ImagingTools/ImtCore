// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <iser/IObject.h>
#include <istd/TSmartPtr.h>

// ImtCore includes
#include <imtauth/IDocumentShare.h>


namespace imtauth
{


/**
	Serializable data holder for a single document share.
	Used as the stored representation of a DocumentShareInfo inside an object
	collection (e.g. an SQL-backed repository).
*/
class IDocumentShareData: virtual public iser::IObject
{
public:
	/**
		Get the full share description.
	*/
	virtual DocumentShareInfo GetShareInfo() const = 0;

	/**
		Set the full share description.
	*/
	virtual void SetShareInfo(const DocumentShareInfo& shareInfo) = 0;

	/**
		Convenience accessor for the share ID, which is used as the
		collection element key.
	*/
	virtual QByteArray GetShareId() const = 0;
};


typedef istd::TUniqueInterfacePtr<IDocumentShareData> IDocumentShareDataUniquePtr;
typedef istd::TSharedInterfacePtr<IDocumentShareData> IDocumentShareDataSharedPtr;


} // namespace imtauth
