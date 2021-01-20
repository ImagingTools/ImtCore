#include <imtlog/CMessageFilterParams.h>


namespace imtlog
{


CMessageFilterParams::FilterMode CMessageFilterParams::GetFilterMode() const
{
	return FilterMode();
}


void CMessageFilterParams::SetFilterMode(FilterMode mode)
{
}


QSet<int> CMessageFilterParams::GetMessageFilterIds() const
{
	return QSet<int>();
}


void CMessageFilterParams::SetMessageFilterIds(const QSet<int>& ids)
{
}


}


