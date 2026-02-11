// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtbase/IMultiStatusProvider.h>


namespace imtbase
{


/**
	Interface for editing status list.
*/
class IMultiStatusManager: virtual public IMultiStatusProvider
{
public:
	/**
		Set status data from a given information provider.
	*/
	virtual void SetStatus(const QByteArray& statusId, const istd::IInformationProvider& status) = 0;
};


} // namespace imtbase


