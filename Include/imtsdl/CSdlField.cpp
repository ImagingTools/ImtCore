#include <imtsdl/CSdlField.h>

#include <istd/CChangeNotifier.h>


namespace imtsdl
{


CSdlField::CSdlField():
	m_isRequired(false),
	m_isArray(false),
	m_isNonEmpty(false),
	m_type("INVALID")
{
}


bool CSdlField::IsRequired() const
{
	return m_isRequired;
}


void CSdlField::SetIsRequired(bool isRequired)
{
	if (m_isRequired != isRequired){
		istd::CChangeNotifier notifier(this);

		m_isRequired = isRequired;
	}
}


bool CSdlField::IsArray() const
{
	return m_isArray;
}


void CSdlField::SetIsArray(bool isArray)
{
	if (m_isArray != isArray){
		istd::CChangeNotifier notifier(this);

		m_isArray = isArray;
	}
}


bool CSdlField::IsNonEmpty() const
{
	return m_isNonEmpty;
}


void CSdlField::SetIsNonEmpty(bool isNonEmpty)
{
	if (m_isNonEmpty != isNonEmpty){
		istd::CChangeNotifier notifier(this);

		m_isNonEmpty = isNonEmpty;
	}
}


QString CSdlField::GetType() const
{
	return m_type;
}


void CSdlField::SetType(const QString& type)
{
	if (m_type != type){
		istd::CChangeNotifier notifier(this);

		m_type = type;
	}
}


QString CSdlField::GetId() const
{
	return m_id;
}


void CSdlField::SetId(const QString& id)
{
	if (m_id != id){
		istd::CChangeNotifier notifier(this);

		m_id = id;
	}
}

bool CSdlField::operator==(const CSdlField& other) const
{
	bool retVal = true;

	retVal = retVal && (other.m_isRequired == this->m_isRequired);
	retVal = retVal && (other.m_isArray == this->m_isArray);
	retVal = retVal && (other.m_isNonEmpty == this->m_isNonEmpty);
	retVal = retVal && (other.m_type == this->m_type);
	retVal = retVal && (other.m_id == this->m_id);

	return retVal;
}


// reimplemented(iser::ISerializable)
bool CSdlField::Serialize(iser::IArchive& archive)
{
	bool retVal = true;


	return retVal;
}


} // namespace imtsdl
