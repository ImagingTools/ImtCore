#include <imtqml/CObserverQmlComp.h>


// Qt includes
#include <QtQml/QQmlEngine>

// ACF includes
#include <iprm/ISelectionParam.h>
#include <iprm/IOptionsList.h>
#include <iprm/TParamsPtr.h>

// ImtCore includes
#include <imtgql/CGqlContext.h>


namespace imtqml
{


CObserverQmlComp::CObserverQmlComp()
	:m_settingsModelPtr(nullptr)
{
}


// protected methods

void CObserverQmlComp::ApplyUrl() const
{
	if (!m_quickObjectCompPtr.IsValid()){
		return;
	}

	imtbase::CHierarchicalItemModelPtr pageModelPtr = CreatePageModel("NetworkSettings");
	if (pageModelPtr.IsValid()){
		imtbase::CTreeItemModel* elementsModelPtr = pageModelPtr->GetTreeItemModel("Elements");
		if (elementsModelPtr != nullptr){
			if (elementsModelPtr->ContainsKey("Value")){
				QString serverUrl = elementsModelPtr->GetData("Value").toString();

				QQuickItem* quickItem = m_quickObjectCompPtr->GetQuickItem();
				Q_ASSERT(quickItem != nullptr);

				QQmlEngine* engine = qmlEngine(quickItem);
				Q_ASSERT(engine != nullptr);

				engine->setBaseUrl(serverUrl + *m_prefixServer);
			}
		}
	}
}


// reimplemented (icomp::CComponentBase)

void CObserverQmlComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_quickObjectCompPtr.IsValid()){
		QQuickItem* quickItem = m_quickObjectCompPtr->GetQuickItem();
		if (quickItem != nullptr){
			if (m_pagesDataProviderCompPtr.IsValid()){
				QList<imtgql::CGqlObject> params;
				QByteArrayList fields;

				fields.append("NetworkSettings");

				istd::TDelPtr<imtgql::CGqlContext> gqlContextPtr = new imtgql::CGqlContext();
				if (m_settingsCompPtr.IsValid()){
					iprm::TParamsPtr<iprm::ISelectionParam> selectedLanguagePtr(m_settingsCompPtr.GetPtr(), "Language");
					if (selectedLanguagePtr.IsValid()){
						int languageIndex = selectedLanguagePtr->GetSelectedOptionIndex();
						if (languageIndex >= 0){
							const iprm::IOptionsList* languageListPtr = selectedLanguagePtr->GetSelectionConstraints();
							if (languageListPtr != nullptr){
								QByteArray languageId = languageListPtr->GetOptionId(languageIndex);

								gqlContextPtr->SetLanguageId(languageId);
							}
						}
					}

					m_settingsModelPtr = m_pagesDataProviderCompPtr->GetRepresentation(params, fields, gqlContextPtr.GetPtr());
					if (m_settingsModelPtr != nullptr){
						QVariant data = QVariant::fromValue(m_settingsModelPtr);

						quickItem->setProperty("localSettings", data);
					}
				}

				connect(quickItem, SIGNAL(settingsUpdate(QString)), this, SLOT(OnModelChanged(QString)));

				ApplyUrl();

				QMetaObject::invokeMethod(quickItem, "updateModels");
			}
		}
	}
}


void CObserverQmlComp::OnComponentDestroyed()
{
	if (m_settingsModelPtr != nullptr){
		delete m_settingsModelPtr;

		m_settingsModelPtr = nullptr;
	}

	BaseClass::OnComponentDestroyed();
}


void CObserverQmlComp::OnChangeSourceItem(QString src)
{
	if (m_quickObjectCompPtr.IsValid()){
		QQuickItem* quickItem = m_quickObjectCompPtr->GetQuickItem();
		Q_ASSERT(quickItem != nullptr);

		quickItem->setProperty("sourceItem", "qrc:///qml/" + src);
	}
}


void CObserverQmlComp::OnModelChanged(const QString& pageId)
{
	imtbase::CHierarchicalItemModelPtr pageModelPtr = CreatePageModel(pageId.toUtf8());
	if (pageModelPtr.IsValid()){
		ApplyUrl();

		if (m_mutationDataDelegateCompPtr.IsValid()){
			QList<imtgql::CGqlObject> params;
			imtgql::CGqlObject gqlObject;
			gqlObject.InsertField("Id", pageId);
			params << gqlObject;

			m_mutationDataDelegateCompPtr->UpdateModelFromRepresentation(params, pageModelPtr.GetPtr());
		}
	}
}


imtbase::CHierarchicalItemModelPtr CObserverQmlComp::CreatePageModel(const QByteArray& pageId) const
{
	imtbase::CHierarchicalItemModelPtr retVal;

	if (m_settingsModelPtr != nullptr){
		for (int i = 0; i < m_settingsModelPtr->GetItemsCount(); i++){
			if (m_settingsModelPtr->ContainsKey("Id", i)){
				QByteArray currentPageId = m_settingsModelPtr->GetData("Id", i).toByteArray();
				if (currentPageId == pageId){
					retVal.SetPtr(new imtbase::CTreeItemModel());
					int index = retVal->InsertNewItem();

					m_settingsModelPtr->CopyItemDataToModel(i, retVal.GetPtr(), index);
					break;
				}
			}
		}
	}

	return retVal;
}


} // namespace imtqml


