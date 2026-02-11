// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ilog/IMessageConsumer.h>
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtsdl/ISdlProcessArgumentsParser.h>


namespace imtsdl
{


/**
	Filter for logging component.
*/
class CLogFilterComp:
			public icomp::CComponentBase,
			virtual public ilog::IMessageConsumer
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CLogFilterComp);
		I_REGISTER_INTERFACE(ilog::IMessageConsumer);
		I_ASSIGN(m_logCompPtr, "Log", "Consumer log messages", true, "Log");
		I_ASSIGN(m_argumentParserCompPtr, "ArgumentParser", "Command line process argument parser", true, "ArgumentParser")
	I_END_COMPONENT;

	// reimplemented (ilog::IMessageConsumer)
	virtual bool IsMessageSupported(
				int messageCategory = -1,
				int messageId = -1,
				const istd::IInformationProvider* messagePtr = NULL) const override;
	virtual void AddMessage(const ilog::IMessageConsumer::MessagePtr& messagePtr) override;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

private:
	I_REF(ilog::IMessageConsumer, m_logCompPtr);
	I_REF(ISdlProcessArgumentsParser, m_argumentParserCompPtr);
};


} // namespace imtsdl


