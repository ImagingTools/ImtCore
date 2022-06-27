#pragma once


// Qt includes
#include <QtCore/QString>
#include <QtCore/QDir>

// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <ifile/IFileNameParam.h>
#include <ifile/IRelativeFilePath.h>

// ImtCore includes
#include <imtbase/IBinaryDataProvider.h>

namespace imtbase
{


/**
	This component provides file from disc, resources or memory
 */
class CBinaryDataProviderComp: public IBinaryDataProvider, public ilog::CLoggerComponentBase
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CBinaryDataProviderComp);
		I_REGISTER_INTERFACE(IBinaryDataProvider);
		I_ASSIGN(m_homeDirPathAttrPtr, "HomePath", "Home path", false, "HomePath");
//		I_ASSIGN(m_homeDirPathAttrPtr, "HomePath", "Home path", true, QDir::currentPath());
		I_ASSIGN(m_pathsProblemsAutoSolveAttrPtr, "AutoSolvePathsProblems", "If 'true' servelt'll automatically solve paths problems", true, true);
		I_ASSIGN(m_fileTemplatePathCompPtr, "FileTemplateHomePath", "FileTemplate based Home path\n Note: type is ignored only value matters \n Warning: overwrites HomePath!", false, "FileTemplatePath");
	I_END_COMPONENT

	// reimplemented (IBinaryDataProvider)
	virtual bool GetData(QByteArray& data, const QByteArray& dataId) const override;

private:
//	I_ATTR (QString, m_homeDirPathAttrPtr);
	I_REF (ifile::IRelativeFilePath, m_homeDirPathAttrPtr);
	I_ATTR (bool, m_pathsProblemsAutoSolveAttrPtr);
	I_REF(ifile::IFileNameParam, m_fileTemplatePathCompPtr);
//	I_REF(ifile::IRelativeFilePath, m_relativeFilePathCompPtr);
};


} // namespace imtbase


