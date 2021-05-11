#pragma once

// ACF includes
#include <istd/IPolymorphic.h>
#include <imod/IObserver.h>
#include <imtbase/CTreeItemModel.h>
#include <imtbase/ICollectionInfo.h>


namespace imtbase
{


/**
	Base interface for all data controllers
	\ingroup LicenseManagement
*/
class IItemBasedRepresentationDataProvider: virtual public istd::IPolymorphic
{
public:
//	virtual const imtbase::ICollectionInfo& GetModelsList() const = 0;
	virtual CTreeItemModel* GetTreeItemModel(const QByteArray& modelId) = 0;
};


} // namespace imtauthgui


