#pragma once


// ACF includes
#include <istd/IPolymorphic.h>
#include <istd/IChangeable.h>

// ImtCore includes
#include <imtbase/CObjectCollection.h>
#include <imtbase/IOperationDescription.h>


namespace imtbase
{


class IDocumentChangeGenerator: virtual public istd::IPolymorphic
{
public:
	virtual bool GenerateDocumentChanges(
				const QByteArray& operationTypeId,
				const QByteArray& documentId,
				const istd::IChangeable* documentPtr,
				CObjectCollection& documentChangeCollection,
				QString& errorMessage,
				const iprm::IParamsSet* paramsPtr) = 0;
	virtual QString GetOperationDescription(CObjectCollection& documentChangeCollection, const QByteArray& languageId = QByteArray()) = 0;
};


} // namespace imtbase


