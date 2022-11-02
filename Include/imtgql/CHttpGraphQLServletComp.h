#pragma once


// ImtCore includes
#include <imtrest/IProtocolEngine.h>
#include <imtrest/CHttpServletCompBase.h>
#include <imtgql/IGqlRepresentationDataController.h>
#include <imtbase/IObjectCollection.h>


namespace imtgql
{


class CHttpGraphQLServletComp: public imtrest::CHttpServletCompBase
{
public:
	typedef imtrest::CHttpServletCompBase BaseClass;

	I_BEGIN_COMPONENT(CHttpGraphQLServletComp);
		I_ASSIGN_MULTI_0(m_gqlRepresentationDataControllerCompPtr, "GqlRepresentationDataController", "Gql representationDataController", true);
		I_ASSIGN(m_sessionCollectionCompPtr, "SessionCollection", "Session collection", true, "SessionCollection");
		I_ASSIGN(m_settingsCollectionCompPtr, "SettingsCollection", "Settings collection", true, "SettingsCollection");
		I_ASSIGN(m_userCollectionCompPtr, "UserCollection", "User collection", true, "UserCollection");
	I_END_COMPONENT;

protected:
	// reimplemented (imtrest::CHttpServletCompBase)
	virtual ConstResponsePtr OnPost(const QByteArray& commandId, const imtrest::IRequest::CommandParams& commandParams, const HeadersMap& headers, const imtrest::CHttpRequest& request) const override;

private:
	imtrest::IRequestServlet::ConstResponsePtr CreateResponse(
				const imtrest::IProtocolEngine::StatusCode& statusCode,
				const QByteArray& payload,
				const imtrest::IRequest& request,
				const QByteArray& contentTypeId = "text/plain; charset=utf-8") const;
	imtrest::IRequestServlet::ConstResponsePtr GenerateError(
				const imtrest::IProtocolEngine::StatusCode& errorCode,
				const QString& errorString,
				const imtrest::CHttpRequest& request) const;

private:
	I_MULTIREF(imtgql::IGqlRepresentationDataController, m_gqlRepresentationDataControllerCompPtr);
	I_REF(imtbase::IObjectCollection, m_sessionCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_settingsCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_userCollectionCompPtr);
};


} // namespace imtgql


