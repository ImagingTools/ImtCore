pragma Singleton

import QtQuick 2.0

/*!
	\qmltype AcfRegistry
	\inqmlmodule imtacf
	\brief Центральный реестр и DI-контейнер для QML ACF компонентов.

	AcfRegistry — синглтон, который:
	- Регистрирует AcfComponent по их componentId
	- Разрешает зависимости (dependency injection) между компонентами
	- Загружает конфигурацию из JSON
	- Предоставляет интроспекцию и экспорт графа

	Вся логика реализована на JavaScript — без C++ зависимостей,
	полностью совместима с JQML v2 (web runtime).

	\b Жизненный цикл:
	1. Каждый AcfComponent при создании вызывает AcfRegistry.registerComponent(self)
	2. После регистрации всех компонентов — AcfRegistry.resolveAll()
	3. Реестр обходит все ссылки, ищет целевые компоненты, присваивает target
	4. Компоненты с разрешёнными required-ссылками активируются

	\b Пример:
	\qml
	// В любом месте приложения
	Component.onCompleted: {
		AcfRegistry.loadConfiguration(jsonString);
		AcfRegistry.resolveAll();
		AcfRegistry.dumpComponentIds();
	}
	\endqml
*/
QtObject {
	id: root

	/*! Количество зарегистрированных компонентов. */
	property int componentCount: 0

	/*! Сигнал: компонент зарегистрирован. */
	signal componentRegistered(string componentId)

	/*! Сигнал: компонент удалён из реестра. */
	signal componentUnregistered(string componentId)

	/*! Сигнал: все ссылки разрешены. */
	signal referencesResolved()

	/*! Внутреннее хранилище: componentId → AcfComponent. */
	property var __components: ({})

	// ─── Registration ───

	/*!
		Регистрация компонента.
		\param component — AcfComponent
		\return true если зарегистрирован успешно

		\b Когда вызывать: в Component.onCompleted компонента или
		в AcfCompositeComponent после создания дочерних компонентов.
	*/
	function registerComponent(component) {
		if (!component) {
			console.warn("AcfRegistry.registerComponent: null component");
			return false;
		}
		var id = component.componentId;
		if (!id || id === "") {
			console.warn("AcfRegistry.registerComponent: empty componentId");
			return false;
		}
		if (__components[id]) {
			console.warn("AcfRegistry.registerComponent: already registered:", id);
			return false;
		}
		__components[id] = component;
		componentCount = Object.keys(__components).length;
		componentRegistered(id);
		return true;
	}

	/*!
		Удаление компонента из реестра.
		\param componentId — строковый ID
		\return true если компонент был найден и удалён
	*/
	function unregisterComponent(componentId) {
		if (!__components[componentId]) {
			return false;
		}
		var comp = __components[componentId];
		comp.deactivate();
		delete __components[componentId];
		componentCount = Object.keys(__components).length;
		componentUnregistered(componentId);
		return true;
	}

	// ─── Lookup ───

	/*!
		Получить компонент по componentId.
		\return AcfComponent или null
	*/
	function getComponent(componentId) {
		return __components[componentId] || null;
	}

	/*!
		Найти первый компонент, реализующий данный интерфейс.
		\return AcfComponent или null
	*/
	function getByInterface(interfaceName) {
		var ids = Object.keys(__components);
		for (var i = 0; i < ids.length; ++i) {
			var comp = __components[ids[i]];
			if (comp.implementsInterface(interfaceName)) {
				return comp;
			}
		}
		return null;
	}

	/*!
		Вернуть все компоненты, реализующие данный интерфейс.
		\return массив AcfComponent[]
	*/
	function getAllByInterface(interfaceName) {
		var result = [];
		var ids = Object.keys(__components);
		for (var i = 0; i < ids.length; ++i) {
			var comp = __components[ids[i]];
			if (comp.implementsInterface(interfaceName)) {
				result.push(comp);
			}
		}
		return result;
	}

	/*!
		Вернуть массив всех зарегистрированных componentId.
	*/
	function componentIds() {
		return Object.keys(__components);
	}

	// ─── Dependency Injection ───

	/*!
		Разрешить все ссылки всех зарегистрированных компонентов.
		\return количество неразрешённых ссылок

		\b Логика разрешения:
		Для каждой AcfReference компонента:
		1. Если targetId задан — ищем компонент по ID
		2. Иначе если interfaceName задан — ищем первый компонент с этим интерфейсом
		3. Самоссылки (на себя) игнорируются при поиске по интерфейсу
		4. Если найден — записывается target и targetId

		После разрешения: компоненты, у которых все required-ссылки
		разрешены, активируются (componentCreated).
	*/
	function resolveAll() {
		var unresolvedCount = 0;
		var ids = Object.keys(__components);

		// Шаг 1: разрешить ссылки
		for (var i = 0; i < ids.length; ++i) {
			unresolvedCount += __resolveReferences(__components[ids[i]]);
		}

		// Шаг 2: активировать готовые компоненты
		for (var j = 0; j < ids.length; ++j) {
			var comp = __components[ids[j]];
			if (comp.isActive) continue;

			var allRequired = true;
			for (var k = 0; k < comp.acfReferences.length; ++k) {
				var ref = comp.acfReferences[k];
				if (ref.required && !ref.isResolved()) {
					allRequired = false;
					break;
				}
			}

			if (allRequired) {
				comp.activate();
			}
		}

		referencesResolved();
		return unresolvedCount;
	}

	/*!
		Разрешить ссылки одного компонента.
		\return количество неразрешённых ссылок
	*/
	function resolveComponent(componentId) {
		var comp = getComponent(componentId);
		if (!comp) return -1;

		var unresolvedCount = __resolveReferences(comp);

		var allRequired = true;
		for (var k = 0; k < comp.acfReferences.length; ++k) {
			var ref = comp.acfReferences[k];
			if (ref.required && !ref.isResolved()) {
				allRequired = false;
				break;
			}
		}

		if (allRequired && !comp.isActive) {
			comp.activate();
		}

		return unresolvedCount;
	}

	// ─── Configuration ───

	/*!
		Загрузить конфигурацию из JSON-строки.
		Формат:
		\code
		{
			"elements": [
				{
					"id": "MainDataProvider",
					"attributes": { "ServerUrl": "https://api.example.com" },
					"references": { "AuthProvider": "MainAuthProvider" }
				}
			]
		}
		\endcode

		\b Когда вызывать: после регистрации компонентов, перед resolveAll().
		\return true при успехе
	*/
	function loadConfiguration(jsonString) {
		var config;
		try {
			config = JSON.parse(jsonString);
		} catch (e) {
			console.warn("AcfRegistry.loadConfiguration: JSON parse error:", e);
			return false;
		}

		var elements = config.elements;
		if (!elements || !Array.isArray(elements) || elements.length === 0) {
			console.warn("AcfRegistry.loadConfiguration: no elements found");
			return false;
		}

		for (var i = 0; i < elements.length; ++i) {
			var elem = elements[i];
			var id = elem.id;
			if (!id) continue;

			var comp = getComponent(id);
			if (!comp) {
				console.warn("AcfRegistry.loadConfiguration: component not found:", id);
				continue;
			}

			// Применить атрибуты
			if (elem.attributes) {
				var attrKeys = Object.keys(elem.attributes);
				for (var a = 0; a < attrKeys.length; ++a) {
					var attr = comp.findAttribute(attrKeys[a]);
					if (attr) {
						attr.value = elem.attributes[attrKeys[a]];
					}
				}
			}

			// Применить ссылки (targetId)
			if (elem.references) {
				var refKeys = Object.keys(elem.references);
				for (var r = 0; r < refKeys.length; ++r) {
					var ref = comp.findReference(refKeys[r]);
					if (ref) {
						ref.targetId = elem.references[refKeys[r]];
					}
				}
			}
		}

		return true;
	}

	/*!
		Загрузить конфигурацию и сразу разрешить все ссылки.
	*/
	function loadAndResolve(jsonString) {
		var ok = loadConfiguration(jsonString);
		if (ok) resolveAll();
		return ok;
	}

	// ─── Introspection / Visualization ───

	/*!
		Экспортировать граф компонентов как JS-объект.
		Структура: { nodes: [...], edges: [...] }
		- node: { id, packageId, interfaces, isActive, description, attributes }
		- edge: { from, to, refId/factoryId, interfaceName, type, isResolved }
	*/
	function exportGraph() {
		var nodes = [];
		var edges = [];
		var ids = Object.keys(__components);

		for (var i = 0; i < ids.length; ++i) {
			var comp = __components[ids[i]];

			nodes.push({
				id: comp.componentId,
				packageId: comp.packageId,
				interfaces: comp.interfaces,
				isActive: comp.isActive,
				description: comp.description,
				attributes: comp.attributeValues()
			});

			// Рёбра из ссылок
			for (var r = 0; r < comp.acfReferences.length; ++r) {
				var ref = comp.acfReferences[r];
				if (ref.targetId || ref.isResolved()) {
					edges.push({
						from: comp.componentId,
						to: ref.targetId,
						refId: ref.refId,
						interfaceName: ref.interfaceName,
						type: "reference",
						isResolved: ref.isResolved()
					});
				}
			}

			// Рёбра из фабрик
			for (var f = 0; f < comp.acfFactories.length; ++f) {
				var fact = comp.acfFactories[f];
				edges.push({
					from: comp.componentId,
					to: "",
					factoryId: fact.factoryId,
					interfaceName: fact.interfaceName,
					type: "factory"
				});
			}
		}

		return { nodes: nodes, edges: edges };
	}

	/*!
		Вывести в консоль список всех компонентов (для отладки).
	*/
	function dumpComponentIds() {
		var ids = Object.keys(__components);
		console.log("=== AcfRegistry: " + ids.length + " components ===");
		for (var i = 0; i < ids.length; ++i) {
			var comp = __components[ids[i]];
			var ifaces = comp.interfaces ? comp.interfaces.join(", ") : "";
			console.log("  [" + ids[i] + "] pkg=" + comp.packageId + " ifaces=[" + ifaces + "] active=" + comp.isActive);
		}
		console.log("=== End AcfRegistry ===");
	}

	// ─── Private ───

	function __resolveReferences(component) {
		var unresolvedCount = 0;

		for (var i = 0; i < component.acfReferences.length; ++i) {
			var ref = component.acfReferences[i];
			if (ref.isResolved()) continue;

			// По явному targetId
			if (ref.targetId && ref.targetId !== "") {
				var target = getComponent(ref.targetId);
				if (target) {
					ref.target = target;
					continue;
				}
			}

			// По interfaceName
			if (ref.interfaceName && ref.interfaceName !== "") {
				var found = null;
				var ids = Object.keys(__components);
				for (var j = 0; j < ids.length; ++j) {
					var cand = __components[ids[j]];
					if (cand !== component && cand.implementsInterface(ref.interfaceName)) {
						found = cand;
						break;
					}
				}
				if (found) {
					ref.target = found;
					ref.targetId = found.componentId;
					continue;
				} else if (__components[component.componentId] && component.implementsInterface(ref.interfaceName)) {
					console.warn("AcfRegistry: skipping self-reference for",
						component.componentId, "ref", ref.refId);
				}
			}

			++unresolvedCount;
		}

		return unresolvedCount;
	}
}
