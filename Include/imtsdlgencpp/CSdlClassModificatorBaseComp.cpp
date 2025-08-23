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

// reimplemented (ICxxFileProcessor)

bool CSdlClassModificatorBaseComp::ProcessEntry (
			const imtsdl::CSdlEntryBase& sdlEntry,
			QIODevice* headerDevicePtr,
			QIODevice* sourceDevicePtr,
			const iprm::IParamsSet* paramsPtr) const
{
	Q_ASSERT(m_argumentParserCompPtr.IsValid());
	Q_ASSERT(m_sdlTypeListCompPtr.IsValid());
	Q_ASSERT(m_sdlEnumListCompPtr.IsValid());

	const imtsdl::CSdlType* sdlTypePtr =  dynamic_cast<const imtsdl::CSdlType*>(&sdlEntry);
	if (sdlTypePtr == nullptr || (headerDevicePtr == nullptr && sourceDevicePtr == nullptr)){
		I_CRITICAL();

		return false;
	}

	bool isOk = false;
	if (headerDevicePtr != nullptr){
		isOk = ProcessHeaderClassFile(*sdlTypePtr, headerDevicePtr, paramsPtr);
		if (!isOk){
			return false;
		}
	}

	if (sourceDevicePtr != nullptr){
		isOk = isOk && ProcessSourceClassFile(*sdlTypePtr, sourceDevicePtr, paramsPtr);
		if (!isOk){
			return false;
		}
	}

	return true;
}



} // namespace imtsdlgencpp

