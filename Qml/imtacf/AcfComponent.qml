import QtQuick 2.0

/*!
	\qmltype AcfComponent
	\inqmlmodule imtacf
	\brief Базовый QML-компонент с метаданными (аналог CComponentBase / I_BEGIN_COMPONENT в ACF).

	AcfComponent — невизуальный компонент, который:
	- Хранит метаданные: componentId, packageId, interfaces[]
	- Управляет дочерними дескрипторами: AcfAttribute, AcfReference, AcfFactory
	- Предоставляет API для интроспекции
	- Управляет жизненным циклом: componentCreated / componentDestroyed

	\b Жизненный цикл:
	1. Компонент создаётся декларативно в QML
	2. В Component.onCompleted заполняются внутренние списки
	3. Вызов AcfRegistry.registerComponent(comp) регистрирует его
	4. AcfRegistry.resolveAll() разрешает ссылки (references)
	5. Если все required ссылки разрешены — componentCreated()
	6. При удалении — componentDestroyed()

	\qml
	AcfComponent {
		componentId: "MyDataProvider"
		packageId: "imtguigql"
		interfaces: ["IDataProvider", "IFilterable"]

		acfAttributes: [
			AcfAttribute { attrId: "ServerUrl"; type: "string"; required: true; defaultValue: "http://localhost" },
			AcfAttribute { attrId: "PageSize"; type: "int"; defaultValue: 20 }
		]

		acfReferences: [
			AcfReference { refId: "AuthProvider"; interfaceName: "IAuthProvider"; required: true }
		]

		acfFactories: [
			AcfFactory { factoryId: "ItemFactory"; interfaceName: "IListItem" }
		]
	}
	\endqml
*/
QtObject {
	id: root

	/*! Уникальный идентификатор компонента. */
	property string componentId: ""

	/*! Идентификатор пакета, к которому принадлежит компонент. */
	property string packageId: ""

	/*! Список реализуемых интерфейсов. */
	property var interfaces: []

	/*! Описание компонента. */
	property string description: ""

	/*! Ключевые слова (для ACC/Compositor). */
	property string keywords: ""

	/*! Компонент активирован (все required-ссылки разрешены). */
	property bool isActive: false

	/*! Позиция в визуальном макете Compositor (AcfSln). { x, y, note } */
	property var layoutPosition: ({ "x": 0, "y": 0, "note": "" })

	/*! Список атрибутов. Каждый элемент — AcfAttribute. */
	property var acfAttributes: []

	/*! Список ссылок. Каждый элемент — AcfReference. */
	property var acfReferences: []

	/*! Список фабрик. Каждый элемент — AcfFactory. */
	property var acfFactories: []

	/*!
		Сигнал: компонент активирован (все обязательные ссылки разрешены).
	*/
	signal componentCreated()

	/*!
		Сигнал: компонент деактивирован (удалён из реестра).
	*/
	signal componentDestroyed()

	// ─── Introspection API ───

	/*!
		Возвращает карту attrId → resolvedValue для всех атрибутов.
	*/
	function attributeValues() {
		var result = {};
		for (var i = 0; i < acfAttributes.length; ++i) {
			var a = acfAttributes[i];
			result[a.attrId] = a.resolvedValue();
		}
		return result;
	}

	/*!
		Возвращает массив описаний ссылок:
		[{ refId, interfaceName, targetId, isResolved }, ...].
	*/
	function referenceInfo() {
		var result = [];
		for (var i = 0; i < acfReferences.length; ++i) {
			var r = acfReferences[i];
			result.push({
				refId: r.refId,
				interfaceName: r.interfaceName,
				targetId: r.targetId,
				isResolved: r.isResolved()
			});
		}
		return result;
	}

	/*!
		Возвращает массив описаний фабрик:
		[{ factoryId, interfaceName }, ...].
	*/
	function factoryInfo() {
		var result = [];
		for (var i = 0; i < acfFactories.length; ++i) {
			var f = acfFactories[i];
			result.push({
				factoryId: f.factoryId,
				interfaceName: f.interfaceName
			});
		}
		return result;
	}

	/*!
		Найти атрибут по attrId. Возвращает AcfAttribute или null.
	*/
	function findAttribute(attrId) {
		for (var i = 0; i < acfAttributes.length; ++i) {
			if (acfAttributes[i].attrId === attrId) return acfAttributes[i];
		}
		return null;
	}

	/*!
		Найти ссылку по refId. Возвращает AcfReference или null.
	*/
	function findReference(refId) {
		for (var i = 0; i < acfReferences.length; ++i) {
			if (acfReferences[i].refId === refId) return acfReferences[i];
		}
		return null;
	}

	/*!
		Найти фабрику по factoryId. Возвращает AcfFactory или null.
	*/
	function findFactory(factoryId) {
		for (var i = 0; i < acfFactories.length; ++i) {
			if (acfFactories[i].factoryId === factoryId) return acfFactories[i];
		}
		return null;
	}

	/*!
		Возвращает true если компонент реализует данный интерфейс.
	*/
	function implementsInterface(name) {
		return interfaces.indexOf(name) >= 0;
	}

	// ─── Lifecycle (called by registry) ───

	/*!
		Активировать компонент. Вызывается реестром.
	*/
	function activate() {
		if (!isActive) {
			isActive = true;
			componentCreated();
		}
	}

	/*!
		Деактивировать компонент. Вызывается реестром.
	*/
	function deactivate() {
		if (isActive) {
			componentDestroyed();
			isActive = false;
		}
	}

	// ─── Access resolved references ───

	/*!
		Получить разрешённый target по refId.
		Возвращает AcfComponent (target) или null.

		Используется для работы с связанными компонентами:
		\qml
		var auth = myComponent.ref("AuthProvider");
		if (auth) {
			console.log("Auth server:", auth.attr("ServerUrl"));
		}
		\endqml
	*/
	function ref(refId) {
		var r = findReference(refId);
		return r ? r.target : null;
	}

	/*!
		Получить resolvedValue атрибута по attrId.
		\qml
		var url = myComponent.attr("ServerUrl"); // "https://api.example.com"
		\endqml
	*/
	function attr(attrId) {
		var a = findAttribute(attrId);
		return a ? a.resolvedValue() : undefined;
	}
}
