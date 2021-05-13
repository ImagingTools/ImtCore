#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtbase/IParamRepresentationController.h>


namespace imtbase
{


class CNumericParamRepresentationControllerComp:
			public icomp::CComponentBase,
			virtual public IParamRepresentationController
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CNumericParamRepresentationControllerComp);
		I_REGISTER_INTERFACE(IParamRepresentationController);
	I_END_COMPONENT;

	// reimplemented (IParamRepresentationController)
	virtual bool CreateTextRepresentation(const iser::ISerializable& param, QString& textRepresentation) const override;
};


} // namespace imtbase


