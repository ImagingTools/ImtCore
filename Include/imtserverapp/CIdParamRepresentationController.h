// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtserverapp/TJsonRepresentationControllerWrap.h>
#include <GeneratedFiles/imtbasesdl/SDL/1.0/CPP/Settings>


namespace imtserverapp
{


class CIdParamRepresentationController: public imtserverapp::TJsonRepresentationControllerWrap<sdl::V1_0::imtbase::CIdParam>
{
public:
	CIdParamRepresentationController(QByteArray modelId, QString name = QString(), QString description = QString());

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


