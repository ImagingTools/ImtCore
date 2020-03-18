#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <ifile/IFilePersistence.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtfile/IFileCompression.h>


namespace imtbase
{


class CCompositeObjectPersistenceComp:
			public ilog::CLoggerComponentBase,
			virtual public ifile::IFilePersistence
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CCompositeObjectPersistenceComp);
		I_REGISTER_INTERFACE(ifile::IFilePersistence);
		I_REGISTER_INTERFACE(ifile::IFileTypeInfo);
		I_ASSIGN(m_fileCompressionCompPtr, "FileCompressor", "File compressor", false, "FileCompressor");
		I_ASSIGN_MULTI_0(m_objectTypeIdsAttrPtr, "ObjectTypeIds", "List of type-IDs for corresponding persistences", true);
		I_ASSIGN_MULTI_0(m_objectPresistencesCompPtr, "ObjectPersistences", "List of registered persistence according to the specified type-IDs", true);
	I_END_COMPONENT;

	// reimplemented (ifile::IFilePersistence)
	virtual bool IsOperationSupported(
				const istd::IChangeable* dataObjectPtr,
				const QString* filePathPtr = NULL,
				int flags = -1,
				bool beQuiet = true) const override;
	virtual int LoadFromFile(istd::IChangeable& data,
				const QString& filePath = QString(),
				ibase::IProgressManager* progressManagerPtr = NULL) const override;
	virtual int SaveToFile(const istd::IChangeable& data,
				const QString& filePath = QString(),
				ibase::IProgressManager* progressManagerPtr = NULL) const override;

	// reimplemented (ifile::IFileTypeInfo)
	virtual bool GetFileExtensions(QStringList& result, const istd::IChangeable* dataObjectPtr = NULL, int flags = -1, bool doAppend = false) const override;
	virtual QString GetTypeDescription(const QString* extensionPtr = NULL) const override;

protected:
	struct BundleElementInfo
	{
		QString fileName;
		QByteArray id;
		QByteArray typeId;
		QString name;
		QString description;
	};

	const ifile::IFilePersistence* GetFilePersistenceForTypeId(const QByteArray& typeId) const;
	bool SerializeBundleMetaInfo(QVector<BundleElementInfo>& contentMetaInfo, iser::IArchive& archive) const;

private:
	I_REF(imtfile::IFileCompression, m_fileCompressionCompPtr);
	I_MULTIATTR(QByteArray, m_objectTypeIdsAttrPtr);
	I_MULTIREF(ifile::IFilePersistence, m_objectPresistencesCompPtr);
};


} // namespace imtbase


