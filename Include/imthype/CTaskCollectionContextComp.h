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


class CTaskCollectionContextComp:
			public icomp::CComponentBase,
			virtual public ITaskCollectionContext
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CTaskCollectionContextComp);
		I_REGISTER_INTERFACE(ITaskCollectionContext);
		I_REGISTER_INTERFACE(iser::ISerializable);
	I_END_COMPONENT;

	CTaskCollectionContextComp();

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

	QByteArray m_taskId;
	QByteArray m_taskInputId;
};


} // namespace imthype


