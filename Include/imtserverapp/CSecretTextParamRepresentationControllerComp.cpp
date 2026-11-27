// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtserverapp/CSecretTextParamRepresentationControllerComp.h>
#include <GeneratedFiles/imtbasesdl/SDL/1.0/CPP/Settings.h>


// ImtCore includes
#include <imtcrypt/ISecretTextParam.h>


namespace imtserverapp
{


const QString CSecretTextParamRepresentationControllerComp::s_placeholder = QStringLiteral("********");


// protected methods

// reimplemented (imtserverapp::CTextParamRepresentationControllerComp)

bool CSecretTextParamRepresentationControllerComp::IsModelSupported(const istd::IChangeable& dataModel) const
{
	return dynamic_cast<const imtcrypt::ISecretTextParam*>(&dataModel) != nullptr;
}


bool CSecretTextParamRepresentationControllerComp::GetSdlRepresentationFromDataModel(
			sdl::V1_0::imtbase::CTextParam& sdlRepresentation,
			const istd::IChangeable& dataModel,
			const iprm::IParamsSet* /*paramsPtr*/) const
{
	const imtcrypt::ISecretTextParam* secretParamPtr = dynamic_cast<const imtcrypt::ISecretTextParam*>(&dataModel);
	if (secretParamPtr == nullptr){
		return false;
	}

	sdlRepresentation.text = secretParamPtr->GetText().isEmpty() ? QString() : s_placeholder;

	return true;
}


bool CSecretTextParamRepresentationControllerComp::GetDataModelFromSdlRepresentation(
			istd::IChangeable& dataModel,
			const sdl::V1_0::imtbase::CTextParam& sdlRepresentation) const
{
	imtcrypt::ISecretTextParam* secretParamPtr = dynamic_cast<imtcrypt::ISecretTextParam*>(&dataModel);
	if ((secretParamPtr == nullptr) || !sdlRepresentation.text){
		return false;
	}

	QString text = *sdlRepresentation.text;
	if (text == s_placeholder){
		return true;
	}

	secretParamPtr->SetText(text.trimmed());

	return true;
}


} // namespace imtserverapp


