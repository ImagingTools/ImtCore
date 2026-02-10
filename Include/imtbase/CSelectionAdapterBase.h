// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QSharedPointer>

// ACF includes
#include <imod/CMultiModelDispatcherBase.h>


namespace imtbase
{


class CSelectionAdapterBase: public imod::CMultiModelDispatcherBase
{
public:
	CSelectionAdapterBase();

protected:
	class UpdateBlocker
	{
	public:
		UpdateBlocker(bool& flag);
		~UpdateBlocker();

	private:
		bool& m_flag;
	};

	typedef QSharedPointer<UpdateBlocker> UpdateBlockerPtr;

protected:
	virtual void OnConstraintsChanged();
	virtual void OnSelectionChanged();
	virtual UpdateBlockerPtr CreateUpdateBlocker();

	// reimplemented (imod::CMultiModelDispatcherBase)
	virtual void OnModelChanged(int modelId, const istd::IChangeable::ChangeSet& changeSet) override;

protected:
	enum ModelId
	{
		MI_CONSTRAINTS = 0,
		MI_SELECTION
	};

private:
	bool m_isUpdateBlocked;
};


} // namespace imtbase


