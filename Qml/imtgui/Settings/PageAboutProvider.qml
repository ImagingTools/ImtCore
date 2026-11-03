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
	
	property string serverVersion
	// Services the server depends on, with their own dependencies (ServiceInfo list of GetApplicationInfo)
	property var dependencies: null
	
	Component.onCompleted: {
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
	
	// Data of one dependency row, m_version holds the formatted version, see ServiceDependencyView
	property ServiceInfo dependencyRowInfo: ServiceInfo {}
	
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
	
	// One row per service, a nested service is named by its path, e.g. "Puma / Lisa"
	function addDependencyParams(dependencyList, parentName, parentId){
		if (!dependencyList){
			return
		}
	
		for (let i = 0; i < dependencyList.getItemsCount(); ++i){
			let dependency = dependencyList.get(i).item
			if (!dependency){
				continue
			}
	
			let serviceId = dependency.m_serviceId ? String(dependency.m_serviceId) : ""
			let applicationName = dependency.m_applicationName ? String(dependency.m_applicationName) : ""
			let name = applicationName !== "" ? applicationName : serviceId
			if (parentName !== ""){
				name = parentName + " / " + name
			}
	
			let id = parentId + "/" + i
	
			root.dependencyRowInfo.m_serviceId = serviceId
			root.dependencyRowInfo.m_status = dependency.m_status ? dependency.m_status : "UNKNOWN"
			root.dependencyRowInfo.m_applicationName = applicationName
			root.dependencyRowInfo.m_version = formatVersion(dependency.m_version, dependency.m_versionKind, dependency.m_buildTime)
	
			addParam("ServiceDependency", id, name, "", root.dependencyRowInfo.toJson())
	
			addDependencyParams(dependency.m_dependencies, name, id)
		}
	}
	
	function createRepresentation(){
		clearParamsSet()
	
		addParam(ParamTypeIdsTypeMetaInfo.s_textView, "ServerVersion", qsTr("Server Version"), "", serverVersionText.toJson())
		addDependencyParams(root.dependencies, "", "ServiceDependency")
		addParam("ServerLog", "ServerLog", qsTr("Server Log"), "", "")
	}
}
