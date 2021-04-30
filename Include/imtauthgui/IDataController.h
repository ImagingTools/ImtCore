#pragma once

// ACF includes
#include <istd/IPolymorphic.h>
#include <imtauthgui/CTreeItemModel.h>


namespace imtauthgui
{


/**
	Base interface for all data controllers
	\ingroup LicenseManagement
*/
class IDataController : virtual public istd::IPolymorphic
{
public:
	virtual bool GetJsonData(QByteArray& jsonData, const QString& typeData = "") = 0;
	virtual CTreeItemModel* GetTreeItemModel(const QString& typeData) = 0;
};


} // namespace imtauthgui


