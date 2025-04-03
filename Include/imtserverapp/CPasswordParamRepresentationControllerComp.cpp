#include <imtserverapp/CPasswordParamRepresentationControllerComp.h>


// ImtCore includes
#include <GeneratedFiles/imtbasesdl/SDL/1.0/CPP/Settings.h>


namespace imtserverapp
{


// protected methods

QByteArray CPasswordParamRepresentationControllerComp::GetTypeId() const
{
	return sdl::imtbase::ImtBaseTypes::CParamTypeIds::V1_0::ParamTypeIdsFields::PasswordParam.toUtf8();
}


} // namespace imtserverapp


