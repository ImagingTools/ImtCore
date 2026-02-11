// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IInformationProvider.h>


namespace imtbase
{


/**
	Common interface for editing a status.
*/
class IStatusManager: virtual public istd::IInformationProvider
{
public:
	virtual void SetInformationTimeStamp(const QDateTime& timestamp) = 0;
	virtual void SetInformationCategory(InformationCategory category) = 0;
	virtual void SetInformationId(int id) = 0;
	virtual void SetInformationDescription(const QString& description) = 0;
	virtual void SetInformationSource(const QString& source) = 0;
	virtual void SetInformationFlags(int flags) = 0;
};


} // namespace imtbase


