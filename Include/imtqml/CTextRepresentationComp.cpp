#include <imtqml/CTextRepresentationComp.h>


// ImtCore includes
#include <imtbase/imtbase.h>


namespace imtqml
{


// public methods

// reimplemented (imtgql::CGqlRepresentationDataControllerComp)

imtbase::CTreeItemModel* CTextRepresentationComp::CreateRepresentationFromRequest(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	if (!m_nameParamCompPtr.IsValid()){
		return nullptr;
	}

	imtbase::CTreeItemModel* rootModelPtr = new imtbase::CTreeItemModel();

	QByteArray paramId = *m_paramIdAttrPtr;
	QString paramName = *m_paramNameAttrPtr;

	rootModelPtr->SetData("Id", paramId);

	imtgql::IGqlContext* gqlContextPtr = gqlRequest.GetGqlContext();\

	QByteArray languageId;
	if(gqlContextPtr != nullptr){
		languageId = gqlContextPtr->GetLanguageId();
	}

	if (m_translationManagerCompPtr.IsValid()){
		QString paramNameTr = imtbase::GetTranslation(m_translationManagerCompPtr.GetPtr(), paramName.toUtf8(), languageId, "Attribute");

		rootModelPtr->SetData("Name", paramNameTr);
	}
	else{
		rootModelPtr->SetData("Name", paramName);
	}

	if (m_qmlPathAttrPtr.IsValid()){
		rootModelPtr->SetData("Source", *m_qmlPathAttrPtr);
	}

	QString value = m_nameParamCompPtr->GetText();

	rootModelPtr->SetData("Value", value);

	return rootModelPtr;
}


bool CTextRepresentationComp::UpdateModelFromRepresentation(const imtgql::CGqlRequest& request, imtbase::CTreeItemModel* representationPtr) const
{
	if (!m_nameParamCompPtr.IsValid()){
		return false;
	}

	QByteArray parameterId = *m_paramIdAttrPtr;
	if (representationPtr != nullptr){
		QString value;
		if (representationPtr->ContainsKey("Value")){
			value = representationPtr->GetData("Value").toString();
		}

		m_nameParamCompPtr->SetText(value);

		return true;
	}

	return false;
}


} // namespace imtqml


