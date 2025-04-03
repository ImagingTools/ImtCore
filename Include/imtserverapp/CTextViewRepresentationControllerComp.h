#pragma once


// ImtCore includes
#include <imtserverapp/CTextParamRepresentationControllerComp.h>


namespace imtserverapp
{


class CTextViewRepresentationControllerComp: public imtserverapp::CTextParamRepresentationControllerComp
{
public:
	typedef imtserverapp::CTextParamRepresentationControllerComp BaseClass;

	I_BEGIN_COMPONENT(CTextViewRepresentationControllerComp)
	I_END_COMPONENT;

protected:
	// reimplemented (imtserverapp::TJsonRepresentationControllerCompWrap<sdl::imtbase::ImtBaseTypes::CTextParam::V1_0>)
	virtual QByteArray GetTypeId() const override;
};


} // namespace imtbase


