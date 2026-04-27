import QtQuick 2.0
import imtacf 1.0

/*!
	\qmltype Example_AcfUsage
	\inqmlmodule imtacf
	\brief Полный пример использования QML ACF Framework.

	Этот файл демонстрирует все основные сценарии:
	1. Создание компонентов
	2. Задание атрибутов
	3. Dependency injection через ссылки
	4. Работа с конфигурацией из JSON
	5. Композитные компоненты
	6. Интроспекция и визуализация

	========================================================================
	ОГЛАВЛЕНИЕ:
	  Шаг 1. Создание отдельных компонентов
	  Шаг 2. Задание атрибутов и ссылок
	  Шаг 3. Регистрация и разрешение зависимостей
	  Шаг 4. Конфигурация из JSON
	  Шаг 5. Композитный компонент (AcfCompositeComponent)
	  Шаг 6. Работа с разрешёнными ссылками
	  Шаг 7. Визуализация графа
	========================================================================
*/
Item {
	id: exampleRoot
	width: 800
	height: 600

	// ═══════════════════════════════════════════════════════
	// ШАГ 1. СОЗДАНИЕ КОМПОНЕНТОВ
	// ═══════════════════════════════════════════════════════
	//
	// Компонент создаётся декларативно как QML-объект.
	// Момент создания = Component.onCompleted QML-движка.
	// Никакого C++ — всё чистый QML + JavaScript.
	//
	// Каждый компонент — невизуальный QtObject с метаданными.

	// --- Компонент 1: Сервис аутентификации ---
	AcfComponent {
		id: authProvider

		// Уникальный ID этого экземпляра в реестре
		componentId: "MainAuthProvider"

		// Пакет, к которому принадлежит
		packageId: "imtauthgui"

		// Какие «интерфейсы» реализует (массив строк)
		interfaces: ["IAuthProvider"]

		description: "Сервис аутентификации"

		// Атрибуты — аналог I_ATTR / I_ASSIGN
		acfAttributes: [
			AcfAttribute {
				attrId: "AuthServerUrl"
				type: "string"
				required: true
				defaultValue: "https://auth.example.com"
				description: "URL сервера аутентификации"
			},
			AcfAttribute {
				attrId: "TokenLifetime"
				type: "int"
				required: false
				defaultValue: 3600
				description: "Время жизни токена в секундах"
			}
		]

		// У AuthProvider нет внешних зависимостей
		acfReferences: []

		// Реакция на активацию
		onComponentCreated: {
			console.log("[AuthProvider] Активирован! URL:", attr("AuthServerUrl"));
		}
	}

	// --- Компонент 2: Логгер ---
	AcfComponent {
		id: appLogger

		componentId: "AppLogger"
		packageId: "imtcore"
		interfaces: ["ILogger"]
		description: "Системный логгер"

		acfAttributes: [
			AcfAttribute { attrId: "LogLevel"; type: "string"; defaultValue: "info" }
		]
	}

	// --- Компонент 3: Провайдер данных ---
	AcfComponent {
		id: dataProvider

		componentId: "MainDataProvider"
		packageId: "imtguigql"
		interfaces: ["IDataProvider", "IFilterable"]
		description: "GraphQL data provider"

		// Атрибуты
		acfAttributes: [
			AcfAttribute {
				attrId: "ServerUrl"
				type: "string"
				required: true
				defaultValue: "http://localhost:4000/graphql"
			},
			AcfAttribute {
				attrId: "PageSize"
				type: "int"
				defaultValue: 20
			}
		]

		// Зависимости (ссылки) — аналог I_REF
		// refId — имя слота, interfaceName — какой интерфейс нужен
		acfReferences: [
			AcfReference {
				refId: "AuthProvider"
				interfaceName: "IAuthProvider"
				required: true  // обязательная зависимость
			},
			AcfReference {
				refId: "Logger"
				interfaceName: "ILogger"
				required: false // необязательная
			}
		]

		// Фабрики — аналог I_FACT
		acfFactories: [
			AcfFactory {
				factoryId: "ItemFactory"
				interfaceName: "IListItem"
			}
		]

		onComponentCreated: {
			console.log("[DataProvider] Активирован!");
			console.log("  ServerUrl:", attr("ServerUrl"));
			console.log("  PageSize:", attr("PageSize"));

			// ═══════════════════════════════════════════════════════
			// ШАГ 6. РАБОТА С РАЗРЕШЁННЫМИ ССЫЛКАМИ
			// ═══════════════════════════════════════════════════════
			//
			// После resolveAll() у каждой AcfReference заполнен target.
			// Доступ через ref("refId") — возвращает AcfComponent или null.

			var auth = ref("AuthProvider");
			if (auth) {
				console.log("  AuthProvider URL:", auth.attr("AuthServerUrl"));
				console.log("  AuthProvider active:", auth.isActive);
			}

			var logger = ref("Logger");
			if (logger) {
				console.log("  Logger level:", logger.attr("LogLevel"));
			} else {
				console.log("  Logger не подключён (optional)");
			}
		}
	}

	// ═══════════════════════════════════════════════════════
	// ШАГ 3. РЕГИСТРАЦИЯ И РАЗРЕШЕНИЕ ЗАВИСИМОСТЕЙ
	// ═══════════════════════════════════════════════════════
	//
	// Порядок:
	// 1. register — добавляет компонент в реестр по componentId
	// 2. loadConfiguration — (опционально) перезаписывает атрибуты/ссылки из JSON
	// 3. resolveAll — обходит все ссылки, находит целевые компоненты
	// 4. activate — компоненты с разрешёнными required-ссылками получают componentCreated

	Component.onCompleted: {
		// 1. Регистрируем компоненты
		AcfRegistry.registerComponent(authProvider);
		AcfRegistry.registerComponent(appLogger);
		AcfRegistry.registerComponent(dataProvider);

		// ═══════════════════════════════════════════════════════
		// ШАГ 4. КОНФИГУРАЦИЯ ИЗ JSON (опционально)
		// ═══════════════════════════════════════════════════════
		//
		// JSON перезаписывает значения атрибутов и targetId ссылок.
		// Формат идентичен .acc XML но в JSON:
		var config = JSON.stringify({
			elements: [
				{
					id: "MainDataProvider",
					attributes: {
						ServerUrl: "https://api.production.com/graphql",
						PageSize: 50
					},
					references: {
						AuthProvider: "MainAuthProvider",
						Logger: "AppLogger"
					}
				}
			]
		});

		AcfRegistry.loadConfiguration(config);

		// 3. Разрешаем все зависимости
		var unresolved = AcfRegistry.resolveAll();
		console.log("Неразрешённых ссылок:", unresolved);

		// 4. Дамп для отладки
		AcfRegistry.dumpComponentIds();

		// 5. Интроспекция
		var graph = AcfRegistry.exportGraph();
		console.log("Граф:", JSON.stringify(graph, null, 2));
	}

	// ═══════════════════════════════════════════════════════
	// ШАГ 5. КОМПОЗИТНЫЙ КОМПОНЕНТ
	// ═══════════════════════════════════════════════════════
	//
	// AcfCompositeComponent автоматически:
	// - Регистрирует себя и children в AcfRegistry
	// - Применяет AcfConnection (явные связи)
	// - Вызывает resolveAll()
	//
	// Внимание: compositeWorkspace создаётся ПОСЛЕ Component.onCompleted
	// родителя, поэтому к этому моменту AuthProvider уже в реестре.

	AcfCompositeComponent {
		id: compositeWorkspace

		componentId: "TicketWorkspace"
		packageId: "imtdeskgui"
		interfaces: ["IWorkspace"]
		description: "Рабочее пространство тикетов"

		children: [
			AcfComponent {
				componentId: "TicketList"
				packageId: "imtdeskgui"
				interfaces: ["ISelectable", "ITicketList"]
				description: "Список тикетов"

				acfReferences: [
					AcfReference {
						refId: "DataProvider"
						interfaceName: "IDataProvider"
						required: true
					}
				]

				onComponentCreated: {
					console.log("[TicketList] Активирован!");
					var dp = ref("DataProvider");
					if (dp) {
						console.log("  DataProvider URL:", dp.attr("ServerUrl"));
					}
				}
			},

			AcfComponent {
				componentId: "TicketEditor"
				packageId: "imtdeskgui"
				interfaces: ["IEditor"]
				description: "Редактор тикета"

				acfReferences: [
					AcfReference {
						refId: "TicketList"
						interfaceName: "ISelectable"
						required: true
					}
				]

				onComponentCreated: {
					console.log("[TicketEditor] Активирован!");
					var list = ref("TicketList");
					if (list) {
						console.log("  Подключён к:", list.componentId);
					}
				}
			}
		]

		// Явная «проводка» — TicketEditor.TicketList → TicketList
		acfConnections: [
			AcfConnection {
				from: "TicketEditor"
				refId: "TicketList"
				to: "TicketList"
			}
		]
	}

	// ═══════════════════════════════════════════════════════
	// ШАГ 7. ВИЗУАЛИЗАЦИЯ ГРАФА
	// ═══════════════════════════════════════════════════════
	//
	// ComponentGraphView рисует все узлы и рёбра на Canvas.
	// Обновляется автоматически при изменении реестра.

	ComponentGraphView {
		anchors.fill: parent
	}

	// ═══════════════════════════════════════════════════════
	// ШАГ 8. ИНТЕГРАЦИЯ С COMPOSITOR (AcfSln)
	// ═══════════════════════════════════════════════════════
	//
	// Compositor (AcfSln) — визуальный редактор ACF-конфигураций.
	// Он работает с XML-форматами:
	//   *.acc  — конфигурация компонентов (элементы, атрибуты, ссылки)
	//   *.accl — визуальная раскладка (позиции компонентов на диаграмме)
	//
	// Наш QML ACF Framework может:
	// 1. Экспортировать текущий реестр в ACC XML → открыть в Compositor
	// 2. Импортировать ACC XML → настроить QML-компоненты из Compositor
	// 3. Экспортировать/импортировать ACCL позиции для визуализации
	//
	// Пример экспорта:
	//
	//   var accXml = AcfRegistry.exportToAcc("My Application", "QML WebApp");
	//   console.log(accXml);
	//   // → <?xml version="1.0" encoding="UTF-8"?>
	//   // → <Acf>
	//   // →   <ElementsList>
	//   // →     <Element Id="MainAuthProvider" PackageId="imtauthgui" ...>
	//   // →       <Data IsEnabled="true">
	//   // →         <AttributeInfoMap>
	//   // →           <AttributeInfo Id="AuthServerUrl" Type="String" ...>
	//   // →   ...
	//
	// Пример импорта из Compositor:
	//
	//   var accFromCompositor = loadFile("MyApp.acc");
	//   AcfRegistry.importFromAcc(accFromCompositor);
	//   AcfRegistry.resolveAll();
	//
	// Пример полного цикла:
	//
	//   // 1. Зарегистрировать компоненты
	//   AcfRegistry.registerComponent(myComponent);
	//   // 2. Загрузить конфигурацию из Compositor
	//   AcfRegistry.importAccAndResolve(accXml, acclXml);
	//   // 3. Экспортировать обратно
	//   var exportedAcc  = AcfRegistry.exportToAcc("MyApp", "");
	//   var exportedAccl = AcfRegistry.exportToAccl();

	// Пример программного экспорта:
	Timer {
		interval: 500
		running: true
		repeat: false
		onTriggered: {
			console.log("=== ACC EXPORT ===");
			console.log(AcfRegistry.exportToAcc("Example Application", "QML ACF Demo"));
			console.log("=== ACCL EXPORT ===");
			console.log(AcfRegistry.exportToAccl());
		}
	}
}
