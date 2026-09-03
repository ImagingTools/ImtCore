// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtserverapp/CDoubleParamRepresentationControllerComp.h>
#include <GeneratedFiles/imtbasesdl/SDL/1.0/CPP/Settings.h>


// ACF includes
#include <imeas/INumericValue.h>


namespace imtserverapp
{


// protected methods

// reimplemented (imtserverapp::TJsonRepresentationControllerWrap<sdl::V1_0::imtbase::CDoubleParam>)

QByteArray CDoubleParamRepresentationControllerComp::GetTypeId() const
{
	return sdl::V1_0::imtbase::CParamTypeIds::ParamTypeIdsFields::DoubleParam.toUtf8();
}


bool CDoubleParamRepresentationControllerComp::IsModelSupported(const istd::IChangeable& dataModel) const
{
	const imeas::INumericValue* numericValuePtr = dynamic_cast<const imeas::INumericValue*>(&dataModel);

	return numericValuePtr != nullptr && numericValuePtr->GetValues().GetElementsCount() == 1;
}


bool CDoubleParamRepresentationControllerComp::GetSdlRepresentationFromDataModel(
			sdl::V1_0::imtbase::CDoubleParam& sdlRepresentation,
			const istd::IChangeable& dataModel,
			const iprm::IParamsSet* /*paramsPtr*/) const
{
	const imeas::INumericValue* numericValuePtr = dynamic_cast<const imeas::INumericValue*>(&dataModel);
	Q_ASSERT(numericValuePtr != nullptr);
	if (numericValuePtr == nullptr){
		return false;
	}

	imath::CVarVector values = numericValuePtr->GetValues();
	Q_ASSERT(values.GetElementsCount() == 1);
	if (values.GetElementsCount() != 1){
		return false;
	}

	sdlRepresentation.value = values[0];

	return true;
}


bool CDoubleParamRepresentationControllerComp::GetDataModelFromSdlRepresentation(
			istd::IChangeable& dataModel,
			const sdl::V1_0::imtbase::CDoubleParam& sdlRepresentation) const
{
	imeas::INumericValue* numericValuePtr = dynamic_cast<imeas::INumericValue*>(&dataModel);
	Q_ASSERT(numericValuePtr != nullptr);
	if (numericValuePtr == nullptr){
		return false;
	}

	if (!sdlRepresentation.value){
		return false;
	}

	imath::CVarVector values = numericValuePtr->GetValues();
	Q_ASSERT(values.GetElementsCount() == 1);
	if (values.GetElementsCount() != 1){
		return false;
	}

	values[0] = *sdlRepresentation.value;
	numericValuePtr->SetValues(values);

	return true;
}


} // namespace imtserverapp


