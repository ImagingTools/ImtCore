#pragma once


// ACF includes
#include <istd/IPolymorphic.h>
#include <istd/IChangeable.h>

// ImtCore includes
#include <imtbase/CObjectCollection.h>


namespace imtbase
{


class IDocumentChangeGenerator: virtual public istd::IPolymorphic
{
public:
	enum OperationType
	{
		OT_CREATE,
		OT_UPDATE,
		OT_REMOVE,
		OT_SET,
		OT_ADD,
		OT_CLEAR,
		OT_CHANGE,
		OT_USER = 1024
	};

	virtual bool GenerateDocumentChanges(
				int operationType,
				const QByteArray& documentId,
				const istd::IChangeable* documentPtr,
				CObjectCollection& documentChangeCollection,
				QString& errorMessage,
				const iprm::IParamsSet* paramsPtr) = 0;
	virtual QString GetOperationDescription(CObjectCollection& documentChangeCollection, const QByteArray& languageId = QByteArray()) = 0;
};


} // namespace imtbase


