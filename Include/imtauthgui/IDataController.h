#pragma once


// ACF includes
#include <istd/IPolymorphic.h>

// ImtCore includes
#include <imtbase/CTreeItemModel.h>


namespace imtauthgui
{


/**
	Base interface for all data controllers
	\ingroup LicenseManagement
*/
class IDataController: virtual public istd::IPolymorphic
{
public:
	virtual bool GetJsonData(QByteArray& jsonData, const QString& typeData = "") const = 0;
	virtual imtbase::CTreeItemModel* GetTreeItemModel(const QString& typeData) const = 0;
};


} // namespace imtauthgui


