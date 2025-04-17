import QtQuick 2.0
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0
import imtauthgui 1.0
import imtgui 1.0
import imtguigql 1.0
import imtbaseSettingsSdl 1.0
import imtbaseImtBaseTypesSdl 1.0

QtObject {
	id: container;
	
	property TreeItemModel serverModel: null;
	property TreeItemModel localModel: null;
	
	property var applicationInfoProvider;
	
	onApplicationInfoProviderChanged: {
		if (container.applicationInfoProvider){
			container.aboutApplicationProvider.applicationInfoProvider = container.applicationInfoProvider;
		}
	}
	
	property PageAboutProvider aboutApplicationProvider: PageAboutProvider{}
	
	property QtObject private_: QtObject {
		property TreeItemModel representationModel: TreeItemModel {};
		
		function mergeWithExternModel(externModel){
			if (externModel != undefined && externModel != null){
				for (let i = 0; i < externModel.getItemsCount(); i++){
					let pageId = externModel.getData("id", i);
					let pageName = externModel.getData("name", i);
					let pageParameters = externModel.getData("Parameters", i);
					
					let index = container.private_.getPageIndexByPageId(pageId)
					if (index < 0){
						index = container.private_.representationModel.insertNewItem();
					}
					
					container.private_.representationModel.setData("id", pageId, index);
					container.private_.representationModel.setData("name", pageName, index);
					
					if(pageParameters !== null){
						let parametersModel = container.private_.representationModel.addTreeModel("Parameters", index);
						parametersModel.copy(pageParameters)
					}
				}
			}
		}
		
		function getPageIndexByPageId(pageId){
			for (let i = 0; i < container.private_.representationModel.getItemsCount(); i++){
				let id = container.private_.representationModel.getData("id", i);
				if (String(id) === String(pageId)){
					return i;
				}
			}
			
			return -1;
		}
		
		function setValueToLocalModel(rootKey, key, value){
			if (!container.localModel){
				return;
			}
			
			for (let i = 0; i < container.localModel.getItemsCount(); i++){
				let id = String(container.localModel.getData("id", i));
				if (id === rootKey){
					let parametersModel = container.localModel.getData("Parameters", i)
					if (parametersModel){
						for (let j = 0; j < parametersModel.getItemsCount(); j++){
							let parameterId = String(parametersModel.getData("id", j));
							if (parameterId === key){
								parametersModel.setData("Value", value, j);
								break;
							}
						}
					}
					
					break;
				}
			}
		}
		
		function setValueToServerModel(rootKey, key, value){
			if (!container.serverModel){
				return;
			}
			
			for (let i = 0; i < container.serverModel.getItemsCount(); i++){
				let id = String(container.serverModel.getData("id", i));
				if (id === rootKey){
					let parametersModel = container.serverModel.getData("Parameters", i)
					if (parametersModel){
						for (let j = 0; j < parametersModel.getItemsCount(); j++){
							let parameterId = String(parametersModel.getData("id", j));
							if (parameterId === key){
								parametersModel.setData("Value", value, j);
								break;
							}
						}
					}
					break;
				}
			}
		}
		
		function getParameters(rootKey, key){
			if (!container.serverModel){
				return null;
			}
			for (let i = 0; i < container.serverModel.getItemsCount(); i++){
				let id = String(container.serverModel.getData("id", i));
				if (id === rootKey){
					let parametersModel = container.serverModel.getData("Parameters", i)
					
					return parametersModel;
				}
			}
			
			return null;
		}
	}
	
	property bool modelsCompleted: serverModel && localModel;
	
	signal serverSettingsSaved();
	signal localSettingsSaved();
	
	onServerModelChanged: {
		container.cacheServerModel();
	}
	
	onLocalModelChanged: {
		if (container.localModel){
			container.localModel.modelChanged.connect(container.onLocalModelDataChanged);
		}
	}
	
	function onLocalModelDataChanged(){
	}
	
	function clearModel(){
		if (container.serverModel){
			container.serverModel.clear();
		}
	}
	
	function getRepresentationModel(){
		return paramsSet
	}
	
	function updateModel(){
		getSettingsQuery.send()
	}
	
	function saveLocalModel(){
		container.localSettingsSaved();
	}
	
	function saveServerModel(){
		setSettingsQuery.send()
	}
	
	function setDesignSchema(schema){
		let model = container.serverModel;
		
		if (!model){
			model = container.localModel;
		}
		
		if (model){
			for (let i = 0; i < model.getItemsCount(); i++){
				let id = String(model.getData("id", i));
				if (id == String("General")){
					let parametersModel = model.getData("Parameters", i)
					if (parametersModel){
						for (let j = 0; j < parametersModel.getItemsCount(); j++){
							let parameterId = String(parametersModel.getData("id", j));
							if (parameterId == String("DesignSchema")){
								let languageParametersModel = parametersModel.getData("Parameters", j);
								for (let k = 0; k < languageParametersModel.getItemsCount(); k++){
									let langId = languageParametersModel.getData("id", k);
									if (String(langId) == String(schema)){
										let currentValue = parametersModel.getData("Value", j)
										if (currentValue === k){
											return;
										}
										
										parametersModel.setData("Value", k, j);
										break;
									}
								}
								break;
							}
						}
					}
					break;
				}
			}
		}
		
		if (container.serverModel){
			container.cacheServerModel();
			
			preferenceSaveQuery.save();
		}
		else{
			container.localSettingsSaved();
		}
	}
	
	function setLanguage(language){
		let model = container.serverModel;
		
		if (!model){
			model = container.localModel;
		}
		
		if (model){
			for (let i = 0; i < model.getItemsCount(); i++){
				let id = String(model.getData("id", i));
				if (id == String("General")){
					let parametersModel = model.getData("Parameters", i)
					if (parametersModel){
						for (let j = 0; j < parametersModel.getItemsCount(); j++){
							let parameterId = String(parametersModel.getData("id", j));
							if (parameterId == String("Language")){
								let languageParametersModel = parametersModel.getData("Parameters", j);
								if (languageParametersModel){
									for (let k = 0; k < languageParametersModel.getItemsCount(); k++){
										let langId = languageParametersModel.getData("id", k);
										if (String(langId) == String(language)){
											let currentValue = parametersModel.getData("Value", j)
											if (currentValue === k){
												return;
											}
											
											parametersModel.setData("Value", k, j);
											break;
										}
									}
									break;
								}
							}
						}
					}
					break;
				}
			}
		}
		
		if (container.serverModel){
			container.cacheServerModel();
			
			preferenceSaveQuery.save();
		}
		else{
			container.localSettingsSaved();
		}
	}
	
	function setServerUrl(serverUrl){
		private_.setValueToLocalModel("NetworkSettings", "ServerUrl", serverUrl);
		
		container.localSettingsSaved();
	}
	
	function setWebSocketServerUrl(webSocketServerUrl){
		private_.setValueToLocalModel("NetworkSettings", "WebSocketServerUrl", webSocketServerUrl);
		
		container.localSettingsSaved();
	}
	
	function cacheServerModel(){
		container.rewriteModel(container.serverModel, container.localModel);
		container.localSettingsSaved();
	}
	
	function rewriteModel(fromModel, toModel){
		if (!fromModel || !toModel){
			return;
		}
		
		for (let i = 0; i < toModel.getItemsCount(); i++){
			let pageId = toModel.getData("id", i);
			let index = -1;
			for (let j = 0; j < fromModel.getItemsCount(); j++){
				let id = fromModel.getData("id", j);
				if (id == pageId){
					index = j;
					
					break;
				}
			}
			
			if (index >= 0){
				toModel.copyItemDataFromModel(i, fromModel, index);
			}
		}
	}
	
	function getValue(parameterId){
		let settingsModel = getRepresentationModel();
		
		return findValue(settingsModel, parameterId);
	}
	
	function findValue(model, parameterId){
		for (let i = 0; i < model.getItemsCount(); i++){
			let paramId = model.getData("id", i);
			if (String(paramId) === String(parameterId)){
				return model.getData("Value", i)
			}
			
			if (model.containsKey("Parameters", i)){
				let parameters = model.getData("Parameters", i);
				
				let result = findValue(parameters, parameterId);
				if (result !== ""){
					return result;
				}
			}
		}
		
		return "";
	}
	
	
	function getHeaders(){
		return {};
	}
}
