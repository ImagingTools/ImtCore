import QtQuick 2.0

/*!
	\qmltype AcfCompositeComponent
	\inqmlmodule imtacf
	\brief Композитный компонент, группирующий дочерние AcfComponent (аналог CCompositeComponent).

	AcfCompositeComponent:
	- Регистрирует себя и все дочерние компоненты в AcfRegistry
	- Применяет явные соединения (AcfConnection)
	- Вызывает resolveAll() для всех

	\b Жизненный цикл:
	1. Component.onCompleted: регистрирует себя в AcfRegistry
	2. Регистрирует каждый дочерний AcfComponent из children[]
	3. Применяет acfConnections: для каждого AcfConnection находит
	   ссылку from.refId и записывает targetId = to
	4. Вызывает AcfRegistry.resolveAll()
	5. При удалении — удаляет всех детей и себя из реестра

	\qml
	AcfCompositeComponent {
		componentId: "TicketWorkspace"
		packageId: "imtdeskgui"
		interfaces: ["IWorkspace"]

		children: [
			AcfComponent {
				componentId: "TicketList"
				interfaces: ["ISelectable"]
				acfReferences: [
					AcfReference { refId: "DataProvider"; interfaceName: "IDataProvider" }
				]
			},
			AcfComponent {
				componentId: "TicketEditor"
				acfReferences: [
					AcfReference { refId: "List"; interfaceName: "ISelectable" }
				]
			}
		]

		acfConnections: [
			AcfConnection { from: "TicketEditor"; refId: "List"; to: "TicketList" }
		]
	}
	\endqml
*/
AcfComponent {
	id: root

	/*! Дочерние AcfComponent. */
	property var children: []

	/*! Явные соединения между дочерними компонентами. */
	property var acfConnections: []

	Component.onCompleted: {
		// 1. Регистрируем себя
		if (root.componentId !== "") {
			AcfRegistry.registerComponent(root);
		}

		// 2. Регистрируем дочерние компоненты
		for (var i = 0; i < children.length; ++i) {
			var child = children[i];
			if (child.componentId && child.componentId !== "") {
				AcfRegistry.registerComponent(child);
			}
		}

		// 3. Применяем явные соединения
		for (var j = 0; j < acfConnections.length; ++j) {
			var conn = acfConnections[j];
			if (conn && conn.from && conn.refId && conn.to) {
				var sourceComp = AcfRegistry.getComponent(conn.from);
				if (sourceComp) {
					var ref = sourceComp.findReference(conn.refId);
					if (ref) {
						ref.targetId = conn.to;
					}
				}
			}
		}

		// 4. Разрешаем все ссылки
		AcfRegistry.resolveAll();
	}

	Component.onDestruction: {
		for (var i = 0; i < children.length; ++i) {
			var child = children[i];
			if (child.componentId && child.componentId !== "") {
				AcfRegistry.unregisterComponent(child.componentId);
			}
		}
		if (root.componentId !== "") {
			AcfRegistry.unregisterComponent(root.componentId);
		}
	}
}
