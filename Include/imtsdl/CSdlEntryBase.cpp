// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtsdl/CSdlEntryBase.h>


// ACF includes
#include <istd/CChangeNotifier.h>
#include <iprm/CParamsSet.h>


imtsdl::CSdlEntryBase::CSdlEntryBase()
	: m_isExternal(false)
{
}


QString imtsdl::CSdlEntryBase::GetName() const
{
	return m_name;
}


void imtsdl::CSdlEntryBase::SetName(const QString& name)
{
	if (m_name != name){
		istd::CChangeNotifier notifier(this);
		m_name = name;
	}
}


QString imtsdl::CSdlEntryBase::GetTargetHeaderFilePath() const
{
	return m_targetHeaderFilePath;
}


void imtsdl::CSdlEntryBase::SetTargetHeaderFilePath(const QString& headerFilePath)
{
	if (m_targetHeaderFilePath != headerFilePath){
		istd::CChangeNotifier notifier(this);
		m_targetHeaderFilePath = headerFilePath;
	}
}


QString imtsdl::CSdlEntryBase::GetSchemaFilePath() const
{
	return m_schemaFilePath;
}


void imtsdl::CSdlEntryBase::SetSchemaFilePath(const QString& schemaFilePath)
{
	if (m_schemaFilePath != schemaFilePath){
		istd::CChangeNotifier notifier(this);
		m_schemaFilePath = schemaFilePath;
	}
}


QString imtsdl::CSdlEntryBase::GetObjectTargetHeaderFilePath() const
{
	return m_objectTargetHeaderFilePath;
}


void imtsdl::CSdlEntryBase::SetObjectTargetHeaderFilePath(const QString& objectHeaderFile)
{
	if (m_objectTargetHeaderFilePath != objectHeaderFile){
		istd::CChangeNotifier notifier(this);
		m_objectTargetHeaderFilePath = objectHeaderFile;
	}
}


QString imtsdl::CSdlEntryBase::GetObjectTargetSourceFilePath() const
{
	return m_objectTargetSourceFilePath;
}


void imtsdl::CSdlEntryBase::SetObjectTargetSourceFilePath(const QString& objectSourceFile)
{
	if (m_objectTargetSourceFilePath != objectSourceFile){
		istd::CChangeNotifier notifier(this);
		m_objectTargetSourceFilePath = objectSourceFile;
	}
}

QString imtsdl::CSdlEntryBase::GetFwdHeaderTargetFilePath() const
{
	return m_fwdHeaderTargetFilePath;
}


void imtsdl::CSdlEntryBase::SetFwdHeaderTargetFilePath(const QString& fwdHeaderFile)
{
	if (m_fwdHeaderTargetFilePath != fwdHeaderFile){
		istd::CChangeNotifier notifier(this);
		m_fwdHeaderTargetFilePath = fwdHeaderFile;
	}
}


bool imtsdl::CSdlEntryBase::IsExternal() const
{
	return m_isExternal;
}


void imtsdl::CSdlEntryBase::SetExternal(bool isExternal)
{
	if (m_isExternal != isExternal){
		istd::CChangeNotifier notifier(this);
		m_isExternal = isExternal;
	}
}


QString imtsdl::CSdlEntryBase::GetQmlImportDeclaration() const
{
	return m_qmlImportDeclaration;
}


void imtsdl::CSdlEntryBase::SetQmlImportDeclaration(const QString& qmlImportDeclaration)
{
	if (m_qmlImportDeclaration != qmlImportDeclaration){
		istd::CChangeNotifier notifier(this);
		m_qmlImportDeclaration = qmlImportDeclaration;
	}
}


const iprm::IParamsSet& imtsdl::CSdlEntryBase::GetSchemaParams() const
{
	if (m_schemaParamsPtr == nullptr){
		static const iprm::CParamsSet dummyParams;

		return dummyParams;
	}

	return *m_schemaParamsPtr;
}


void imtsdl::CSdlEntryBase::SetSchemaParamsPtr(const std::shared_ptr<iprm::IParamsSet>& schemaParamsPtr)
{
	m_schemaParamsPtr = schemaParamsPtr;

	// ensure, params is comparable
	if (schemaParamsPtr != nullptr){
		const bool isEqual = schemaParamsPtr->IsEqual(*schemaParamsPtr);
		if (!isEqual){
			Q_ASSERT_X(false, "Initializing incomparable paras!", __func__);
			qFatal("Initializing incomparable paras!");
		}
	}
}
