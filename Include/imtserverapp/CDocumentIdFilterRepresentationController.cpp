// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtserverapp/CDocumentIdFilterRepresentationController.h>
#include <GeneratedFiles/imtbasesdl/SDL/1.0/CPP/DocumentIdFilter.h>


// ImtCore includes
#include <imtcol/IDocumentIdFilter.h>


namespace imtserverapp
{


// public methods

CDocumentIdFilterRepresentationController::CDocumentIdFilterRepresentationController()
{
	m_modelId = QByteArrayLiteral("DocumentIdFilter");
	m_name = QStringLiteral("Document ID Filter");
	m_description = QStringLiteral("Document id filter");
}


// reimplemented (imtserverapp::TJsonRepresentationControllerWrap<Filter::CDocumentIdFilter>)

QByteArray CDocumentIdFilterRepresentationController::GetTypeId() const
{
	return QByteArrayLiteral("DocumentIdFilter");
}


bool CDocumentIdFilterRepresentationController::IsModelSupported(const istd::IChangeable& dataModel) const
{
	const imtcol::IDocumentIdFilter* documentIdFilterPtr = dynamic_cast<const imtcol::IDocumentIdFilter*>(&dataModel);

	return documentIdFilterPtr != nullptr;
}


// reimplemented (imtserverapp::TJsonRepresentationControllerWrap<Filter::CDocumentIdFilter>)

bool CDocumentIdFilterRepresentationController::GetSdlRepresentationFromDataModel(
			sdl::V1_0::imtbase::CDocumentIdFilter& sdlRepresentation,
			const istd::IChangeable& dataModel,
			const iprm::IParamsSet* /*paramsPtr*/) const
{
	const imtcol::IDocumentIdFilter* documentIdFilterPtr = dynamic_cast<const imtcol::IDocumentIdFilter*>(&dataModel);
	if (documentIdFilterPtr == nullptr){
		return false;
	}

	sdlRepresentation.documentIds.Emplace().FromList(documentIdFilterPtr->GetDocumentIds());

	imtcol::IDocumentIdFilter::ConditionType conditionType = documentIdFilterPtr->GetConditionType();
	switch(conditionType){
	case imtcol::IDocumentIdFilter::CT_NONE:
		sdlRepresentation.conditionType = sdl::V1_0::imtbase::ConditionType::None;
		break;
	case imtcol::IDocumentIdFilter::CT_IN:
		sdlRepresentation.conditionType = sdl::V1_0::imtbase::ConditionType::In;
		break;
	case imtcol::IDocumentIdFilter::CT_NOT_IN:
		sdlRepresentation.conditionType = sdl::V1_0::imtbase::ConditionType::NotIn;
		break;
	}

	return true;
}


bool CDocumentIdFilterRepresentationController::GetDataModelFromSdlRepresentation(
			istd::IChangeable& dataModel,
			const sdl::V1_0::imtbase::CDocumentIdFilter& sdlRepresentation) const
{
	imtcol::IDocumentIdFilter* documentIdFilterPtr = dynamic_cast<imtcol::IDocumentIdFilter*>(&dataModel);
	if (documentIdFilterPtr == nullptr){
		return false;
	}

	if (sdlRepresentation.documentIds){
		QByteArrayList documentIds = sdlRepresentation.documentIds->ToList();
		documentIdFilterPtr->SetDocumentIds(documentIds);
	}

	if (sdlRepresentation.conditionType){
		switch (*sdlRepresentation.conditionType){
		case sdl::V1_0::imtbase::ConditionType::None:
			documentIdFilterPtr->SetConditionType(imtcol::IDocumentIdFilter::CT_NONE);
			break;
		case sdl::V1_0::imtbase::ConditionType::In:
			documentIdFilterPtr->SetConditionType(imtcol::IDocumentIdFilter::CT_IN);
			break;
		case sdl::V1_0::imtbase::ConditionType::NotIn:
			documentIdFilterPtr->SetConditionType(imtcol::IDocumentIdFilter::CT_NOT_IN);
			break;
		}
	}

	return true;
}


} // namespace imtserverapp


