// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <ilog/TLoggerCompWrap.h>
#include <imtserverapp/TJsonRepresentationControllerWrap.h>
#include <GeneratedFiles/imtbasesdl/SDL/1.0/CPP/Settings_fwd.h>


namespace imtserverapp
{


class CFileNameParamRepresentationControllerComp:
			public ilog::CLoggerComponentBase,
			public imtserverapp::TJsonRepresentationControllerWrap<sdl::V1_0::imtbase::CFileNameParam>
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CFileNameParamRepresentationControllerComp)
		I_REGISTER_INTERFACE(IJsonRepresentationController);
	I_END_COMPONENT;


protected:
	// reimplemented (imtserverapp::TJsonRepresentationControllerWrap<sdl::V1_0::imtbase::CFileNameParam>)
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


