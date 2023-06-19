#pragma once


// Qt includes
#include <QtCore/QTimer>


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtgql/IGqlContextController.h>
#include <imtgql/IGqlRequestHandler.h>


namespace imtgql
{


class CGqlContextControllerComp: public QObject, public ilog::CLoggerComponentBase, virtual public imtgql::IGqlContextController
{
	Q_OBJECT
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CGqlContextControllerComp);
		I_REGISTER_INTERFACE(imtgql::IGqlContextController);
		I_ASSIGN(m_sessionCollectionCompPtr, "SessionCollection", "Session collection", true, "SessionCollection");
		I_ASSIGN(m_userCollectionCompPtr, "UserCollection", "User collection", true, "UserCollection");
	I_END_COMPONENT;

	// reimplemented (imtgql::IGqlContextController)
	virtual imtgql::IGqlContext* GetGqlContext(const imtgql::CGqlRequest& gqlRequest, const QByteArray& token) const override;

private:
	I_REF(imtbase::IObjectCollection, m_sessionCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_userCollectionCompPtr);
};


} // namespace imtgql


