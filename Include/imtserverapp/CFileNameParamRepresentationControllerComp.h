// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtserverapp/TJsonRepresentationControllerCompWrap.h>
#include <GeneratedFiles/imtbasesdl/SDL/1.0/CPP/Settings>


namespace imtserverapp
{


class CFileNameParamRepresentationControllerComp: public imtserverapp::TJsonRepresentationControllerCompWrap<sdl::V1_0::imtbase::CFileNameParam>
{
public:
	typedef imtserverapp::TJsonRepresentationControllerCompWrap<sdl::V1_0::imtbase::CFileNameParam> BaseClass;

	I_BEGIN_COMPONENT(CFileNameParamRepresentationControllerComp)
	I_END_COMPONENT;

protected:
	// reimplemented (imtserverapp::TJsonRepresentationControllerCompWrap<sdl::V1_0::imtbase::CFileNameParam>)
	virtual QByteArray GetTypeId() const override;
	virtual bool IsModelSupported(const istd::IChangeable& dataModel) const override;
	virtual bool GetSdlRepresentationFromDataModel(
				sdl::V1_0::imtbase::CFileNameParam& sdlRepresentation,
				const istd::IChangeable& dataModel,
				const iprm::IParamsSet* paramsPtr = nullptr) const override;
	virtual bool GetDataModelFromSdlRepresentation(
				istd::IChangeable& dataModel,
				const sdl::V1_0::imtbase::CFileNameParam& sdlRepresentation) const override;
};


} // namespace imtserverapp


