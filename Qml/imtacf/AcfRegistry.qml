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

	// ─── ACC/ACCL Export/Import (Compositor Integration) ───

	/*!
		Маппинг JS-типов на ACC-типы (Compositor).
	*/
	function __jsTypeToAccType(jsType) {
		var map = {
			"string": "String",
			"text": "Text",
			"int": "Integer",
			"integer": "Integer",
			"bool": "Boolean",
			"boolean": "Boolean",
			"real": "Real",
			"id": "Id",
			"reference": "Reference",
			"factory": "Factory"
		};
		return map[jsType.toLowerCase()] || "String";
	}

	function __accTypeToJsType(accType) {
		var map = {
			"String": "string",
			"Text": "string",
			"Integer": "int",
			"Boolean": "bool",
			"Real": "real",
			"Id": "string",
			"Reference": "reference",
			"Factory": "factory"
		};
		return map[accType] || "var";
	}

	function __escapeXml(str) {
		if (!str) return "";
		return String(str)
			.replace(/&/g, "&amp;")
			.replace(/</g, "&lt;")
			.replace(/>/g, "&gt;")
			.replace(/"/g, "&quot;")
			.replace(/'/g, "&apos;");
	}

	/*!
		Экспортировать реестр в ACC XML (формат Compositor / AcfSln).

		Формат полностью совместим с *.acc файлами, которые
		открываются в Compositor.

		\param registryDescription — описание реестра
		\param registryKeywords — ключевые слова
		\return строка XML в формате ACC
	*/
	function exportToAcc(registryDescription, registryKeywords) {
		var xml = '<?xml version="1.0" encoding="UTF-8"?>\n';
		xml += '<Acf>\n';

		// Header
		xml += '    <AcfHeader>\n';
		xml += '        <VersionInfos>\n';
		xml += '            <Version Id="0" Number="1" Description="QML-ACF"/>\n';
		xml += '        </VersionInfos>\n';
		xml += '    </AcfHeader>\n';

		// Description & Keywords
		xml += '    <Description>' + __escapeXml(registryDescription || "") + '</Description>\n';
		xml += '    <Keywords>' + __escapeXml(registryKeywords || "") + '</Keywords>\n';

		// Elements
		xml += '    <ElementsList>\n';

		var ids = Object.keys(__components);
		for (var i = 0; i < ids.length; ++i) {
			var comp = __components[ids[i]];
			xml += __exportElementToAcc(comp);
		}

		xml += '    </ElementsList>\n';

		// Embedded registries (not used in QML ACF currently)
		xml += '    <EmbeddedRegistriesList/>\n';

		// Exported interfaces
		xml += '    <ExportedInterfaces>\n';
		for (var j = 0; j < ids.length; ++j) {
			var comp2 = __components[ids[j]];
			var ifaces = comp2.interfaces || [];
			for (var k = 0; k < ifaces.length; ++k) {
				xml += '        <Interface InterfaceId="' + __escapeXml(ifaces[k]);
				xml += '" ComponentId="' + __escapeXml(comp2.componentId) + '"/>\n';
			}
		}
		xml += '    </ExportedInterfaces>\n';

		// Exported components
		xml += '    <ExportedComponents>\n';
		for (var m = 0; m < ids.length; ++m) {
			var comp3 = __components[ids[m]];
			xml += '        <Component ExportedId="' + __escapeXml(comp3.componentId);
			xml += '" ComponentId="' + __escapeXml(comp3.componentId) + '"/>\n';
		}
		xml += '    </ExportedComponents>\n';

		xml += '</Acf>\n';
		return xml;
	}

	function __exportElementToAcc(comp) {
		var xml = '';
		xml += '        <Element Id="' + __escapeXml(comp.componentId);
		xml += '" PackageId="' + __escapeXml(comp.packageId);
		xml += '" ComponentId="' + __escapeXml(comp.componentId) + '">\n';
		xml += '            <Data IsEnabled="true" Flags="0">\n';
		xml += '                <AttributeInfoMap>\n';

		// Attributes
		for (var a = 0; a < comp.acfAttributes.length; ++a) {
			var attr = comp.acfAttributes[a];
			xml += __exportAttributeToAcc(attr);
		}

		// References (appear as Reference-type attributes in ACC)
		for (var r = 0; r < comp.acfReferences.length; ++r) {
			var ref = comp.acfReferences[r];
			xml += '                    <AttributeInfo Id="' + __escapeXml(ref.refId);
			xml += '" Type="Reference"';
			xml += ' ExportId="' + __escapeXml(ref.exportId || "") + '">\n';

			if (ref.targetId && ref.targetId !== "") {
				xml += '                        <Data IsEnabled="true" Value="' + __escapeXml(ref.targetId) + '"/>\n';
			} else {
				xml += '                        <Data IsEnabled="false"/>\n';
			}

			xml += '                    </AttributeInfo>\n';
		}

		// Factories (appear as Factory-type attributes in ACC)
		for (var f = 0; f < comp.acfFactories.length; ++f) {
			var fact = comp.acfFactories[f];
			xml += '                    <AttributeInfo Id="' + __escapeXml(fact.factoryId);
			xml += '" Type="Factory"';
			xml += ' ExportId="' + __escapeXml(fact.exportId || "") + '">\n';
			xml += '                        <Data IsEnabled="false"/>\n';
			xml += '                    </AttributeInfo>\n';
		}

		xml += '                </AttributeInfoMap>\n';
		xml += '            </Data>\n';
		xml += '        </Element>\n';
		return xml;
	}

	function __exportAttributeToAcc(attr) {
		var xml = '';
		var accType = __jsTypeToAccType(attr.type);
		xml += '                    <AttributeInfo Id="' + __escapeXml(attr.attrId);
		xml += '" Type="' + accType;
		xml += '" ExportId="' + __escapeXml(attr.exportId || "") + '">\n';

		var val = attr.resolvedValue();
		if (val !== undefined && val !== null) {
			xml += '                        <Data IsEnabled="true" Value="' + __escapeXml(String(val)) + '"/>\n';
		} else {
			xml += '                        <Data IsEnabled="false"/>\n';
		}

		xml += '                    </AttributeInfo>\n';
		return xml;
	}

	/*!
		Экспортировать визуальный макет (позиции) в ACCL XML (для Compositor).

		\return строка XML в формате ACCL
	*/
	function exportToAccl() {
		var xml = '<?xml version="1.0" encoding="UTF-8"?>\n';
		xml += '<Acf>\n';

		// Header
		xml += '    <AcfHeader>\n';
		xml += '        <VersionInfos>\n';
		xml += '            <Version Id="0" Number="1" Description="QML-ACF"/>\n';
		xml += '        </VersionInfos>\n';
		xml += '    </AcfHeader>\n';

		// PositionMap
		xml += '    <PositionMap>\n';

		var ids = Object.keys(__components);
		for (var i = 0; i < ids.length; ++i) {
			var comp = __components[ids[i]];
			var pos = comp.layoutPosition || { x: 0, y: 0, note: "" };
			xml += '        <Element ComponentName="' + __escapeXml(comp.componentId);
			xml += '" X="' + Math.round(pos.x || 0);
			xml += '" Y="' + Math.round(pos.y || 0);
			xml += '" Note="' + __escapeXml(pos.note || "") + '"/>\n';
		}

		xml += '    </PositionMap>\n';
		xml += '    <EmbeddedRegistries/>\n';
		xml += '</Acf>\n';

		return xml;
	}

	/*!
		Импорт конфигурации из ACC XML (формат Compositor / AcfSln).

		Загружает элементы из ACC XML и применяет значения атрибутов и ссылок
		к уже зарегистрированным компонентам (аналогично loadConfiguration для JSON).

		\param accXmlString — строка с XML в формате ACC
		\return true при успехе
	*/
	function importFromAcc(accXmlString) {
		// Простой XML-парсер на JS (без DOM — совместимо с JQML v2)
		var elements = __parseAccElements(accXmlString);
		if (!elements || elements.length === 0) {
			console.warn("AcfRegistry.importFromAcc: no elements found");
			return false;
		}

		for (var i = 0; i < elements.length; ++i) {
			var elem = elements[i];
			if (!elem.id) continue;

			var comp = getComponent(elem.id);
			if (!comp) {
				console.warn("AcfRegistry.importFromAcc: component not found:", elem.id);
				continue;
			}

			// Применить packageId если задан
			if (elem.packageId && comp.packageId === "") {
				comp.packageId = elem.packageId;
			}

			// Применить атрибуты
			var attrKeys = Object.keys(elem.attributes);
			for (var a = 0; a < attrKeys.length; ++a) {
				var attrInfo = elem.attributes[attrKeys[a]];

				if (attrInfo.type === "Reference") {
					// Reference-type → это ссылка
					var ref = comp.findReference(attrKeys[a]);
					if (ref && attrInfo.value) {
						ref.targetId = attrInfo.value;
					}
				} else if (attrInfo.type === "Factory") {
					// Factory-type → пропускаем (informational)
				} else {
					// Обычный атрибут
					var attr = comp.findAttribute(attrKeys[a]);
					if (attr && attrInfo.isEnabled && attrInfo.value !== undefined) {
						attr.value = __convertAccValue(attrInfo.value, attrInfo.type);
					}
				}
			}
		}

		return true;
	}

	function __convertAccValue(value, accType) {
		switch (accType) {
			case "Integer": return parseInt(value, 10);
			case "Real": return parseFloat(value);
			case "Boolean": return value === "true";
			default: return value;
		}
	}

	/*!
		Импорт позиций из ACCL XML (формат Compositor).

		\param acclXmlString — строка с XML в формате ACCL
		\return true при успехе
	*/
	function importFromAccl(acclXmlString) {
		var positions = __parseAcclPositions(acclXmlString);
		if (!positions || positions.length === 0) {
			console.warn("AcfRegistry.importFromAccl: no positions found");
			return false;
		}

		for (var i = 0; i < positions.length; ++i) {
			var pos = positions[i];
			if (!pos.name) continue;

			var comp = getComponent(pos.name);
			if (comp) {
				comp.layoutPosition = { x: pos.x, y: pos.y, note: pos.note };
			}
		}

		return true;
	}

	/*!
		Загрузить ACC + ACCL и разрешить все ссылки.
		\param accXml — ACC XML строка
		\param acclXml — ACCL XML строка (опционально)
	*/
	function importAccAndResolve(accXml, acclXml) {
		var ok = importFromAcc(accXml);
		if (acclXml) importFromAccl(acclXml);
		if (ok) resolveAll();
		return ok;
	}

	// ─── Simple XML Parsers (no DOM dependency, JQML v2 compatible) ───

	function __parseAccElements(xmlStr) {
		var elements = [];
		var elemRegex = /<Element\s+Id="([^"]*)"(?:\s+PackageId="([^"]*)")?(?:\s+ComponentId="([^"]*)")?[^>]*>([\s\S]*?)<\/Element>/g;
		var match;

		while ((match = elemRegex.exec(xmlStr)) !== null) {
			var elem = {
				id: match[1],
				packageId: match[2] || "",
				componentId: match[3] || "",
				attributes: {}
			};

			var attrBlock = match[4];
			var attrRegex = /<AttributeInfo\s+Id="([^"]*)"(?:\s+Type="([^"]*)")?(?:\s+ExportId="([^"]*)")?[^>]*>([\s\S]*?)<\/AttributeInfo>/g;
			var attrMatch;

			while ((attrMatch = attrRegex.exec(attrBlock)) !== null) {
				var attrId = attrMatch[1];
				var attrType = attrMatch[2] || "String";
				var exportId = attrMatch[3] || "";
				var dataBlock = attrMatch[4];

				var dataMatch = /<Data\s+IsEnabled="([^"]*)"(?:\s+Value="([^"]*)")?[^>]*>/.exec(dataBlock);
				var isEnabled = dataMatch ? (dataMatch[1] === "true") : false;
				var value = dataMatch ? dataMatch[2] : undefined;

				// Check for Values list (arrays: Reference[], Id[], etc.)
				var values = [];
				var valRegex = /<Value>([^<]*)<\/Value>/g;
				var valMatch;
				while ((valMatch = valRegex.exec(dataBlock)) !== null) {
					values.push(valMatch[1]);
				}

				elem.attributes[attrId] = {
					type: attrType,
					exportId: exportId,
					isEnabled: isEnabled,
					value: values.length > 0 ? values : value
				};
			}

			elements.push(elem);
		}

		return elements;
	}

	function __parseAcclPositions(xmlStr) {
		var positions = [];
		var posRegex = /<Element\s+ComponentName="([^"]*)"(?:\s+X="([^"]*)")?(?:\s+Y="([^"]*)")?(?:\s+Note="([^"]*)")?/g;
		var match;

		while ((match = posRegex.exec(xmlStr)) !== null) {
			positions.push({
				name: match[1],
				x: parseInt(match[2] || "0", 10),
				y: parseInt(match[3] || "0", 10),
				note: match[4] || ""
			});
		}

		return positions;
	}
}
