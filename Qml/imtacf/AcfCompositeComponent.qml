import QtQuick 2.0
import com.imtcore.imtqml 1.0

/**
	AcfCompositeComponent is a container that groups multiple
	AcfComponentWrapper children and resolves their internal
	connections via AcfConnection declarations.

	Usage:
	\code
	AcfCompositeComponent {
		componentId: "TicketWorkspace"
		packageId: "imtdeskgui"
		interfaces: ["IWorkspace"]

		AcfComponentWrapper {
			componentId: "TicketList"
			acfReferences: [
				AcfReference { refId: "DataProvider"; interfaceName: "IDataProvider" }
			]
		}

		AcfComponentWrapper {
			componentId: "TicketEditor"
			acfReferences: [
				AcfReference { refId: "TicketList"; interfaceName: "ISelectable" }
			]
		}

		acfConnections: [
			AcfConnection { from: "TicketEditor"; refId: "TicketList"; to: "TicketList" }
		]
	}
	\endcode
*/
AcfComponent {
	id: root

	/// If true, auto-register with AcfRegistry on Component.onCompleted
	property bool autoRegister: true

	/// Child ACF components
	default property list<AcfComponent> children

	/// Internal connections between child components
	property list<QtObject> acfConnections

	Component.onCompleted: {
		// Register this composite
		if (autoRegister && root.componentId !== ""){
			AcfRegistry.registerComponent(root);
		}

		// Register all children
		for (var i = 0; i < children.length; ++i){
			var child = children[i];
			if (child.componentId && child.componentId !== ""){
				AcfRegistry.registerComponent(child);
			}
		}

		// Apply explicit connections
		for (var j = 0; j < acfConnections.length; ++j){
			var conn = acfConnections[j];
			if (conn && conn.from && conn.refId && conn.to){
				var sourceComp = AcfRegistry.getComponent(conn.from);
				var targetComp = AcfRegistry.getComponent(conn.to);
				if (sourceComp && targetComp){
					var ref = sourceComp.findReference(conn.refId);
					if (ref){
						ref.targetId = conn.to;
					}
				}
			}
		}

		// Resolve all
		AcfRegistry.resolveAll();
	}

	Component.onDestruction: {
		for (var i = 0; i < children.length; ++i){
			var child = children[i];
			if (child.componentId && child.componentId !== ""){
				AcfRegistry.unregisterComponent(child.componentId);
			}
		}

		if (autoRegister && root.componentId !== ""){
			AcfRegistry.unregisterComponent(root.componentId);
		}
	}
}
