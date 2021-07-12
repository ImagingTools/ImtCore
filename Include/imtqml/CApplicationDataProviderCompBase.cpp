#include <imtqml/CApplicationDataProviderCompBase.h>


namespace imtqml
{


// public methods

CApplicationDataProviderCompBase::CApplicationDataProviderCompBase() :
	CComponentBase()
{
}


// reimplemented (imtbase::IItemBasedRepresentationProvider)

QByteArray CApplicationDataProviderCompBase::GetModelId() const
{
	return *m_applicationIdAttrPtr;
}


imtbase::CTreeItemModel *CApplicationDataProviderCompBase::GetTreeItemModel(const QList<QByteArray>& query, const imtrest::QweryParams& params)
{
	imtbase::CTreeItemModel *rootModel = nullptr;

//	if (modelId == imtqml::Acf::APP){

//	if (modelId == imtqml::Acf::Application()){
//	if (modelId == "__Application__"){
	if (query.count() == 1){ // Application
		if (query[0] == AppEnum::APPLICATION){
			rootModel = new imtbase::CTreeItemModel();
			rootModel->SetData(AppEnum::APPLICATION, *m_applicationIdAttrPtr);
			rootModel->SetData(PageEnum::NAME, *m_applicationNameAttrPtr);
			rootModel->SetData(PageEnum::ICON, *m_applicationNameAttrPtr);
		}
	}
	else if (query.count() == 2 && query[1] == AppEnum::PAGES && params.empty()){  // Pages
		QList<QByteArray> queryPages = query;
		queryPages.removeFirst();
		rootModel = new imtbase::CTreeItemModel();
		imtbase::CTreeItemModel* pagesModel = rootModel->AddTreeModel(AppEnum::PAGES);
		for (int i = 0; i < m_representationPageProvidersCompPtr.GetCount(); ++i){
			imtrest::QweryParams queryParams;
			pagesModel->InsertNewItem();
			imtbase::IItemBasedRepresentationDataProvider* representationPageProvider = m_representationPageProvidersCompPtr[i];			
			QByteArray modelId = representationPageProvider->GetModelId();
			queryParams.insert(PageEnum::ID, modelId);
			imtbase::CTreeItemModel *pageModel = representationPageProvider->GetTreeItemModel(queryPages, queryParams);
			if(pageModel != nullptr){
				pagesModel->CopyItemDataFromModel(i,pageModel);
			}
		}
	}
	else if (query.count() > 1){   // Commands
		QList<QByteArray> queryPages = query;
		queryPages.removeFirst();
		for (int i = 0; i < m_representationPageProvidersCompPtr.GetCount(); ++i){
			imtbase::IItemBasedRepresentationDataProvider* representationPageProvider = m_representationPageProvidersCompPtr[i];
			rootModel = representationPageProvider->GetTreeItemModel(queryPages, params);
			if(rootModel != nullptr){
				break;
			}
		}
	}

//	if (modelId == "__Commands__"){
////		rootModel = new imtbase::CTreeItemModel();
////		rootModel->SetData("id", *m_applicationIdAttrPtr);
////		imtbase::CTreeItemModel* commandsModel = rootModel->AddTreeModel("commands");
//		if (m_representationPageProvidersCompPtr.GetCount() > 0){
//			imtbase::IItemBasedRepresentationDataProvider* representationPageProvider = m_representationPageProvidersCompPtr[0];
//			QByteArray testKey = representationPageProvider->GetModelId();
//			istd::IChangeable::ChangeInfoMap testParams;
//			testParams.insert("pageId",testKey);
//			rootModel = representationPageProvider->GetTreeItemModel("commands", testParams);
//		}
//	}
	return rootModel;
}



} // namespace ilisa


