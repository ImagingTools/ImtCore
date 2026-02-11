// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IPolymorphic.h>

// ImtCore includes
#include <imtsdl/CSdlTools.h>
#include <imtsdl/ISdlEnumListProvider.h>
#include <imtsdl/ISdlUnionListProvider.h>
#include <imtsdl/ISdlTypeListProvider.h>
#include <imtsdlgencpp/CSdlGenTools.h>
#include <imtsdlgencpp/CCxxProcessorCompBase.h>


namespace imtsdlgencpp
{

/**
	A Qt QObject C++ class generator of SDL types
*/
class CSdlUnionObjectGeneratorComp:
			 public CCxxProcessorCompBase,
			 private imtsdl::CSdlTools,
			 private CSdlGenTools
{

public:
	typedef CCxxProcessorCompBase BaseClass;
	typedef imtsdl::CSdlTools BaseClass2;
	typedef CSdlGenTools BaseClass3;

	I_BEGIN_COMPONENT(CSdlUnionObjectGeneratorComp)
		I_ASSIGN(m_sdlTypeListCompPtr, "SdlTypeListProvider", "SDL types used to create a code", true, "SdlTypeListProvider")
		I_ASSIGN(m_sdlEnumListCompPtr, "SdlEnumListProvider", "SDL enums used to create a code", true, "SdlEnumListProvider")
		I_ASSIGN(m_sdlUnionListCompPtr, "SdlUnionListProvider", "SDL unioins used to create a code", true, "SdlUnionListProvider")
	I_END_COMPONENT



	// reimplemented (ICxxFileProcessor)
	virtual bool ProcessEntry(const imtsdl::CSdlEntryBase& sdlEntry, QIODevice* headerDevicePtr, QIODevice* sourceDevicePtr, const iprm::IParamsSet* paramsPtr) const override;

	// reimplemented (IIncludeDirectivesProvider)
	virtual QList<imtsdl::IncludeDirective> GetIncludeDirectives() const override;

private:
	bool ProcessHeaderClassFile(QTextStream& stream, const imtsdl::CSdlEntryBase& sdlEntry) const;
	bool ProcessHeaderClassListFile(QTextStream& stream, const imtsdl::CSdlEntryBase& sdlEntry) const;
	bool ProcessSourceClassListFile(QTextStream& stream, const imtsdl::CSdlEntryBase& sdlEntry) const;
	// void CreatePropertiesForType(QTextStream& stream, const imtsdl::SdlFieldList& fieldList) const;
	// void CreateAccessMethodsDefinition(QTextStream& stream, const imtsdl::SdlFieldList& fieldList) const;
	bool ProcessInsertToList(QTextStream& stream, const imtsdl::CSdlEntryBase& sdlEntry, const QByteArray& /*insertData*/) const;
	bool ProcessConvertListData(QTextStream& stream, const imtsdl::CSdlEntryBase& sdlEntry, const QByteArray& convertFormat) const;

private:
	I_REF(imtsdl::ISdlTypeListProvider, m_sdlTypeListCompPtr);
	I_REF(imtsdl::ISdlEnumListProvider, m_sdlEnumListCompPtr);
	I_REF(imtsdl::ISdlUnionListProvider, m_sdlUnionListCompPtr);

};


} // namespace imtsdlgencpp

