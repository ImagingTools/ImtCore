#include <imtqml/CPageDataProviderCompBase.h>


// Qt includes
#include <QtCore/QTranslator>

// ImtCore includes
#include <imtauth/IUserInfo.h>


namespace imtqml
{


// public methods

// reimplemented (imtbase::IItemBasedRepresentationProvider)

QByteArray CPageDataProviderCompBase::GetModelId() const
{
	return *m_pageIdAttrPtr;
}


imtbase::CTreeItemModel* CPageDataProviderCompBase::GetTreeItemModel(const QList<imtgql::CGqlObject>& params, const QByteArrayList& fields, const imtgql::IGqlContext* gqlContext)
{
	imtbase::CTreeItemModel* rootModelPtr = new imtbase::CTreeItemModel();

	if (gqlContext != nullptr){
		const imtauth::IUserInfo* userInfoPtr = gqlContext->GetUserInfo();

		bool result = true;
		if (userInfoPtr != nullptr){
			QByteArray userId = userInfoPtr->GetUserId();
			imtauth::IUserInfo::FeatureIds permissions = userInfoPtr->GetPermissions();

			QByteArrayList permissionIds;
			for (int i = 0; i < m_permissionIdsAttrPtr.GetCount(); i++){
				permissionIds << m_permissionIdsAttrPtr[i];
			}

			if (m_permissionIdsAttrPtr.IsValid()){
				if (m_checkPermissionCompPtr.IsValid()){
					result = m_checkPermissionCompPtr->CheckPermission(permissions, permissionIds);
				}
			}
		}

		if (!result){
			return nullptr;
		}
	}

	for (int indexField = 0; indexField < fields.count(); indexField++){
		if (fields[indexField] == PageEnum::ID){
			QString strId = QString(*m_pageIdAttrPtr);
			rootModelPtr->SetData(PageEnum::ID, strId);
		}
		if (fields[indexField] == PageEnum::NAME){
			if (m_translationManagerCompPtr.IsValid()){
				QByteArray languageId;
				if(gqlContext != nullptr){
					languageId = gqlContext->GetLanguageId();
				}
				int currentIndex = -1;

				if (languageId.isEmpty()){
					currentIndex = m_translationManagerCompPtr->GetCurrentLanguageIndex();
				}
				else{
					currentIndex = iprm::FindOptionIndexById(languageId, m_translationManagerCompPtr->GetLanguagesInfo());
				}

				if (currentIndex >= 0){
					const iqt::ITranslationManager* slaveManagerPtr = m_translationManagerCompPtr->GetSlaveTranslationManager();
					if(slaveManagerPtr != nullptr){
						const QTranslator* translatorPtr = slaveManagerPtr->GetLanguageTranslator(currentIndex);
						if (translatorPtr != nullptr){
							QString pageName = (*m_pageNameAttrPtr);
							QString pageNameTr = translatorPtr->translate("Attribute", pageName.toUtf8());

							rootModelPtr->SetData(PageEnum::NAME, pageNameTr);
						}
					}
				}
			}
			else{
				rootModelPtr->SetData(PageEnum::NAME, *m_pageNameAttrPtr);
			}
		}
		if (fields[indexField] == PageEnum::ICON){
			rootModelPtr->SetData(PageEnum::ICON, *m_pageDefaultStatusIconAttrPtr);
		}
		else if (fields[indexField] == PageEnum::ICON_ON_SELECTED){
			rootModelPtr->SetData(PageEnum::ICON_ON_SELECTED, *m_pageOnSelectedStatusIconAttrPtr);
		}
		else if (fields[indexField] == PageEnum::ICON_OFF_SELECTED){
			rootModelPtr->SetData(PageEnum::ICON_OFF_SELECTED, *m_pageOffSelectedStatusIconAttrPtr);
		}
		else if (fields[indexField] == PageEnum::SOURCE){
			rootModelPtr->SetData(PageEnum::SOURCE, *m_pageSourceItemAttrPtr);
		}
		else if (fields[indexField] == PageEnum::ENABLED){
			rootModelPtr->SetData(PageEnum::ENABLED, "true");
		}
		else if (fields[indexField] == "StartItem"){
			if (m_startSourceItemAttrPtr.IsValid()){
				rootModelPtr->SetData("StartItem", *m_startSourceItemAttrPtr);
			}
			else{
				rootModelPtr->SetData("StartItem", "");
			}
		}
	}

	if (m_subPagesDataProviderCompPtr.IsValid()){
		imtbase::CTreeItemModel* subPagesModel =  m_subPagesDataProviderCompPtr->GetTreeItemModel(params, fields);
		rootModelPtr->SetExternTreeModel("SubPages", subPagesModel);
	}
	rootModelPtr->SetData("isOpened", false);

	return rootModelPtr;
}


} // namespace imtqml


