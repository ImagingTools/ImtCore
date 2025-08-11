#include "CSdlClassModificatorBaseComp.h"


// Qt includes
#include <QtCore/QDir>
#include <QtCore/QRegularExpression>
#include <QtCore/QRegularExpressionMatch>

//Acf includes
#include <istd/CSystem.h>

// imtbase includes
#include <imtbase/CTempDir.h>

// ImtCore includes
#include <imtsdl/CSdlType.h>


namespace imtsdlgencpp
{


// public methods

//reimplemented(iproc::IProcessor)

bool CSdlClassModificatorBaseComp::ProcessType(const imtsdl::CSdlType& sdlType, QFile* headerFilePtr, QFile* sourceFilePtr)
{
	Q_ASSERT(m_argumentParserCompPtr.IsValid());
	Q_ASSERT(m_sdlTypeListCompPtr.IsValid());
	Q_ASSERT(m_sdlEnumListCompPtr.IsValid());

	if (!m_argumentParserCompPtr->IsModificatorEnabled(*m_processorModificatorNameAttrPtr)){
		// nothing todo if our modificator is not exsits
		return true;
	}

	m_headerFilePtr = headerFilePtr;
	m_sourceFilePtr = sourceFilePtr;

	bool isOk = ProcessHeaderClassFile(sdlType);
	if (!isOk){
		return false;
	}

	isOk = isOk && ProcessSourceClassFile(sdlType);
	if (!isOk){
		return false;
	}

	return true;
}



} // namespace imtsdlgencpp

