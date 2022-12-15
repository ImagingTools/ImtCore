#include <imtqml/CTextRepresentationComp.h>


// Qt includes
#include <QtCore/QTranslator>


namespace imtqml
{


// public methods

// reimplemented (imtgql::IItemBasedRepresentationProvider)

imtbase::CTreeItemModel* CTextRepresentationComp::GetRepresentation(
			const QList<imtgql::CGqlObject>& params,
			const QByteArrayList& fields,
			const imtgql::IGqlContext* gqlContext)
{
	if (!m_nameParamCompPtr.IsValid()){
		return nullptr;
	}

	imtbase::CTreeItemModel* rootModelPtr = new imtbase::CTreeItemModel();

	QByteArray paramId = *m_paramIdAttrPtr;
	QString paramName = *m_paramNameAttrPtr;

	rootModelPtr->SetData("Id", paramId);

	if (m_translationManagerCompPtr.IsValid()){
		QByteArray languageId;
		if(gqlContext != nullptr){
			languageId = gqlContext->GetLanguageId();
		}

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


// reimplemented (imtgql::IGqlModelEditor)

bool CTextRepresentationComp::UpdateModelFromRepresentation(
		const QList<imtgql::CGqlObject> &params,
		imtbase::CTreeItemModel *baseModelPtr,
		const imtgql::IGqlContext* gqlContext)
{
	if (!m_nameParamCompPtr.IsValid()){
		return false;
	}

	Q_ASSERT(baseModelPtr != nullptr);

	QByteArray parameterId = *m_paramIdAttrPtr;
	const imtbase::CTreeItemModel* elementModelPtr = GetElementModel(parameterId, baseModelPtr);
	if (elementModelPtr != nullptr){
		QString value;
		if (elementModelPtr->ContainsKey("Value")){
			value = elementModelPtr->GetData("Value").toString();
		}

		m_nameParamCompPtr->SetText(value);
	}

	return true;
}


} // namespace imtqml


