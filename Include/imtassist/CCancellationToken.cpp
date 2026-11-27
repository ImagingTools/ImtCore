// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtassist/CCancellationToken.h>


namespace imtassist
{


CCancellationToken::CCancellationToken()
	:m_canceledPtr(std::make_shared<std::atomic<bool>>(false))
{
}


void CCancellationToken::Cancel()
{
	m_canceledPtr->store(true);
}


bool CCancellationToken::IsCanceled() const
{
	return m_canceledPtr->load();
}


} // namespace imtassist


