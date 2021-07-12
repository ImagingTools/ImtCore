#pragma once

// ACF includes
#include <istd/IPolymorphic.h>
#include <istd/IChangeable.h>
//#include <imod/IObserver.h>

// ImtCore includes
#include <imtbase/CTreeItemModel.h>
#include <imtrest/imtrest.h>
//#include <imtbase/ICollectionInfo.h>


namespace imtbase
{


/**
	Base interface for all data controllers
	\ingroup LicenseManagement
*/
class IItemBasedRepresentationDataProvider: virtual public istd::IPolymorphic
{
public:
	virtual QByteArray GetModelId() const = 0;
	virtual CTreeItemModel* GetTreeItemModel(const QList<QByteArray>& query, const imtrest::QweryParams& params) = 0;
};


} // namespace imtauthgui


