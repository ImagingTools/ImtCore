#include <imtauthgql/CAuthorizationOptionsControllerComp.h>


// ACF includes
#include <iprm/ISelectionParam.h>
#include <iprm/IOptionsList.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>


namespace imtauthgql
{


// protected methods

// reimplemented (imtgql::CGqlRepresentationDataControllerComp)

imtbase::CTreeItemModel* CAuthorizationOptionsControllerComp::CreateRepresentationFromRequest(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	if (!m_databaseEngineCompPtr.IsValid()){
		return nullptr;
	}

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());

	rootModelPtr.SetPtr(BaseClass::CreateRepresentationFromRequest(gqlRequest, errorMessage));
	if (!rootModelPtr.IsValid()){
		return nullptr;
	}

	imtbase::CTreeItemModel* dataModelPtr = rootModelPtr->GetTreeItemModel("data");
	Q_ASSERT(dataModelPtr != nullptr);

	QSqlError sqlError;
	QByteArray query = QString("SELECT * FROM \"Users\" WHERE \"UserId\" = '%1';").arg(qPrintable(*m_superuserLoginAttrPtr)).toLocal8Bit();

	QSqlQuery sqlQuery = m_databaseEngineCompPtr->ExecSqlQuery(query, &sqlError);

	if (sqlError.type() == QSqlError::NoError){
		bool superUserExists = sqlQuery.size() > 0;
		dataModelPtr->SetData("SuperUserExists", superUserExists);
		dataModelPtr->SetData("DatabaseConnectionState", true);
	}
	else{
		dataModelPtr->SetData("DatabaseConnectionState", false);
		dataModelPtr->SetData("Message", sqlError.databaseText());
	}

	return rootModelPtr.PopPtr();
}


} // namespace imtauthgql


