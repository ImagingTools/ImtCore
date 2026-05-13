// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtgql/CGqlRequestContextManager.h>


namespace imtgql
{


thread_local istd::TSharedInterfacePtr<IGqlContext> CGqlRequestContextManager::s_context;


} // namespace imtgql
