import QtQuick 2.0

/*!
	\qmltype AcfAttribute
	\inqmlmodule imtacf
	\brief Описание атрибута компонента (аналог I_ATTR / I_ASSIGN в ACF).

	AcfAttribute хранит метаданные одного свойства компонента:
	- \c attrId — уникальный идентификатор (строка)
	- \c type — тип значения ("string", "int", "bool", "real", "var")
	- \c required — обязательность
	- \c defaultValue — значение по умолчанию
	- \c value — текущее значение (если задано извне через конфигурацию)
	- \c description — человекочитаемое описание

	\b Жизненный цикл:
	При создании value не задано. Реестр (AcfRegistry) может записать
	value из JSON-конфигурации. resolvedValue() возвращает value, если
	оно задано, иначе — defaultValue.

	\qml
	AcfComponent {
		AcfAttribute { attrId: "ServerUrl"; type: "string"; required: true; defaultValue: "http://localhost" }
		AcfAttribute { attrId: "PageSize"; type: "int"; defaultValue: 20 }
	}
	\endqml
*/
QtObject {
	id: root

	/*! Уникальный идентификатор атрибута внутри компонента. */
	property string attrId: ""

	/*! Тип значения: "string", "int", "bool", "real", "var". */
	property string type: "var"

	/*! Если true — атрибут обязателен. */
	property bool required: false

	/*! Значение по умолчанию. */
	property var defaultValue: undefined

	/*! Текущее значение (устанавливается реестром или вручную). */
	property var value: undefined

	/*! Человекочитаемое описание атрибута. */
	property string description: ""

	/*! ExportId для Compositor (AcfSln). Если пустой — атрибут не экспортируется. */
	property string exportId: ""

	/*!
		Возвращает эффективное значение: value, если задано, иначе defaultValue.
	*/
	function resolvedValue() {
		if (value !== undefined) {
			return value;
		}
		return defaultValue;
	}
}
