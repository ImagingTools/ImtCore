#include <imtqml/CPageDataProviderComp.h>


// ImtCore includes
#include <imtbase/imtbase.h>
#include <imtauth/IUserInfo.h>


namespace imtqml
{


// public methods

// reimplemented (imtgql::IGqlRequestHandler)

bool CPageDataProviderComp::IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const
{
	return true;
}


// protected methods

// reimplemented (imtgql::CGqlRepresentationControllerCompBase)

imtbase::CTreeItemModel* CPageDataProviderComp::CreateInternalResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	const QList<imtgql::CGqlObject> fieldsPtr = gqlRequest.GetFields();

	QByteArrayList fieldsList;
	for (int i = 0; i < fieldsPtr.count(); i++){
		if (fieldsPtr.at(i).GetId() == "items"){
			fieldsList = fieldsPtr.at(i).GetFieldIds();
			break;
		}
	}

	QByteArray languageId;
	const imtgql::IGqlContext* gqlContextPtr = gqlRequest.GetGqlContext();
	if (gqlContextPtr != nullptr){
		languageId = gqlContextPtr->GetLanguageId();
	}

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr = new imtbase::CTreeItemModel();

	for (int indexField = 0; indexField < fieldsList.count(); indexField++){
		if (fieldsList[indexField] == "PageId"){
			QString strId = QString(*m_pageIdAttrPtr);
			rootModelPtr->SetData("PageId", strId);
		}
		else if (fieldsList[indexField] == "Name"){
			QString pageName = (*m_pageNameAttrPtr);

			if (m_translationManagerCompPtr.IsValid()){
				QByteArray context = "Attribute";
				QByteArray pageNameTr = imtbase::GetTranslation(m_translationManagerCompPtr.GetPtr(), pageName.toUtf8(), languageId, context);

				pageName = pageNameTr;
			}

			rootModelPtr->SetData("Name", pageName);
		}
		else if (fieldsList[indexField] == "Icon"){
			rootModelPtr->SetData("Icon", *m_pageDefaultStatusIconAttrPtr);
		}
		else if (fieldsList[indexField] == "IconOnSelected"){
			rootModelPtr->SetData("IconOnSelected", *m_pageOnSelectedStatusIconAttrPtr);
		}
		else if (fieldsList[indexField] == "IconOffSelected"){
			rootModelPtr->SetData("IconOffSelected", *m_pageOffSelectedStatusIconAttrPtr);
		}
		else if (fieldsList[indexField] == "Source"){
			rootModelPtr->SetData("Source", *m_pageSourceItemAttrPtr);
		}
		else if (fieldsList[indexField] == "Enabled"){
			rootModelPtr->SetData("Enabled", "true");
		}
		else if (fieldsList[indexField] == "StartItem"){
			rootModelPtr->SetData("StartItem", *m_startSourceItemAttrPtr);
		}
	}

	return rootModelPtr.PopPtr();
}


} // namespace imtqml


