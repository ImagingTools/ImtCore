// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IPolymorphic.h>
#include <iprm/IParamsSet.h>

// imtbase includes
#include <imtbase/IOperationContext.h>


namespace imtbase
{


class IOperationContextController: virtual public istd::IPolymorphic
{
public:
	virtual imtbase::IOperationContext* CreateOperationContext(
				const QByteArray& operationTypeId,
				const QByteArray& objectId,
				const istd::IChangeable* objectPtr = nullptr,
				const iprm::IParamsSet* paramsPtr = nullptr) = 0;
};


} // namespace imtbase


