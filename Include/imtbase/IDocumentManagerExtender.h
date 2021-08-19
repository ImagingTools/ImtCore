#pragma once


// ACF includes
#include <istd/IChangeable.h>


namespace imtbase
{


/**
	Interface for extending idoc::IDocumentManager.
*/
class IDocumentManagerExtender: virtual public istd::IChangeable
{
public:
	virtual bool SetDocumentName(int documentIndex, const QString& documentName) = 0;
};


} // namespace imtbase


