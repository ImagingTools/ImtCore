#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtbase/IDocumentChangeGenerator.h>
#include <imtbase/IObjectCollection.h>
#include <imtbase/COperationDescription.h>


namespace imtbase
{


class CDocumentChangeGeneratorCompBase:
			public ilog::CLoggerComponentBase,
			virtual public imtbase::IDocumentChangeGenerator
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_BASE_COMPONENT(CDocumentChangeGeneratorCompBase);
		I_REGISTER_INTERFACE(imtbase::IDocumentChangeGenerator);
		I_ASSIGN(m_objectCollectionCompPtr, "ObjectCollection", "Object collection", true, "ObjectCollection");
	I_END_COMPONENT;

protected:
	// reimplemented (imtbase::IDocumentChangeGenerator)
	virtual bool GenerateDocumentChanges(const QByteArray& documentId, const istd::IChangeable* documentPtr, CObjectCollection& documentChangeCollection, QString& errorMessage) override;
	virtual bool CompareDocuments(const istd::IChangeable* oldDocumentPtr, const istd::IChangeable* newDocumentPtr, CObjectCollection& documentChangeCollection, QString& errorMessage) = 0;
	virtual imtbase::COperationDescription* CreateOperationDescription(const QByteArray& operationTypeId, const QByteArray& key, const QString& keyName, const QByteArray& oldValue, const QByteArray& newValue) const;

protected:
	I_REF(imtbase::IObjectCollection, m_objectCollectionCompPtr);
};


} // namespace imtbase


