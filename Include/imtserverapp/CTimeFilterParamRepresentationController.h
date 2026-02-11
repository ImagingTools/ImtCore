// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtserverapp/TJsonRepresentationControllerWrap.h>
#include <GeneratedFiles/imtbasesdl/SDL/1.0/CPP/ComplexCollectionFilter.h>


namespace imtserverapp
{


class CTimeFilterParamRepresentationController:
			public TJsonRepresentationControllerWrap<sdl::imtbase::ComplexCollectionFilter::CTimeFilter::V1_0>
{
public:
	CTimeFilterParamRepresentationController();
	CTimeFilterParamRepresentationController(const QByteArray& filterId, const QString& filterName = QString(), const QString& filterDescription = QString());

	// reimplemented (imtserverapp::TJsonRepresentationControllerWrap<sdl::imtbase::ComplexCollectionFilter::CTimeFilter::V1_0>)
	virtual QByteArray GetTypeId() const override;
	virtual bool IsModelSupported(const istd::IChangeable& dataModel) const override;
	virtual bool GetSdlRepresentationFromDataModel(
				sdl::imtbase::ComplexCollectionFilter::CTimeFilter::V1_0& sdlRepresentation,
				const istd::IChangeable& dataModel,
				const iprm::IParamsSet* paramsPtr = nullptr) const override;
	virtual bool GetDataModelFromSdlRepresentation(
				istd::IChangeable& dataModel,
				const sdl::imtbase::ComplexCollectionFilter::CTimeFilter::V1_0& sdlRepresentation) const override;
};


} // namespace imtserverapp


