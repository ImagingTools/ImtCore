// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtbase/CMimeType.h>


// Qt includes
#include <QtCore/QRegularExpression>
#include <QtCore/QRegularExpressionMatch>

// ACF includes
#include <istd/TDelPtr.h>
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>


namespace imtbase
{


// reimplemented (IMimeType)

const QString& CMimeType::GetType() const
{
	return m_type;
}


bool CMimeType::SetType(const QString& type)
{
	if (!isTypeValid(type)){
		return false;
	}

	if (m_type != type){
		istd::CChangeNotifier notifier(this);

		m_type = type;
	}

	return true;
}


QStringList CMimeType::GetTree() const
{
	return m_tree;
}


bool CMimeType::SetTree(const QStringList& tree)
{
	if (!isTreeValid(tree)){
		return false;
	}

	if (m_tree != tree){
		istd::CChangeNotifier notifier(this);

		m_tree = tree;
	}

	return true;
}


const QString& CMimeType::GetSubType() const
{
	return m_subType;
}


bool CMimeType::SetSubType(const QString& subType)
{
	if (!isSubTypeValid(subType)){
		return false;
	}

	if (m_subType != subType){
		istd::CChangeNotifier notifier(this);

		m_subType = subType;
	}

	return true;
}


const QString& CMimeType::GetSuffix() const
{
	return m_suffix;
}


bool CMimeType::SetSuffix(const QString& suffix)
{
	if (!isSuffixValid(suffix)){
		return false;
	}

	if (m_suffix != suffix){
		istd::CChangeNotifier notifier(this);

		m_suffix = suffix;
	}

	return true;
}


const IMimeType::Parameters& CMimeType::GetParameters() const
{
	return m_parameters;
}


bool CMimeType::SetParameters(const Parameters& parameters)
{
	if (!isParametersValid(parameters)){
		return false;
	}

	if (m_parameters != parameters){
		istd::CChangeNotifier notifier(this);

		m_parameters = parameters;
	}

	return true;
}


QString CMimeType::ToString()
{
	QString retVal;

	if (m_type.isEmpty() || m_subType.isEmpty()){
		return retVal;
	}

	retVal += m_type + "/";

	for (const QString& item : m_tree){
		retVal += item + ".";
	}

	retVal += m_subType;

	if (!m_suffix.isEmpty()){
		retVal += "+" + m_suffix;
	}

	for (const QString& key : m_parameters.keys()){
		retVal += ";" + key + "=" + m_parameters[key];
	}

	return retVal;
}


bool CMimeType::FromString(const QString& string)
{
	static QRegularExpression mimeRegExp("(?<type>\\w*)\\/(?<subtype>[\\w\\.-]*)(?:\\+(?<typeext>[\\w\\.-]*))?(?:;(?:(?<key>.+)=(?<value>.*))*)?");

	QRegularExpressionMatch mimeRegExpMatch = mimeRegExp.match(string);
	if (mimeRegExpMatch.hasMatch()){
		m_type = mimeRegExpMatch.captured("type");
		m_subType = mimeRegExpMatch.captured("subtype");
		m_suffix = mimeRegExpMatch.captured("typeext");

		QString key = mimeRegExpMatch.captured("key").trimmed();
		QString value = mimeRegExpMatch.captured("value").trimmed();
		if (!key.isEmpty() && !value.isEmpty()){
			m_parameters[key] = value;
		}

		return true;
	}

	return false;
}


// protected methods

bool CMimeType::isTypeValid(const QString& /*type*/)
{
	return true;
}


bool CMimeType::isTreeValid(const QStringList& /*tree*/)
{
	return true;
}


bool CMimeType::isSubTypeValid(const QString& /*subType*/)
{
	return true;
}


bool CMimeType::isSuffixValid(const QString& /*suffix*/)
{
	return true;
}


bool CMimeType::isParametersValid(const Parameters& /*parameters*/)
{
	return true;
}


} // namespace imtbase


