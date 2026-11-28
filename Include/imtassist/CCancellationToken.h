// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// STL includes
#include <atomic>
#include <memory>


namespace imtassist
{


/**
	Thread-safe cancellation flag shared between copies.
	Cancelling stops further processing; it does not undo a change that was already applied.

	\ingroup imtassist
*/
class CCancellationToken
{
public:
	CCancellationToken();

	void Cancel();
	bool IsCanceled() const;

private:
	std::shared_ptr<std::atomic<bool>> m_canceledPtr;
};


} // namespace imtassist


