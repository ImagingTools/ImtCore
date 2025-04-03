#pragma once


// ImtCore includes
#include <imtserverapp/CTextParamRepresentationControllerComp.h>


namespace imtserverapp
{


class CPasswordParamRepresentationControllerComp: public imtserverapp::CTextParamRepresentationControllerComp
{
public:
	typedef imtserverapp::CTextParamRepresentationControllerComp BaseClass;

	I_BEGIN_COMPONENT(CPasswordParamRepresentationControllerComp)
	I_END_COMPONENT;

protected:
	virtual QByteArray GetTypeId() const override;
};


} // namespace imtbase


