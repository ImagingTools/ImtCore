#include <imtauthgql/CAuthorizationOptionsControllerComp.h>


// ACF includes
#include <iprm/ISelectionParam.h>
#include <iprm/IOptionsList.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>


namespace imtauthgql
{


imtbase::CHierarchicalItemModelPtr CAuthorizationOptionsControllerComp::CreateResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	if (!m_userModeCompPtr.IsValid() || !m_databaseEngineCompPtr.IsValid()){
		return imtbase::CHierarchicalItemModelPtr();
	}

	imtbase::CHierarchicalItemModelPtr rootModelPtr(new imtbase::CTreeItemModel());
	imtbase::CTreeItemModel* dataModelPtr = rootModelPtr->AddTreeModel("data");

	const iprm::IOptionsList* optionList = m_userModeCompPtr->GetSelectionConstraints();
	Q_ASSERT(optionList != nullptr);

	if (optionList != nullptr){
		int defaultIndex = m_userModeCompPtr->GetSelectedOptionIndex();

		QByteArray userModeId = optionList->GetOptionId(defaultIndex);

		dataModelPtr->SetData("Value", userModeId);
	}

	QSqlError sqlError;
	QByteArray query = QString("SELECT * FROM \"Users\" WHERE UserId = '%1';").arg(qPrintable(*m_superuserLoginAttrPtr)).toLocal8Bit();

	m_databaseEngineCompPtr->ExecSqlQuery(query, &sqlError);

	bool connectionState = sqlError.type() != QSqlError::ConnectionError;
	dataModelPtr->SetData("DatabaseConnectionState", connectionState);

	bool superUserExists = sqlError.type() == QSqlError::NoError;
	dataModelPtr->SetData("SuperUserExists", superUserExists);

	return rootModelPtr;
}


} // namespace imtauthgql


