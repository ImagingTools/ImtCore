// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtassist/CUiRepresentationRegistry.h>
#include <imtassist/IUiRepresentationSchemaProvider.h>


namespace imtassist
{


/**
	Component of the UI representation registry.
	Registers the standard representations of ImtCore and those of the assigned schema providers.

	\ingroup imtassist
*/
class CUiRepresentationRegistryComp:
			public ilog::CLoggerComponentBase,
			public CUiRepresentationRegistry
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CUiRepresentationRegistryComp);
		I_REGISTER_INTERFACE(IUiRepresentationRegistry);
		I_ASSIGN(m_registerStandardAttrPtr, "RegisterStandardRepresentations", "Register the standard representations of ImtCore, e.g. imtassist.selectableList", true, true);
		I_ASSIGN_MULTI_0(m_schemaProvidersCompPtr, "SchemaProviders", "Providers of application-specific representation types", false);
	I_END_COMPONENT;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

private:
	I_ATTR(bool, m_registerStandardAttrPtr);
	I_MULTIREF(IUiRepresentationSchemaProvider, m_schemaProvidersCompPtr);
};


} // namespace imtassist


