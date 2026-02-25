// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtserverapp/CDocumentFilterRepresentationController.h>


// ImtCore includes
#include <imtcol/IDocumentCollectionFilter.h>


namespace imtserverapp
{


// public methods

CDocumentFilterRepresentationController::CDocumentFilterRepresentationController()
{
	m_modelId = QByteArrayLiteral("DocumentFilter");
	m_name = QStringLiteral("Document Filter");
	m_description = QStringLiteral("Document filter");
}


// reimplemented (imtserverapp::TJsonRepresentationControllerCompWrap<Filter::CDocumentCollectionFilter::V1_0>)

QByteArray CDocumentFilterRepresentationController::GetTypeId() const
{
	return QByteArrayLiteral("DocumentFilter");
}


bool CDocumentFilterRepresentationController::IsModelSupported(const istd::IChangeable& dataModel) const
{
	const imtcol::IDocumentCollectionFilter* documentCollectionFilter = dynamic_cast<const imtcol::IDocumentCollectionFilter*>(&dataModel);

	return documentCollectionFilter != nullptr;
}


bool CDocumentFilterRepresentationController::GetSdlRepresentationFromDataModel(
			Filter::CDocumentCollectionFilter::V1_0& sdlRepresentation,
			const istd::IChangeable& dataModel,
			const iprm::IParamsSet* /*paramsPtr*/) const
{
	const imtcol::IDocumentCollectionFilter* documentCollectionFilter = dynamic_cast<const imtcol::IDocumentCollectionFilter*>(&dataModel);
	if (documentCollectionFilter == nullptr){
		return false;
	}

	sdlRepresentation.documentId = documentCollectionFilter->GetDocumentId();

	imtsdl::TElementList<Filter::DocumentState> sdlDocumentStates;
	imtcol::IDocumentCollectionFilter::DocumentStates states = documentCollectionFilter->GetDocumentStates();
	if (states.contains(imtcol::IDocumentCollectionFilter::DS_ACTIVE)){
		sdlDocumentStates << Filter::DocumentState::ACTIVE;
	}

	if (states.contains(imtcol::IDocumentCollectionFilter::DS_INACTIVE)){
		sdlDocumentStates << Filter::DocumentState::INACTIVE;
	}

	if (states.contains(imtcol::IDocumentCollectionFilter::DS_DISABLED)){
		sdlDocumentStates << Filter::DocumentState::DISABLED;
	}

	sdlRepresentation.documentStates = sdlDocumentStates;

	return true;
}


bool CDocumentFilterRepresentationController::GetDataModelFromSdlRepresentation(
			istd::IChangeable& dataModel,
			const Filter::CDocumentCollectionFilter::V1_0& sdlRepresentation) const
{
	imtcol::IDocumentCollectionFilter* documentCollectionFilter = dynamic_cast<imtcol::IDocumentCollectionFilter*>(&dataModel);
	if (documentCollectionFilter == nullptr){
		return false;
	}

	if (sdlRepresentation.documentId){
		documentCollectionFilter->SetDocumentId(*sdlRepresentation.documentId);
	}

	if (sdlRepresentation.documentStates){
		imtsdl::TElementList<Filter::DocumentState> sdlDocumentStates = *sdlRepresentation.documentStates;
		if (sdlDocumentStates.contains(Filter::DocumentState::ACTIVE)){
			documentCollectionFilter->AddDocumentState(imtcol::IDocumentCollectionFilter::DS_ACTIVE);
		}

		if (sdlDocumentStates.contains(Filter::DocumentState::INACTIVE)){
			documentCollectionFilter->AddDocumentState(imtcol::IDocumentCollectionFilter::DS_INACTIVE);
		}

		if (sdlDocumentStates.contains(Filter::DocumentState::DISABLED)){
			documentCollectionFilter->AddDocumentState(imtcol::IDocumentCollectionFilter::DS_DISABLED);
		}
	}

	return true;
}


} // namespace imtserverapp


