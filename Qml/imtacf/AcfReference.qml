import QtQuick 2.0

/*!
	\qmltype AcfReference
	\inqmlmodule imtacf
	\brief Описание зависимости (ссылки) компонента (аналог I_REF в ACF).

	AcfReference декларирует, что компонент-владелец нуждается в ссылке на
	другой компонент — либо по конкретному targetId, либо по interfaceName.

	\b Разрешение зависимости:
	1. Реестр ищет компонент по targetId (если задан).
	2. Иначе — первый компонент, реализующий interfaceName.
	3. Результат записывается в target.

	\b Жизненный цикл:
	- До resolveAll() — target === null, isResolved() === false
	- После resolveAll() — target указывает на найденный AcfComponent

	\qml
	AcfComponent {
		AcfReference { refId: "AuthProvider"; interfaceName: "IAuthProvider"; required: true }
		AcfReference { refId: "Logger"; interfaceName: "ILogger"; required: false }
	}
	\endqml
*/
QtObject {
	id: root

	/*! Уникальный идентификатор ссылки внутри компонента. */
	property string refId: ""

	/*! Имя интерфейса для поиска по интерфейсу. */
	property string interfaceName: ""

	/*! Если true — зависимость обязательна. */
	property bool required: false

	/*! Явный ID целевого компонента (прямое связывание). */
	property string targetId: ""

	/*! Разрешённая ссылка на целевой AcfComponent (заполняется реестром). */
	property var target: null

	/*! Человекочитаемое описание. */
	property string description: ""

	/*! ExportId для Compositor (AcfSln). Если пустой — ссылка не экспортируется. */
	property string exportId: ""

	/*!
		Возвращает true, если ссылка разрешена (target !== null).
	*/
	function isResolved() {
		return target !== null;
	}
}
