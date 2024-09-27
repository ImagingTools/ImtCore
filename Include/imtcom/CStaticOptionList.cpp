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


