#pragma once


// ACF includes
#include <iqt/CTranslationManagerComp.h>


namespace imtbase
{

	
/**
	A translation manager component.

	Translation manager based on using a set of Qt translation files.
*/
class CFileBasedTranslationManagerComp: public iqt::CTranslationManagerComp
{
public:
	typedef CTranslationManagerComp BaseClass;

	I_BEGIN_COMPONENT(CFileBasedTranslationManagerComp);
	I_END_COMPONENT;

protected:
	// reimplemented (CTranslationManagerComp)
	virtual void LoadTranslations();
};


} // namespace imtbase


