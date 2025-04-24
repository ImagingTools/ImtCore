#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include<iprm/ITextParam.h>

// ImtCore includes
#include <imtsdl/IModuleManager.h>
#include <imtsdl/ISdlTypeListProvider.h>
#include <imtsdl/ISdlEnumListProvider.h>
#include <imtsdl/ISdlRequestListProvider.h>
#include <imtsdl/ISdlUnionListProvider.h>
#include <imtsdl/ISdlDocumentTypeListProvider.h>
#include <imtsdl/ISdlProcessArgumentsParser.h>



namespace imtsdl
{


class CModuleManagerComp: virtual public IModuleManager,
			virtual public ilog::CLoggerComponentBase
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CModuleManagerComp)
		I_REGISTER_INTERFACE(IModuleManager)

		I_ASSIGN(m_typeListProviderCompPtr, "SdlTypeListProvider", "SDL types used to create a code", true, "SdlTypeListProvider")
		I_ASSIGN(m_enumListProviderCompPtr, "SdlEnumListProvider", "SDL enums used to create a code", true, "SdlEnumListProvider")
		I_ASSIGN(m_requestListProviderCompPtr, "SdlRequestListProvider", "SDL requests used to filter requests", true, "SdlRequestListProvider")
		I_ASSIGN(m_unionListProviderCompPtr, "SdlUnionListProvider", "SDL unioins used to create a code", true, "SdlUnionListProvider")
		I_ASSIGN(m_documentTypeListProviderCompPtr, "SdlDocumentListProvider", "SDL documents used to remove list from", true, "SdlDocumentListProvider")
		I_ASSIGN(m_argumentParserCompPtr, "ArgumentParser", "Command line process argument parser", true, "ArgumentParser")
		I_ASSIGN(m_schemaParamsCompPtr, "SchemaParams", "Original schema parameters, that contains additional options", true, "SchemaParams")
	I_END_COMPONENT

	// reimplemented(IModuleManager)
	bool Initialize(const QStringList& paths) override;
	std::shared_ptr<CSdlEntryBase> GetItem(const QString& itemId, ItemInfo* itemInfoPtr = nullptr) const override;
	ItemInfo GetItemInfo(const CSdlEntryBase& item) const override;
	bool CreateModuleFile() const override;

private:
	I_REF(ISdlTypeListProvider, m_typeListProviderCompPtr);
	I_REF(ISdlEnumListProvider, m_enumListProviderCompPtr);
	I_REF(ISdlRequestListProvider, m_requestListProviderCompPtr);
	I_REF(ISdlUnionListProvider, m_unionListProviderCompPtr);
	I_REF(ISdlDocumentTypeListProvider, m_documentTypeListProviderCompPtr);
	I_REF(iprm::IParamsSet, m_schemaParamsCompPtr);
	I_REF(ISdlProcessArgumentsParser, m_argumentParserCompPtr);


private:
	QList<CModule> m_modules;

};


} // namespace imtsdl
