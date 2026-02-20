// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
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


