// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <imod/TModelWrap.h>
#include <iprm/IIdParam.h>
#include <iprm/CComposedParamsSetComp.h>

// Acula includes
#include <imthype/ITaskCollectionContext.h>


namespace imthype
{


class CTaskParamsComp: public iprm::CComposedParamsSetComp
{
public:
	typedef iprm::CComposedParamsSetComp BaseClass;

	I_BEGIN_COMPONENT(CTaskParamsComp);
		I_ASSIGN(m_collectionContextCompPtr, "CollectionContext", "Context of the task-related parameters", true, "CollectionContext");
	I_END_COMPONENT;

	// reimplemented (iprm::IParamsSet)
	virtual Ids GetParamIds(bool editableOnly = false) const override;
	virtual const iser::ISerializable* GetParameter(const QByteArray& id) const override;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

private:
	class TaskIdParam: virtual public iprm::IIdParam
	{
	public:
		TaskIdParam();

		void SetCollectionContext(const ITaskCollectionContext* contextPtr);

		// reimplemented (iprm::IIdParam)
		virtual QByteArray GetId() const override;
		virtual void SetId(const QByteArray& id) override;

		// reimplemented (iser::ISerializable)
		virtual bool Serialize(iser::IArchive& archive) override;

	protected:
		const ITaskCollectionContext* m_contextPtr;
	};

	class TaskInputIdParam: public TaskIdParam
	{
	public:
		// reimplemented (iprm::IIdParam)
		virtual QByteArray GetId() const override;
	};

	imod::TModelWrap<TaskIdParam> m_taskIdParam;
	imod::TModelWrap<TaskInputIdParam> m_taskInputIdParam;
private:
	I_REF(ITaskCollectionContext, m_collectionContextCompPtr);
};


} // namespace imthype


