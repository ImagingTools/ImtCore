#include <imtauthgui/CContactInfoRepresentationDataProviderComp.h>


// Qt includes
#include <QtQml/QQmlEngine>
#include <QtQml/QQmlContext>

// ACF includes
#include <istd/CChangeGroup.h>
#include <iser/CJsonStringWriteArchive.h>

// ImtCore includes
#include <imtbase/ICollectionInfo.h>
#include <imtauth/IAddressManager.h>
#include <imtauth/CAddress.h>
#include <imtbase/CTreeItemModel.h>



namespace imtauthgui
{


// public methods

CContactInfoRepresentationDataProviderComp::CContactInfoRepresentationDataProviderComp()
{

}

// reimplemented (imtrest::IRepresentationDataProvider)

bool CContactInfoRepresentationDataProviderComp::GetRepresentationData(
		imtrest::IRepresentationDataProvider::Format format,
		QByteArray& representationData,
		const QByteArray& commandId)
{
	bool retVal = false;

	if (m_itemBasedRepresentationDataProvider.IsValid()
			&& format == imtrest::IRepresentationDataProvider::RF_JSON
			&& commandId == "__ContactInfo__"){
		imtbase::CTreeItemModel *treeItemModel = m_itemBasedRepresentationDataProvider->GetTreeItemModel("");
		iser::CJsonStringWriteArchive archive(representationData);
		retVal = treeItemModel->Serialize(archive);
	}

	return retVal;
}


bool CContactInfoRepresentationDataProviderComp::SetRepresentationData(
			imtrest::IRepresentationDataProvider::Format format,
			QByteArray& /*representationData*/,
			const QByteArray& commandId)
{
	bool retVal = false;

	if (m_itemBasedRepresentationDataProvider.IsValid()
			&& format == imtrest::IRepresentationDataProvider::RF_JSON
			&& commandId == "__ContactInfo__"){
	}

	return retVal;
}




} // namespace imtauthgui


