#include <imtgql/CGqlEnum.h>


namespace imtgql
{


// public methods

CGqlEnum::CGqlEnum(const QByteArray& value)
	:m_value(value)
{
}


QByteArray CGqlEnum::GetValue()
{
	return m_value;
}


CGqlEnum::operator QVariant() const
{
	return QVariant::fromValue(*this);
}


} // namespace imtgql


