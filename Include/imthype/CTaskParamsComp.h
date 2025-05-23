/********************************************************************************
**
**	Copyright (C) 2017-2020 ImagingTools GmbH
**
**	This file is part of the ImagingTools SDK.
**
**	This file may be used under the terms of the GNU Lesser
**	General Public License version 2.1 as published by the Free Software
**	Foundation and appearing in the file LicenseLGPL.txt included in the
**	packaging of this file.  Please review the following information to
**	ensure the GNU Lesser General Public License version 2.1 requirements
**	will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
**	If you are unsure which license is appropriate for your use, please
**	contact us at info@imagingtools.de.
**
**
********************************************************************************/


#pragma once


// ACF includes
#include <imod/TModelWrap.h>
#include <iprm/IIdParam.h>
#include <iprm/CComposedParamsSetComp.h>

// Acula includes
#include <aculatask/ITaskCollectionContext.h>


namespace aculatask
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
	virtual void OnComponentCreated();

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


} // namespace aculatask


