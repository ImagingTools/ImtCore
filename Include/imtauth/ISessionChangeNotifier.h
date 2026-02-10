// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IInformationProvider.h>
#include <istd/IChangeable.h>


namespace imtauth
{


class ISessionChangeNotifier: virtual public istd::IPolymorphic
{
public:
	virtual void OnSessionModelChanged(const istd::IChangeable::ChangeSet& changeSet, const QByteArray& sessionId) = 0;
};


} // namespace imtauth


