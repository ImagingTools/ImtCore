#pragma once

// ImtCore includes
#include <imtddl/CConstVarCreatorProcessorCompBase.h>


namespace imtddl
{

class CConstVarQrcCreatorProcessorComp: public CConstVarCreatorProcessorCompBase
{
public:
	typedef CConstVarCreatorProcessorCompBase BaseClass;

	I_BEGIN_COMPONENT(CConstVarQrcCreatorProcessorComp);
	I_END_COMPONENT;

protected:
	// reimplemented (CConstVarCreatorProcessorCompBase)
	virtual bool OpenFiles(const iprm::IParamsSet* paramsPtr);
	virtual bool CloseFiles(const iprm::IParamsSet *paramsPtr) override;
	virtual QString GetModuleName(const iprm::IParamsSet* paramsPtr) override;
	virtual bool CreateBody(const QString moduleName, const QJsonDocument& templateDocument, const iprm::IParamsSet* paramsPtr) override;

private:
	QFile m_outputFile;
};


} // namespace imtddl

