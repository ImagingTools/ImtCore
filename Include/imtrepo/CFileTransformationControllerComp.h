#pragma once


// ACF includes
#include <ilog//TLoggerCompWrap.h>

// ImtCore includes
#include <imtrepo/IRepositoryTransformationController.h>
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
	I_MULTIREF(IRepositoryFileTransformaton, m_transformationsCompPtr);
};


} // namespace imtrepo


