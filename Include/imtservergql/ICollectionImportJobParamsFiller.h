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


