// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtserverapp/TJsonRepresentationControllerCompWrap.h>
#include <GeneratedFiles/imtbasesdl/SDL/1.0/CPP/Settings.h>


namespace imtserverapp
{


class CTextParamRepresentationControllerComp: public TJsonRepresentationControllerCompWrap<sdl::imtbase::ImtBaseTypes::CTextParam::V1_0>
{
public:
	typedef TJsonRepresentationControllerCompWrap<sdl::imtbase::ImtBaseTypes::CTextParam::V1_0> BaseClass;

	I_BEGIN_COMPONENT(CTextParamRepresentationControllerComp)
	I_END_COMPONENT;

protected:
	// reimplemented (TJsonRepresentationControllerCompWrap<sdl::imtbase::ImtBaseTypes::CTextParam::V1_0>)
	virtual QByteArray GetTypeId() const override;
	virtual bool IsModelSupported(const istd::IChangeable& dataModel) const override;
	virtual bool GetSdlRepresentationFromDataModel(
				sdl::imtbase::ImtBaseTypes::CTextParam::V1_0& sdlRepresentation,
				const istd::IChangeable& dataModel,
				const iprm::IParamsSet* paramsPtr = nullptr) const override;
	virtual bool GetDataModelFromSdlRepresentation(
				istd::IChangeable& dataModel,
				const sdl::imtbase::ImtBaseTypes::CTextParam::V1_0& sdlRepresentation) const override;
};


} // namespace imtbase


