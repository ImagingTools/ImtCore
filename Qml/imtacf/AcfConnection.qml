import QtQuick 2.0

/**
	AcfConnection describes a wiring between two sibling components
	inside an AcfCompositeComponent.

	Usage:
	\code
	AcfConnection { from: "TicketEditor"; refId: "TicketList"; to: "TicketList" }
	\endcode

	Properties:
	- from:  componentId of the component that owns the reference
	- refId: the refId of the AcfReference within the source component
	- to:    componentId of the target component that satisfies the reference
*/
QtObject {
	/// componentId of the source component (the one that owns the reference)
	property string from

	/// refId of the AcfReference within the source component
	property string refId

	/// componentId of the target component
	property string to
}
