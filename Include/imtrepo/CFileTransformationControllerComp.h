#pragma once


// ACF includes
#include <ilog//TLoggerCompWrap.h>

// ImtCore includes
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
			virtual public IRepositoryTransformationController,
			virtual protected IRepositoryItemInfoProvider
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CFileTransformationControllerComp);
		I_REGISTER_INTERFACE(IRepositoryTransformationController);
		I_ASSIGN_MULTI_0(m_transformationsCompPtr, "FileTransformations", "List of file transformations", true);
	I_END_COMPONENT;

	CFileTransformationControllerComp();

	// reimplemented (IRepositoryTransformationController)
	virtual bool TransformRepository(IFileObjectCollection& repository, int fromRevision, int toRevision) const override;

protected:
	enum TransformationState
	{
		TS_IDLE = 0,
		TS_IN_PROGRESS,
		TS_REPLACING
	};

protected:
	TransformationState GetTransformationState(IFileObjectCollection& repository, bool &isOk) const;
	bool SetTransformationState(IFileObjectCollection& repository, TransformationState state) const;
	bool ApplyNewRevision(IFileObjectCollection& repository) const;
	bool CleanupTrasformation(IFileObjectCollection& repository) const;

	// reimplemented (IRepositoryItemInfoProvider)
	virtual ItemIds GetRepositoryItemIds() const override;
	virtual FileIds GetRepositoryItemFileIds(const QByteArray& itemId) const override;
	virtual QString GetRepositoryItemFilePath(const QByteArray& itemId, FileId fileId) const override;

protected:
	I_MULTIREF(IRepositoryFileTransformaton, m_transformationsCompPtr);

	mutable IRepositoryItemInfoProvider* m_itemInfoProvider;
	mutable IRepositoryItemInfoProvider::ItemIds m_repositoryItemIds;
};


} // namespace imtrepo


