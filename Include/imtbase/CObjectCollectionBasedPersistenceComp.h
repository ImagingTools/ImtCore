#pragma once


// ACF includes
#include <ifile/IFilePersistence.h>
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>


namespace imtbase
{


class CObjectCollectionBasedPersistenceComp:
			public ilog::CLoggerComponentBase,
			virtual public ifile::IFilePersistence
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CObjectCollectionBasedPersistenceComp);
		I_REGISTER_INTERFACE(ifile::IFilePersistence);
		I_REGISTER_INTERFACE(ifile::IFileTypeInfo);
		I_ASSIGN(m_objectTypeIdAttrPtr, "ObjectTypeId", "Object type ID", true, "ObjectTypeId");
		I_ASSIGN(m_commonDescriptionAttrPtr, "CommonDescription", "Common description", true, "CommonDescription");
		I_ASSIGN_MULTI_0(m_extensionAttrPtr, "FileExtension", "File extensions supported by this persistence", false);
		I_ASSIGN_MULTI_0(m_extensionDescAttrPtr, "FileExtensionDescription", "Descriptions for supported extensions", false);
		I_ASSIGN(m_collectionCompPtr, "ObjectCollection", "Object collection", false, "ObjectCollection");
		I_ASSIGN(m_fileTypeInfoCompPtr, "SourceFileTypeInfo", "FileTypeInfo used as source", false, "FileTypeInfo");
	I_END_COMPONENT;

	// reimplemented (ifile::IFilePersistence)
	virtual bool IsOperationSupported(
				const istd::IChangeable* dataObjectPtr,
				const QString* filePathPtr = nullptr,
				int flags = -1,
				bool beQuiet = true) const override;
	virtual int LoadFromFile(
				istd::IChangeable& data,
				const QString& filePath = QString(),
				ibase::IProgressManager* progressManagerPtr = nullptr) const override;
	virtual int SaveToFile(
				const istd::IChangeable& data,
				const QString& filePath = QString(),
				ibase::IProgressManager* progressManagerPtr = nullptr) const override;

	// reimplemented (ifile::IFileTypeInfo)
	virtual bool GetFileExtensions(QStringList& result, const istd::IChangeable* dataObjectPtr = nullptr, int flags = -1, bool doAppend = false) const override;
	virtual QString GetTypeDescription(const QString* extensionPtr = nullptr) const override;

private:
	int GetExtensionCount() const;

private:
	I_ATTR(QByteArray, m_objectTypeIdAttrPtr);
	I_TEXTATTR(m_commonDescriptionAttrPtr);
	I_MULTIATTR(QString, m_extensionAttrPtr);
	I_MULTITEXTATTR(m_extensionDescAttrPtr);
	I_REF(imtbase::IObjectCollection, m_collectionCompPtr);
	I_REF(ifile::IFileTypeInfo, m_fileTypeInfoCompPtr);
};


} // namespace imtbase


