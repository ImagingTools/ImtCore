#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtservergql/ICollectionImportJobParamsFiller.h>


namespace imtservergql
{


class CCollectionImportJobParamsFillerComp:
	public icomp::CComponentBase,
	virtual public ICollectionImportJobParamsFiller
{
	Q_DECLARE_TR_FUNCTIONS(CCollectionImportJobParamsFillerComp)
public:
	typedef CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CCollectionImportJobParamsFillerComp);
		I_REGISTER_INTERFACE(ICollectionImportJobParamsFiller);
	I_END_COMPONENT;

	// reimplemented (imtservergql::ICollectionImportJobParamsFiller)
	virtual bool FillCollectionImportJobParams(
		const imtservergql::ICollectionImportController::SessionInfo& sessionInfo,
		iprm::IParamsSet& jobParams) const override;
};


} // namespace imtservergql


