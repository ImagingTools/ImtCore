#pragma once


// ACF includes
#include <icomp/CComponentBase.h>
#include <iauth/ILogin.h>

// ImtCore includes
#include <imtgql/IGqlClient.h>
#include <imtbase/CTreeItemModel.h>
#include <imtauth/IAccessTokenProvider.h>


namespace imtqml
{


class CSimpleLoginWrapComp:
			public icomp::CComponentBase,
			public iauth::ILogin,
			public imtauth::IAccessTokenProvider,
			public iser::ISerializable
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CSimpleLoginWrapComp);
		I_REGISTER_INTERFACE(ILogin);
		I_REGISTER_INTERFACE(IAccessTokenProvider);
		I_REGISTER_INTERFACE(iser::ISerializable);
		I_ASSIGN(m_apiClientCompPtr, "ApiClient", "Api Client", true, "ApiClient");
	I_END_COMPONENT;

	CSimpleLoginWrapComp();

	// reimplemented (iauth::ILogin)
	virtual iauth::CUser* GetLoggedUser() const override;
	virtual bool Login(const QString& userName, const QString& password) override;
	virtual bool Logout() override;

	// reimplemented (imtauth::IAccessTokenProvider)
	virtual QByteArray GetToken(const QByteArray& userId) const override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

private:
	class Response: virtual public imtgql::IGqlClient::ResponseHandler
	{
	public:
		Response();
		virtual imtbase::CTreeItemModel* GetResult();

		// reimplemented (imtgql::IGqlClient::ResponseHandler)
		virtual void OnReply(const imtgql::IGqlRequest& request, const QByteArray& replyData) override;

	private:
		imtbase::CTreeItemModel* m_replyResultPtr;
	};

protected:
	I_REF(imtgql::IGqlClient, m_apiClientCompPtr);

private:
	QByteArray m_loggedUserId;
	QByteArray m_loggedUserToken;
};


} // namespace imtqml


