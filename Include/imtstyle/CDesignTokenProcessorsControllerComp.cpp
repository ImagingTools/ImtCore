#include <imtstyle/CDesignTokenProcessorsControllerComp.h>


namespace imtstyle
{

void CDesignTokenProcessorsControllerComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	QList<IDesignTokenProcessor*> failedProcessors;
	for (int i = 0; i < m_designTokenProcessors.GetCount(); ++i){
		if(m_designTokenProcessors[i]->Exec() != 0){
			failedProcessors << m_designTokenProcessors[i];
		}
	}

	if(!m_argumentParserAttrPtr->IsErrorsIgnoreRequested()){

		for(const IDesignTokenProcessor* processor: ::qAsConst(failedProcessors)){
			qInfo() << "--------------------------Error";
			qInfo() << processor->GetHelpString();
		}

		if(failedProcessors.size() == m_designTokenProcessors.GetCount()){
			::exit(-1);
		}

	}
	::exit(0);
}



} // namespace imtstyle


