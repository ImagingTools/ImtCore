// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <iqt/CTranslationManagerComp.h>


namespace imtbase
{


/**
	A translation manager component.

	Translation manager based on using a set of Qt translation files in the native file system.
*/
class CFileBasedTranslationManagerComp: public iqt::CTranslationManagerComp
{
public:
	typedef CTranslationManagerComp BaseClass;

	I_BEGIN_COMPONENT(CFileBasedTranslationManagerComp);
	I_END_COMPONENT;

protected:
	// reimplemented (CTranslationManagerComp)
	virtual void LoadTranslations() override;
};


} // namespace imtbase


