// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// Qt includes
#include <QtCore/QList>

// ACF includes
#include <istd/IPolymorphic.h>


namespace imtsdlgencpp
{


class ICxxModifier: virtual public istd::IPolymorphic
{


public:
	/**
		\c Name - name of variable; MUST NOT be empty
		\c Type - a valid FULL(with all namespaces) type of variable; MUST NOT be empty
		\c DefaultValue - defaul value; could be NULL
	 */
	struct Argument
	{
		QString Name;
		QString Type;
		QString DefaultValue;
	};

	typedef QList<Argument> ArgumentList;
	/**
		Returns method's name, allows to read object from container
		\example ReadFromJsonObject
	*/
	[[nodiscard]] virtual QString GetReadMethodName() const = 0;
	/**
		Returns method's name, allows to write object to container
		\example WriteToJsonObject
	*/
	[[nodiscard]] virtual QString GetWriteMethodName() const = 0;
	/**
		Returns a methods's argument list, used for methods call, declaration. 
		\warning The order matters!
		\warning First argument ALWAYS MUST be a <CONTAINER> with <CONTAINER_TYPE>
		\warning list NUST NOT be empty
		\note default value for first(CONTAINER) argument SHOULD be NULL, it will be ignored
		\example [{Name: jsonObject, Type: QJsonObject}]
		\example [{Name: model, Type: ::imtbase::CTreeItemModel}, {Name: modelIndex, Type: int, DefaultValue: 0}]
	*/
	[[nodiscard]] virtual ArgumentList GetArguments() const = 0;
};




} // namespace imtsdlgencppgencpp
