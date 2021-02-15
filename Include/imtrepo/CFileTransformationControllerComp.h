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
		I_ASSIGN(m_repositoryCompPtr, "Repository", "Repositor of items for transformation", true, "Repository");
		I_ASSIGN(m_repositoryItemInfoProviderCompPtr, "RepositoryItemInfoProvider", "Provider of items for transformation", true, "RepositoryItemInfoProvider");
		I_ASSIGN_MULTI_0(m_transformationsCompPtr, "FileTransformations", "List of file transformations", true);
	I_END_COMPONENT;

	// reimplemented (IRepositoryTransformationController)
	virtual bool TransformRepository(int fromRevision, int toRevision) const override;

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
	TransformationState GetTransformationState(bool &isOk) const;
	bool SetTransformationState(TransformationState state) const;
	bool ApplyNewRevision() const;
	bool CleanupTrasformation() const;

protected:
	I_REF(IFileCollectionInfo, m_repositoryCompPtr);
	I_REF(IRepositoryItemInfoProvider, m_repositoryItemInfoProviderCompPtr);
	I_MULTIREF(IRepositoryFileTransformaton, m_transformationsCompPtr);
};


} // namespace imtrepo


