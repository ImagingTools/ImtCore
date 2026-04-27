import QtQuick 2.0
import com.imtcore.imtqml 1.0

/**
	AcfComponentWrapper provides a convenience QML wrapper around
	the C++ AcfComponent type.  It auto-registers itself with the
	AcfRegistry on completion and deregisters on destruction.

	Usage:
	\code
	AcfComponentWrapper {
		componentId: "MyDataProvider"
		packageId: "imtguigql"
		interfaces: ["IDataProvider", "IFilterable"]

		acfAttributes: [
			AcfAttribute { attrId: "ServerUrl"; type: "string"; required: true; defaultValue: "http://localhost" },
			AcfAttribute { attrId: "PageSize";  type: "int";    required: false; defaultValue: 20 }
		]

		acfReferences: [
			AcfReference { refId: "AuthProvider"; interfaceName: "IAuthProvider"; required: true }
		]
	}
	\endcode
*/
AcfComponent {
	id: root

	/// If true, auto-register with AcfRegistry on Component.onCompleted
	property bool autoRegister: true

	/// If true, auto-resolve references after registration
	property bool autoResolve: true

	/// User-defined content (child items are allowed but not treated as ACF children)
	default property alias data: __internal.children
	property QtObject __internal: QtObject { id: __internal }

	Component.onCompleted: {
		if (autoRegister && root.componentId !== ""){
			AcfRegistry.registerComponent(root);

			if (autoResolve){
				AcfRegistry.resolveComponent(root.componentId);
			}
		}
	}

	Component.onDestruction: {
		if (autoRegister && root.componentId !== ""){
			AcfRegistry.unregisterComponent(root.componentId);
		}
	}
}
