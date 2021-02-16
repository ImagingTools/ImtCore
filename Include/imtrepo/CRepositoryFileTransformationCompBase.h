#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtrepo/IRepositoryFileTransformation.h>


namespace imtrepo
{


class CRepositoryFileTransformationCompBase:
			public ilog::CLoggerComponentBase,
			virtual public IRepositoryFileTransformation
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CRepositoryFileTransformationCompBase);
		I_REGISTER_INTERFACE(IRepositoryFileTransformation);
		I_ASSIGN(m_minRevisionAttrPtr, "MinRevision", "Minimum supported revision", true, 0);
		I_ASSIGN(m_maxRevisionAttrPtr, "MaxRevision", "Maximum supported revision", true, 0);
	I_END_COMPONENT;

	// reimplemented (IRepositoryFileTransformation)
	virtual istd::CIntRange GetSupportedRevisionRange() const override;
	virtual bool TransformFile(const IRepositoryItemInfo& repositoryIteminfo, int fromRevision, int toRevision) const override;

protected:
	I_ATTR(int, m_minRevisionAttrPtr);
	I_ATTR(int, m_maxRevisionAttrPtr);
};


} // namespace imtrepo


