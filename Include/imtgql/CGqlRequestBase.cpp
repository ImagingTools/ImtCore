#include <imtgql/CGqlRequestBase.h>


// ACF includes
#include <istd/TDelPtr.h>
#include <istd/CChangeNotifier.h>


namespace imtgql
{


// public methods

CGqlRequestBase::CGqlRequestBase(const QByteArray& commandId)
	:m_commandId(commandId)
{
}


// reimplemented (IGqlRequest)

void CGqlRequestBase::InsertField(const QByteArray& fieldId)
{
	if (!m_fieldsMap.contains(fieldId)){
		Field newField;
		newField.id = fieldId;

		m_fieldsMap[fieldId] = newField;
	}
}


void CGqlRequestBase::InsertFieldArgument(const QByteArray & fieldId, const QByteArray & argumentId, const QVariant & value)
{
	if (m_fieldsMap.contains(fieldId)){
		Field& field = m_fieldsMap[fieldId];

		Argument argument;
		argument.key = argumentId;
		argument.value = value;

		field.arguments.push_back(argument);
	}
}


QByteArray CGqlRequestBase::GetCommandId() const
{
	return m_commandId;
}


QByteArrayList CGqlRequestBase::GetFields() const
{
	return m_fieldsMap.keys();
}


QByteArrayList CGqlRequestBase::GetFieldArguments(const QByteArray& /*fieldId*/) const
{
	Q_ASSERT_X(false, __FILE__, "CGqlRequestBase::GetFieldArguments not implemented yet");

	return QByteArrayList();
}


// reimplemented (iser::IObject)

QByteArray CGqlRequestBase::GetFactoryId() const
{
	return m_commandId;
}


// reimplemented (iser::ISerializable)

bool CGqlRequestBase::Serialize(iser::IArchive& /*archive*/)
{
	return false;
}


// reimplemented (istd::IChangeable)

int CGqlRequestBase::GetSupportedOperations() const
{
	return SO_COPY | SO_RESET;
}


bool CGqlRequestBase::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	const CGqlRequestBase* sourcePtr = dynamic_cast<const CGqlRequestBase*>(&object);
	if (sourcePtr != nullptr){
		istd::CChangeNotifier changeNotifier(this);

		m_commandId = sourcePtr->m_commandId;
		m_fieldsMap = sourcePtr->m_fieldsMap;

		return true;
	}

	return false;
}



bool CGqlRequestBase::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier changeNotifier(this);

	m_commandId.clear();
	m_fieldsMap.clear();

	return true;
}


} // namespace imtgql


