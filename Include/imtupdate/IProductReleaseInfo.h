// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <iser/ISerializable.h>


namespace imtupdate
{


class IProductReleaseInfo: virtual public iser::ISerializable
{


public:
	virtual const iser::IVersionInfo& GetProductVersion() const = 0;
	virtual QDateTime GetReleaseDate() const = 0;
	virtual QString GetChangeNotes() const = 0;
};


}//namespace imtupdate

