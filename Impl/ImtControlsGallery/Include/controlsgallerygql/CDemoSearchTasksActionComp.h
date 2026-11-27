#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtassist/IAssistantAction.h>

// ControlsGallery includes
#include <controlsgallerygql/IDemoTaskStore.h>


namespace controlsgallerygql
{


/**
	Reading assistant action of the example: "tasks.search".
	The result is shown as the standard selectable list.
*/
class CDemoSearchTasksActionComp:
			public icomp::CComponentBase,
			virtual public imtassist::IAssistantAction
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CDemoSearchTasksActionComp);
		I_REGISTER_INTERFACE(imtassist::IAssistantAction);
		I_ASSIGN(m_taskStoreCompPtr, "TaskStore", "Tasks of the example", true, "TaskStore");
	I_END_COMPONENT;

	CDemoSearchTasksActionComp();

	// reimplemented (imtassist::IAssistantAction)
	virtual QByteArray GetActionId() const override;
	virtual QString GetDescription() const override;
	virtual ActionKind GetActionKind() const override;
	virtual const imtassist::CParameterSchema& GetParameterSchema() const override;
	virtual const imtassist::CParameterSchema& GetResultSchema() const override;
	virtual bool CheckAccess(const imtassist::CAssistantRequestContext& context, const QJsonObject& parameters) const override;
	virtual ExecutionStatus Execute(
				const imtassist::CAssistantRequestContext& context,
				const QJsonObject& parameters,
				Result& result,
				QString& userError) override;
	virtual ExecutionStatus PrepareChange(
				const imtassist::CAssistantRequestContext& context,
				const QJsonObject& parameters,
				ChangePreview& preview,
				QString& userError) override;
	virtual ExecutionStatus ApplyChange(
				const imtassist::CAssistantRequestContext& context,
				const QJsonObject& parameters,
				const QByteArray& stateToken,
				Result& result,
				QString& userError) override;

private:
	I_REF(IDemoTaskStore, m_taskStoreCompPtr);

	imtassist::CParameterSchema m_parameterSchema;
	imtassist::CParameterSchema m_resultSchema;
};


} // namespace controlsgallerygql


