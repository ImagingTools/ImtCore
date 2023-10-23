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
	virtual bool GenerateDocumentChanges(const QByteArray& documentId, const istd::IChangeable* documentPtr, CObjectCollection& documentChangeCollection, QString& errorMessage) = 0;
};


} // namespace imtbase


