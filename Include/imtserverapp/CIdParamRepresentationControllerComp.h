// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtserverapp/TJsonRepresentationControllerCompWrap.h>
#include <GeneratedFiles/imtbasesdl/SDL/1.0/CPP/Settings_fwd.h>


namespace imtserverapp
{


class CIdParamRepresentationControllerComp: public imtserverapp::TJsonRepresentationControllerCompWrap<sdl::V1_0::imtbase::CIdParam>
{
public:
	typedef imtserverapp::TJsonRepresentationControllerCompWrap<sdl::V1_0::imtbase::CIdParam> BaseClass;

	I_BEGIN_COMPONENT(CIdParamRepresentationControllerComp)
	I_END_COMPONENT;

	// reimplemented (imtserverapp::TJsonRepresentationControllerCompWrap<sdl::V1_0::imtbase::CParamsSet>)
	virtual QByteArray GetTypeId() const override;
	virtual bool IsModelSupported(const istd::IChangeable& dataModel) const override;
	virtual bool GetSdlRepresentationFromDataModel(
				sdl::V1_0::imtbase::CIdParam& sdlRepresentation,
				const istd::IChangeable& dataModel,
				const iprm::IParamsSet* paramsPtr = nullptr) const override;
	virtual bool GetDataModelFromSdlRepresentation(
				istd::IChangeable& dataModel,
				const sdl::V1_0::imtbase::CIdParam& sdlRepresentation) const override;
};


} // namespace imtserverapp


