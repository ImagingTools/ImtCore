#pragma once


// ACF includes
#include <idoc/CMultiDocumentManagerBase.h>

// ACF includes
#include <imtbase/IDocumentManagerExtender.h>


namespace imtbase
{


class CMultiDocumentManager: public idoc::CMultiDocumentManagerBase, virtual public IDocumentManagerExtender
{
public:
	virtual bool SetDocumentName(int documentIndex, const QString& documentName) override;
};


} // namespace imtbase


