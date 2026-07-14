// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtserverapp/TJsonRepresentationControllerWrap.h>
#include <GeneratedFiles/imtbasesdl/SDL/1.0/CPP/DocumentCollectionFilter.h>


namespace imtserverapp
{


namespace Filter = sdl::V1_0::imtbase;


class CDocumentFilterRepresentationController:
			public imtserverapp::TJsonRepresentationControllerWrap<Filter::CDocumentCollectionFilter>
{
public:
	CDocumentFilterRepresentationController();

	// reimplemented (imtserverapp::TJsonRepresentationControllerWrap<Filter::CDocumentCollectionFilter>)
	virtual QByteArray GetTypeId() const override;
	virtual bool IsModelSupported(const istd::IChangeable& dataModel) const override;
	virtual bool GetSdlRepresentationFromDataModel(
				Filter::CDocumentCollectionFilter& sdlRepresentation,
				const istd::IChangeable& dataModel,
				const iprm::IParamsSet* paramsPtr = nullptr) const override;
	virtual bool GetDataModelFromSdlRepresentation(
				istd::IChangeable& dataModel,
				const Filter::CDocumentCollectionFilter& sdlRepresentation) const override;
};


} // namespace imtserverapp


