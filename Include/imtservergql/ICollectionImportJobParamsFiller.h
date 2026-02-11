// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <iprm/IParamsSet.h>

// ImtCore includes
#include <imtservergql/ICollectionImportController.h>


namespace imtservergql
{


class ICollectionImportJobParamsFiller : virtual public istd::IPolymorphic
{
public:
	virtual bool FillCollectionImportJobParams(
		const imtservergql::ICollectionImportController::SessionInfo& sessionInfo,
		iprm::IParamsSet& jobParams) const = 0;
};


} // namespace imtservergql


