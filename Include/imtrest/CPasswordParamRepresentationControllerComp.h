#pragma once


// ImtCore includes
#include <imtrest/CTextParamRepresentationControllerComp.h>


namespace imtrest
{


class CPasswordParamRepresentationControllerComp: public imtrest::CTextParamRepresentationControllerComp
{
public:
	typedef imtrest::CTextParamRepresentationControllerComp BaseClass;

	I_BEGIN_COMPONENT(CPasswordParamRepresentationControllerComp)
	I_END_COMPONENT;

protected:
	virtual QByteArray GetTypeId() const override;
};


} // namespace imtbase


