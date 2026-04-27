import QtQuick 2.0

/*!
	\qmltype AcfConnection
	\inqmlmodule imtacf
	\brief Описание связи между двумя компонентами-сиблингами внутри AcfCompositeComponent.

	AcfConnection задаёт явную «проводку» — указывает, что ссылка refId
	компонента from должна быть связана с компонентом to.

	\qml
	AcfCompositeComponent {
		componentId: "Workspace"

		AcfComponent { componentId: "Editor";  acfReferences: [ AcfReference { refId: "List" } ] }
		AcfComponent { componentId: "TicketList" }

		acfConnections: [
			AcfConnection { from: "Editor"; refId: "List"; to: "TicketList" }
		]
	}
	\endqml
*/
QtObject {
	/*! componentId компонента-источника (владельца ссылки). */
	property string from: ""

	/*! refId ссылки внутри компонента-источника. */
	property string refId: ""

	/*! componentId целевого компонента. */
	property string to: ""
}
