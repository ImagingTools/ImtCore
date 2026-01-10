#pragma once


// ImtCore includes
#include <imtserverapp/CTextParamRepresentationControllerComp.h>


namespace imtserverapp
{


class CTextViewRepresentationControllerComp: public CTextParamRepresentationControllerComp
{
public:
	typedef CTextParamRepresentationControllerComp BaseClass;

	I_BEGIN_COMPONENT(CTextViewRepresentationControllerComp)
	I_END_COMPONENT;

protected:
	// reimplemented (TJsonRepresentationControllerCompWrap<sdl::imtbase::ImtBaseTypes::CTextParam::V1_0>)
	virtual QByteArray GetTypeId() const override;
};


} // namespace imtbase


