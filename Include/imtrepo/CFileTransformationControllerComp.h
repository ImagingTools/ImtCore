#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtrepo/IFileCollectionInfo.h>
#include <imtrepo/IRepositoryTransformationController.h>
#include <imtrepo/IRepositoryItemInfoProvider.h>
#include <imtrepo/IRepositoryFileTransformation.h>


namespace imtrepo
{


/**
	File repository transformation controller.
	\ingroup FileRepository
*/
class CFileTransformationControllerComp:
			public ilog::CLoggerComponentBase,
			virtual public IRepositoryTransformationController
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CFileTransformationControllerComp);
		I_REGISTER_INTERFACE(IRepositoryTransformationController);
		I_ASSIGN_MULTI_0(m_transformationsCompPtr, "FileTransformations", "List of file transformations", true);
	I_END_COMPONENT;

	// reimplemented (IRepositoryTransformationController)
	virtual bool TransformRepository(IFileObjectCollection& repository, int fromRevision, int toRevision) const override;

protected:
	enum TransformationState
	{
		TS_IDLE = 0,
		TS_IN_PROGRESS,
		TS_REPLACING
	};

	class RepositoryItemInfo: virtual public IRepositoryItemInfo
	{
	public:
		// reimplemented (IRepositoryItemInfo)
		virtual RepositoryFileTypes GetRepositoryItemFileTypes() const override;
		virtual QString GetRepositoryItemFilePath(RepositoryFileType fileId) const override;

		QMap<int, QString> m_files;
	};

protected:
	imtbase::ICollectionInfo::Ids GetRepositoryItemIds(IFileObjectCollection& repository) const;
	const IRepositoryItemInfo* GetRepositoryItemInfo(IFileObjectCollection& repository, const QByteArray& itemId) const;
	TransformationState GetTransformationState(IFileObjectCollection& repository, bool &isOk) const;
	bool SetTransformationState(IFileObjectCollection& repository, TransformationState state) const;
	bool ReplaceWithTransformedItems(IFileObjectCollection& repository) const;
	bool CleanupTrasformation(IFileObjectCollection& repository) const;

protected:
	I_MULTIREF(IRepositoryFileTransformation, m_transformationsCompPtr);
};


} // namespace imtrepo


