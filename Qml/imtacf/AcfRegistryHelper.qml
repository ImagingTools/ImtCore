import QtQuick 2.0
import com.imtcore.imtqml 1.0

/**
	AcfRegistryHelper is a singleton that provides convenience
	functions for working with the C++ AcfRegistry from QML.
*/
pragma Singleton

QtObject {
	id: root

	/// Shortcut to AcfRegistry.componentCount
	readonly property int componentCount: AcfRegistry.componentCount

	/**
		Load a JSON configuration string into the registry
		and resolve all references.
		Returns true on success.
	*/
	function loadAndResolve(jsonString) {
		var ok = AcfRegistry.loadConfiguration(jsonString);
		if (ok){
			AcfRegistry.resolveAll();
		}

		return ok;
	}

	/**
		Export the component graph as a JavaScript object.
		Returns { nodes: [...], edges: [...] }.
	*/
	function getGraph() {
		return AcfRegistry.exportGraph();
	}

	/**
		Find all components implementing a given interface.
		Returns a JavaScript array of AcfComponent objects.
	*/
	function findByInterface(interfaceName) {
		return AcfRegistry.getAllByInterface(interfaceName);
	}

	/**
		Dump all registered component IDs to console (for debugging).
	*/
	function dumpComponentIds() {
		var ids = AcfRegistry.componentIds();
		console.log("=== ACF Registry: " + ids.length + " components ===");
		for (var i = 0; i < ids.length; ++i){
			var comp = AcfRegistry.getComponent(ids[i]);
			var ifaces = comp ? comp.interfaces.join(", ") : "";
			console.log("  [" + ids[i] + "] interfaces: [" + ifaces + "] active: " + (comp ? comp.isActive : "?"));
		}
		console.log("=== End ACF Registry ===");
	}
}
