#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtbase/ISummaryRepresentationController.h>


namespace imtbase
{


class CNumericParamSummaryRepresentationControllerComp:
			public icomp::CComponentBase,
			virtual public ISummaryRepresentationController
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CNumericParamSummaryRepresentationControllerComp);
		I_REGISTER_INTERFACE(ISummaryRepresentationController);
	I_END_COMPONENT;

	// reimplemented (ISummaryRepresentationController)
	virtual bool CreateSummaryRepresentation(const istd::IChangeable& param, QString& textRepresentation) const override;
};


} // namespace imtbase


