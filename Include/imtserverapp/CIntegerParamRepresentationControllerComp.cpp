// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtserverapp/CIntegerParamRepresentationControllerComp.h>


// ACF includes
#include <iprm/ITextParam.h>


namespace imtserverapp
{


// protected methods

// reimplemented (imtserverapp::TJsonRepresentationControllerCompWrap<sdl::imtbase::ImtBaseTypes::CIntegerParam::V1_0>)

QByteArray CIntegerParamRepresentationControllerComp::GetTypeId() const
{
	return sdl::imtbase::ImtBaseTypes::CParamTypeIds::V1_0::ParamTypeIdsFields::IntegerParam.toUtf8();
}


bool CIntegerParamRepresentationControllerComp::IsModelSupported(const istd::IChangeable& dataModel) const
{
	const iprm::ITextParam* textParamPtr = dynamic_cast<const iprm::ITextParam*>(&dataModel);

	return textParamPtr != nullptr;
}


bool CIntegerParamRepresentationControllerComp::GetSdlRepresentationFromDataModel(
			sdl::imtbase::ImtBaseTypes::CIntegerParam::V1_0& sdlRepresentation,
			const istd::IChangeable& dataModel,
			const iprm::IParamsSet* /*paramsPtr*/) const
{
	const iprm::ITextParam* textParamPtr = dynamic_cast<const iprm::ITextParam*>(&dataModel);
	Q_ASSERT(textParamPtr != nullptr);
	if (textParamPtr == nullptr){
		return false;
	}

	bool ok = false;
	QString text = textParamPtr->GetText();
	sdlRepresentation.value = text.toInt(&ok, 10);
	if (!ok){
		return false;
	}

	return true;
}


bool CIntegerParamRepresentationControllerComp::GetDataModelFromSdlRepresentation(
			istd::IChangeable& dataModel,
			const sdl::imtbase::ImtBaseTypes::CIntegerParam::V1_0& sdlRepresentation) const
{
	iprm::ITextParam* textParamPtr = dynamic_cast<iprm::ITextParam*>(&dataModel);
	Q_ASSERT(textParamPtr != nullptr);
	if (textParamPtr == nullptr){
		return false;
	}

	if (!sdlRepresentation.value){
		return false;
	}

	QString text = QString::number(*sdlRepresentation.value);
	textParamPtr->SetText(text);

	return true;
}


} // namespace imtserverapp


