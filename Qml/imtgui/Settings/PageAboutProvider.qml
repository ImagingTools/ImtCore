import QtQuick 2.0
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtbaseImtBaseTypesSdl 1.0
import imtappApplicationSdl 1.0

ParamsSetController {
	id: root;
	
	paramModel: ParamsSet {}
	
	property Component serverLogProvider: Component {
		ServerLogProvider {}
	}
	
	property string serverName
	property string serverVersion
	// Services the server depends on, with their own dependencies (ServiceInfo list of GetApplicationInfo)
	property var dependencies: null

	Component.onCompleted: {
		createRepresentation()
	}

	onServerNameChanged: {
		createRepresentation()
	}

	onServerVersionChanged: {
		serverVersionText.m_text = root.serverVersion
		createRepresentation()
	}

	onDependenciesChanged: {
		createRepresentation()
	}

	property TextParam serverVersionText: TextParam {
		m_text: root.serverVersion
	}

	// Tree of the server and its dependencies, m_version holds the formatted version, see ServiceDependencyView
	property ServiceInfo serviceTree: ServiceInfo {}

	// "1.0.11563 - Beta Version (03.11.2026 14:25)", buildTime is an ISO date in UTC
	function formatVersion(version, versionKind, buildTime){
		let text = version ? String(version) : ""
		if (versionKind){
			text = text === "" ? String(versionKind) : text + " - " + String(versionKind)
		}
		if (buildTime){
			let buildTimeText = Qt.formatDateTime(new Date(String(buildTime)), "dd.MM.yyyy hh:mm")
			text = text === "" ? buildTimeText : text + " (" + buildTimeText + ")"
		}
	
		return text
	}
	
	// Copies the dependencies into the tree node, with the version already formatted
	function addDependencies(node, dependencyList){
		node.emplaceDependencies()

		for (let i = 0; i < dependencyList.getItemsCount(); ++i){
			let dependency = dependencyList.get(i).item
			if (!dependency){
				continue
			}

			let child = node.createDependenciesArrayElement()
			child.m_serviceId = dependency.m_serviceId ? String(dependency.m_serviceId) : ""
			child.m_status = dependency.m_status ? String(dependency.m_status) : "UNKNOWN"
			child.m_credentialStatus = dependency.m_credentialStatus ? String(dependency.m_credentialStatus) : "NOT_USED"
			child.m_applicationName = dependency.m_applicationName ? String(dependency.m_applicationName) : ""
			child.m_version = formatVersion(dependency.m_version, dependency.m_versionKind, dependency.m_buildTime)

			if (dependency.m_dependencies && dependency.m_dependencies.getItemsCount() > 0){
				addDependencies(child, dependency.m_dependencies)
			}

			node.m_dependencies.addElement(child)
		}
	}

	function createRepresentation(){
		clearParamsSet()

		addParam(ParamTypeIdsTypeMetaInfo.s_textView, "ServerVersion", qsTr("Server Version"), "", serverVersionText.toJson())

		if (root.dependencies && root.dependencies.getItemsCount() > 0){
			root.serviceTree.m_applicationName = root.serverName
			root.serviceTree.m_status = "CONNECTED"
			root.serviceTree.m_version = root.serverVersion
			addDependencies(root.serviceTree, root.dependencies)

			addParam("ServiceDependency", "Services", qsTr("Services"), "", root.serviceTree.toJson())
		}

		addParam("ServerLog", "ServerLog", qsTr("Server Log"), "", "")
	}
}
