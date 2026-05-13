// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtcom/CStaticOptionList.h>


namespace imtcom
{


int CStaticOptionList::GetOptionOperationFlags(int /*index*/) const
{
	return OOF_SUPPORT_ENABLING | OOF_SUPPORT_SWAP | OOF_COUNT_FIXED;
}


bool CStaticOptionList::RemoveOption(int /*index*/)
{
	Q_ASSERT(false);

	return false;
}


bool CStaticOptionList::InsertOption(
			const QString& /*optionName*/,
			const QByteArray& /*optionId*/,
			const QString& /*optionDescription*/,
			int /*index*/)
{
	Q_ASSERT(false);

	return false;
}


void CStaticOptionList::ResetOptions()
{
	// do nothing
}


} // namespace imtcom


