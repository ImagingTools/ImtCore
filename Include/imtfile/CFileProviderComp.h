#pragma once


// Qt includes
#include <QtCore/QString>
#include <QtCore/QDir>

// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <ifile/IFileNameParam.h>
#include <istd/IPolymorphic.h>

// IMT includes
#include <imtfile/IFileProvider.h>


namespace imtfile
{

/**
 * \brief The IFileProvider class provides file from disc, resources or memory
 */
class CFileProviderComp: public IFileProvider, public ilog::CLoggerComponentBase
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CFileProviderComp);
		I_REGISTER_INTERFACE(IFileProvider);
		I_ASSIGN(m_homeDirPath, "HomePath", "Home path", true, QDir::currentPath());
		I_ASSIGN(m_pathsProblemsAutoSolve, "AutoSolvePathsProblems", "If 'true' servelt'll automatically solve paths problems", true, true);
		I_ASSIGN(m_fileTemplatePathCompPtr, "FileTemplateHomePath", "FileTemplate based Home path\n Note: type is ignored only value matters \n Warning: overwrites HomePath!", false, "FileTemplatePath");
	I_END_COMPONENT

// reimplemented (IFileProvider)
	virtual bool LoadData(QByteArray& data, const QByteArray& name) const override;

private:
	I_ATTR (QString, m_homeDirPath);
	I_ATTR (bool, m_pathsProblemsAutoSolve);
	I_REF(ifile::IFileNameParam, m_fileTemplatePathCompPtr);

};


} // namespace imtfile


