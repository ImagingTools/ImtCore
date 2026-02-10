// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// imtCore includes
#include <imtsdlgencpp/ICxxFileProcessor.h>
#include <imtsdlgencpp/IIncludeDirectivesProvider.h>


namespace imtsdlgencpp
{
/**
	Common definition for a C++ processor.
*/
class CCxxProcessorCompBase:
			public ilog::CLoggerComponentBase,
			virtual public IIncludeDirectivesProvider,
			virtual public ICxxFileProcessor
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_BASE_COMPONENT(CCxxProcessorCompBase);
		I_REGISTER_INTERFACE(IIncludeDirectivesProvider)
		I_REGISTER_INTERFACE(ICxxFileProcessor)
		I_ASSIGN(m_processorTypeIdAttrPtr, "ProcessorTypeId", "Type ID of a processor and directives\n\tWARNING: MUST be set!", false ,"")
	I_END_COMPONENT
	
	// reimplemented (ICxxFileProcessor)
	virtual QString GetProcessorTypeId() const override;

	// reimplemented (IIncludeDirectivesProvider)
	virtual QString GetDirectivesTypeId() const override;

protected:
	I_ATTR(QString, m_processorTypeIdAttrPtr);
};


// inline methods


// reimplemented (ICxxFileProcessor)
inline QString CCxxProcessorCompBase::GetProcessorTypeId() const
{
	Q_ASSERT(m_processorTypeIdAttrPtr.IsValid());
	return m_processorTypeIdAttrPtr.IsValid() ? *m_processorTypeIdAttrPtr : QString();
}


// reimplemented (IIncludeDirectivesProvider)

inline QString CCxxProcessorCompBase::GetDirectivesTypeId() const
{
	Q_ASSERT(m_processorTypeIdAttrPtr.IsValid());
	return m_processorTypeIdAttrPtr.IsValid() ? *m_processorTypeIdAttrPtr : QString();
}

} // namespace imtsdlgencpp
