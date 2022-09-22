#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <iprm/IOptionsList.h>
#include <ifile/IFilePersistence.h>

// ImtCore includes
#include <imtbase/ICollectionDataController.h>


namespace imtbase
{


class CCollectionDataControllerComp:
			public ilog::CLoggerComponentBase,
			virtual public imtbase::ICollectionDataController
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CCollectionDataControllerComp)
		I_REGISTER_INTERFACE(ICollectionDataController);
		I_ASSIGN(m_resourceTypesCompPtr, "ResourceTypes", "List of supported resource types", false, "ResourceTypes");
		I_ASSIGN_MULTI_0(m_objectPersistenceListCompPtr, "ObjectPersistenceList", "List of persistence components used for data object loading", false);
		I_ASSIGN_MULTI_0(m_resourceFileTypesCompPtr, "FileTypeInfos", "List of file type infos for corresponding resource type", false);
		I_ASSIGN_MULTI_0(m_objectFactoryListCompPtr, "ObjectFactoryList", "List of object factories for corresponding resource type", false);
	I_END_COMPONENT;

public:
	// reimplemented (ICollectionDataController)
	virtual const ifile::IFilePersistence* GetPersistenceForObjectType(const QByteArray& typeId) const override;
	virtual bool ExportFile(const imtbase::IObjectCollection& collection, const QByteArray& objectId, const QString& targetFilePath = QString()) const override;
	virtual imtbase::ICollectionInfo::Id ImportFile(imtbase::IObjectCollection& collection, const QByteArray& typeId, const QString& sourceFilePath = QString(), const ICollectionInfo::Id& parentId = ICollectionInfo::Id()) const override;

protected:
	virtual istd::IChangeable* CreateObjectInstance(const QByteArray& typeId) const;

protected:
	/**
		List of supported resource types.
	*/
	I_REF(iprm::IOptionsList, m_resourceTypesCompPtr);

	/**
		List of file persistence components related to registered resource types.
	*/
	I_MULTIREF(ifile::IFilePersistence, m_objectPersistenceListCompPtr);

	/**
		List of file persistence components related to registered resource types.
	*/
	I_MULTIFACT(istd::IChangeable, m_objectFactoryListCompPtr);

	/**
		List of file type infos related to registered resource types.
	*/
	I_MULTIREF(ifile::IFileTypeInfo, m_resourceFileTypesCompPtr);
};


} // namespace imtbase


