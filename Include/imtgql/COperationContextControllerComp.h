#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtbase/IDocumentChangeGenerator.h>
#include <imtgql/IOperationContextController.h>


namespace imtgql
{


class COperationContextControllerComp:
			public ilog::CLoggerComponentBase,
			virtual public imtgql::IOperationContextController
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(COperationContextControllerComp)
		I_REGISTER_INTERFACE(imtgql::IOperationContextController);
		I_ASSIGN(m_documentChangeGeneratorCompPtr, "DocumentChangeGenerator", "Change generator for the collection object", true, "DocumentChangeGenerator");
	I_END_COMPONENT;

protected:
	// reimplemented (imtbase::IOperationContextController)
	virtual imtbase::IOperationContext* CreateOperationContext(
				const QByteArray& operationTypeId,
				const QByteArray& objectId,
				const istd::IChangeable& object,
				const iprm::IParamsSet* paramsPtr) override;
protected:
	I_REF(imtbase::IDocumentChangeGenerator, m_documentChangeGeneratorCompPtr);
};


} // namespace imtgql


