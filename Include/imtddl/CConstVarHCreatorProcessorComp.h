// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// ImtCore includes
#include <imtddl/CConstVarCreatorProcessorCompBase.h>

// Qt includes
#include <QtCore/QTextStream>


namespace imtddl
{

class CConstVarHCreatorProcessorComp: public CConstVarCreatorProcessorCompBase
{
public:
	typedef CConstVarCreatorProcessorCompBase BaseClass;

	I_BEGIN_COMPONENT(CConstVarHCreatorProcessorComp);
	I_END_COMPONENT;
protected:
	// reimplemented (CConstVarCreatorProcessorCompBase)
	virtual bool OpenFiles(const iprm::IParamsSet* paramsPtr) override;
	virtual bool CloseFiles(const iprm::IParamsSet *paramsPtr) override;
	virtual QString GetModuleName(const iprm::IParamsSet* paramsPtr) override;
	virtual bool CreateBody(const QString& moduleName, const QJsonDocument& templateDocument, const iprm::IParamsSet* paramsPtr) override;

private:
	QFile m_outputFile;
};


} // namespace imtddl

