import QtQuick 2.0

/*!
	\qmltype AcfFactory
	\inqmlmodule imtacf
	\brief Описание фабрики компонента (аналог I_FACT в ACF).

	AcfFactory декларирует, что компонент-владелец может создавать экземпляры
	компонентов, реализующих указанный интерфейс.

	\qml
	AcfComponent {
		AcfFactory { factoryId: "ItemFactory"; interfaceName: "IListItem" }
	}
	\endqml
*/
QtObject {
	id: root

	/*! Уникальный идентификатор фабрики внутри компонента. */
	property string factoryId: ""

	/*! Имя интерфейса создаваемых компонентов. */
	property string interfaceName: ""

	/*! Человекочитаемое описание. */
	property string description: ""
}
