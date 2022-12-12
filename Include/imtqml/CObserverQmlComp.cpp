#include "CObserverQmlComp.h"

// Qt includes
#include <QtQml/QQmlEngine>

// ACF includes
#include <iprm/IParamsSet.h>
#include <iprm/ISelectionParam.h>
#include <iprm/IOptionsList.h>

// ImtCore includes
#include <imtgql/CGqlContext.h>


namespace imtqml
{


CObserverQmlComp::CObserverQmlComp()
	:m_settingsModelPtr(nullptr),
	m_settingsObserver(*this)
{
}


//protected methods

void CObserverQmlComp::OnSettingsUpdated(const istd::IChangeable::ChangeSet &changeSet, const imtbase::CTreeItemModel *settingsModelPtr)
{
	imtbase::CTreeItemModel* model = const_cast<imtbase::CTreeItemModel*>(settingsModelPtr);
	QList<imtgql::CGqlObject> params;

	if (!changeSet.GetChangeInfoMap().isEmpty()){
		ApplyUrl();

		if (m_mutationDataDelegateCompPtr.IsValid()){
			m_mutationDataDelegateCompPtr->UpdateModelFromRepresentation(params, model);
		}
	}
}


void CObserverQmlComp::ApplyUrl() const
{
	if (!m_quickObjectComp.IsValid()){
		return;
	}

	const imtbase::CTreeItemModel* pageModelPtr = GetPageModel("NetworkSettings");
	if (pageModelPtr != nullptr){
		imtbase::CTreeItemModel* elementsModelPtr = pageModelPtr->GetTreeItemModel("Elements");
		if (elementsModelPtr != nullptr){
			if (elementsModelPtr->ContainsKey("Value")){
				QString serverUrl = elementsModelPtr->GetData("Value").toString();

				QQuickItem* quickItem = m_quickObjectComp->GetQuickItem();
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

	if (m_quickObjectComp.IsValid()){
		QQuickItem* quickItem = m_quickObjectComp->GetQuickItem();
		if (quickItem != nullptr){
			if (m_pagesDataProviderCompPtr.IsValid()){
				QList<imtgql::CGqlObject> params;
				imtgql::CGqlObject* inputParamsPtr = new imtgql::CGqlObject("input");

				inputParamsPtr->InsertField("LanguageId", "ru_RU");
				params.append(*inputParamsPtr);

				QByteArrayList fields;
				fields.append("NetworkSettings");

				istd::TDelPtr<imtgql::CGqlContext> gqlContextPtr = new imtgql::CGqlContext();
				if (m_languageParameterCompPtr.IsValid()){
					iprm::IParamsSet* languageParamSetPtr = dynamic_cast<iprm::IParamsSet*>(m_languageParameterCompPtr.GetPtr());
					if (languageParamSetPtr != nullptr){
						const iprm::ISelectionParam* languageParamPtr = dynamic_cast<const iprm::ISelectionParam*>(languageParamSetPtr->GetParameter("Language"));
						if (languageParamPtr != nullptr){
							const iprm::IOptionsList* optionList = languageParamPtr->GetSelectionConstraints();
							int languageIndex = languageParamPtr->GetSelectedOptionIndex();

							if (optionList != nullptr){
								QByteArray languageId = optionList->GetOptionId(languageIndex);

								gqlContextPtr->SetLanguageId(languageId);
							}
						}
					}
				}

				m_settingsModelPtr = m_pagesDataProviderCompPtr->GetRepresentation(params, fields, gqlContextPtr.GetPtr());

				if (m_settingsModelPtr != nullptr){
					QVariant data = QVariant::fromValue(m_settingsModelPtr);

					quickItem->setProperty("localSettings", data);
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
	m_settingsObserver.UnregisterAllObjects();

	if (m_settingsModelPtr != nullptr){
		delete m_settingsModelPtr;

		m_settingsModelPtr = nullptr;
	}

	BaseClass::OnComponentDestroyed();
}


void CObserverQmlComp::OnChangeSourceItem(QString src)
{
	if (m_quickObjectComp.IsValid()){
		QQuickItem* quickItem = m_quickObjectComp->GetQuickItem();
		Q_ASSERT(quickItem != nullptr);

		quickItem->setProperty("sourceItem", "qrc:///qml/"+ src);
	}
}


void CObserverQmlComp::OnModelChanged(const QString& pageId)
{
	QList<imtgql::CGqlObject> params;

	imtgql::CGqlObject gqlObject;
	gqlObject.InsertField("Id", pageId);
	params << gqlObject;

	const imtbase::CTreeItemModel* pageModelPtr = GetPageModel(pageId.toUtf8());
	if (pageModelPtr != nullptr){
		ApplyUrl();

		if (m_mutationDataDelegateCompPtr.IsValid()){
			m_mutationDataDelegateCompPtr->UpdateModelFromRepresentation(params, const_cast<imtbase::CTreeItemModel*>(pageModelPtr));
		}
	}
}


const imtbase::CTreeItemModel* CObserverQmlComp::GetPageModel(const QByteArray &pageId) const
{
	imtbase::CTreeItemModel* pageModelPtr = nullptr;

	if (m_settingsModelPtr != nullptr){
		for (int i = 0; i < m_settingsModelPtr->GetItemsCount(); i++){
			if (m_settingsModelPtr->ContainsKey("Id", i)){
				QByteArray currentPageId = m_settingsModelPtr->GetData("Id", i).toByteArray();
				if (currentPageId == pageId){
					pageModelPtr = new imtbase::CTreeItemModel();
					int index = pageModelPtr->InsertNewItem();

					m_settingsModelPtr->CopyItemDataToModel(i, pageModelPtr, index);
					break;
				}
			}
		}
	}

	return pageModelPtr;
}


} // namespace imtqml


