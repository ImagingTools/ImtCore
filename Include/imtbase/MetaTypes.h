// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IChangeable.h>
#include <istd/TSmartPtr.h>


namespace imtbase
{


typedef istd::TSmartPtr<istd::IChangeable> IChangeablePtr;


} // namespace imtbase


Q_DECLARE_METATYPE(imtbase::IChangeablePtr);


