// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QMutex>

// ACF includes
#include <icomp/CComponentBase.h>

// Acula includes
#include <imthype/ITaskCollectionContext.h>


namespace imthype
{


class CTaskCollectionContextDemultiplexerComp:
			public icomp::CComponentBase,
			virtual public ITaskCollectionContext
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CTaskCollectionContextDemultiplexerComp);
		I_REGISTER_INTERFACE(ITaskCollectionContext);
		I_ASSIGN_MULTI_0(m_outputsCompPtr, "Outputs", "List of the outputs", true);
	I_END_COMPONENT;

	CTaskCollectionContextDemultiplexerComp();

	// reimplemented (imthype::ITaskCollectionContext)
	virtual QByteArray GetTaskId() const override;
	virtual void SetTaskId(const QByteArray& taskId) override;
	virtual QByteArray GetTaskInputId() const override;
	virtual void SetTaskInputId(const QByteArray& inputId) override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

private:
#if QT_VERSION >= 0x060000
		mutable QRecursiveMutex m_dataMutex;
#else
		mutable QMutex m_dataMutex;
#endif

	I_MULTIREF(ITaskCollectionContext, m_outputsCompPtr);
};


} // namespace imthype


