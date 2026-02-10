// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtbase/IDocumentChangeGenerator.h>
#include <imtbase/IOperationContextController.h>


namespace imtservergql
{


class COperationContextControllerComp:
			public ilog::CLoggerComponentBase,
			virtual public imtbase::IOperationContextController
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(COperationContextControllerComp)
		I_REGISTER_INTERFACE(imtbase::IOperationContextController);
		I_ASSIGN(m_documentChangeGeneratorCompPtr, "DocumentChangeGenerator", "Change generator for the collection object", true, "DocumentChangeGenerator");
	I_END_COMPONENT;

protected:
	// reimplemented (imtbase::IOperationContextController)
	virtual imtbase::IOperationContext* CreateOperationContext(
				const QByteArray& operationTypeId,
				const QByteArray& objectId,
				const istd::IChangeable* objectPtr = nullptr,
				const iprm::IParamsSet* paramsPtr = nullptr) override;
protected:
	I_REF(imtbase::IDocumentChangeGenerator, m_documentChangeGeneratorCompPtr);
};


} // namespace imtservergql


