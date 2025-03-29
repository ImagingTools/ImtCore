#pragma once


// ImtCore includes
#include <imtrest/CTextParamRepresentationControllerComp.h>


namespace imtrest
{


class CTextViewRepresentationControllerComp: public imtrest::CTextParamRepresentationControllerComp
{
public:
	typedef imtrest::CTextParamRepresentationControllerComp BaseClass;

	I_BEGIN_COMPONENT(CTextViewRepresentationControllerComp)
	I_END_COMPONENT;

protected:
	// reimplemented (imtrest::TJsonRepresentationControllerCompWrap<sdl::imtbase::ImtBaseTypes::CTextParam::V1_0>)
	virtual QByteArray GetTypeId() const override;
};


} // namespace imtbase


