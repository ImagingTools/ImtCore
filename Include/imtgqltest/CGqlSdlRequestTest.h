// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ipackage/CComponentAccessor.h>

// ImtCore includes
#include <imtbase/CTreeItemModel.h>
#include <imtgql/CGqlRequest.h>
#include <imtgql/IGqlRequestHandler.h>
#include <imtdb/IDatabaseEngine.h>


namespace imtgqltest
{


class CGqlSdlRequestTest: public QObject
{
	Q_OBJECT
protected:
	template <class  Request, class  Arguments, class  ResponseData>
	bool SendRequest(Arguments arguments, class ResponseData::V1_0& response) const
	{
		imtgql::CGqlRequest gqlRequest;

		bool ok = Request::SetupGqlRequest(gqlRequest, arguments);
		if (ok){
			const imtgql::IGqlRequestHandler* requestHandlerPtr = GetRequestHandler();
			if (requestHandlerPtr == nullptr){
				return false;
			}

			QString errorMessage;
			istd::TDelPtr<imtbase::CTreeItemModel> responseModelPtr = requestHandlerPtr->CreateResponse(gqlRequest, errorMessage);
			if (!responseModelPtr.IsValid()){
				return false;
			}

			if (responseModelPtr->ContainsKey("errors")){
				return false;
			}

			imtbase::CTreeItemModel* dataModelPtr = nullptr;
			if (responseModelPtr->ContainsKey("data")){
				dataModelPtr = responseModelPtr->GetTreeItemModel("data");
			}

			if (dataModelPtr == nullptr){
				return false;
			}

			if (!response.ReadFromModel(*dataModelPtr)){
				return false;
			}
		}

		return true;
	}

	ipackage::CComponentAccessor* GetComponentAccessor() const;
	const imtgql::IGqlRequestHandler* GetRequestHandler() const;

	QSqlQuery ExecuteQuery(const QString& query, QSqlError* sqlError = nullptr) const;

protected:
	mutable istd::TSmartPtr<ipackage::CComponentAccessor> m_componentAccessorPtr;
	QString m_registryFile;
	QString m_configFile;
};


} // namespace imtgqltest

