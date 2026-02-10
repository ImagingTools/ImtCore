// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <iprm/COptionsManager.h>


namespace imtcom
{


class CStaticOptionList: public iprm::COptionsManager
{
public:
	// IOptionsManager
	virtual int GetOptionOperationFlags(int index) const override;
	virtual bool RemoveOption(int index) override;
	virtual bool InsertOption(const QString& optionName, const QByteArray& optionId, const QString& optionDescription, int index) override;

	// COptionsManager
	virtual void ResetOptions() override;
};


} // namespace imtcom


