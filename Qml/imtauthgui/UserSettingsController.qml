import QtQuick 2.0
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtguigql 1.0
import imtgui 1.0
import imtbaseSettingsSdl 1.0
import imtbaseImtBaseTypesSdl 1.0

ParamsSetController {
	id: root

	property string permissionPath: ""
	signal settingsReceived()
	signal settingsReceiveFailed()
	signal settingsSaved()
	signal settingsSaveFailed()
	
	// The callback is called when the settings are applied, or when the request failed
	function getSettings(callback){
		getSettingsQuery.callback = callback
		getSettingsQuery.send()
	}
	
	function saveParam(){
		setSettingsQuery.send()
	}

	property GqlSdlRequestSender getSettingsQuery: GqlSdlRequestSender {
		property var callback: null

		permissionPath: root.permissionPath
		gqlCommandId: ImtbaseSettingsSdlCommandIds.s_getSettings

		onFinished: {
			if (callback){
				callback()
			}
		}
		inputObjectComp: Component {
			GetSettingsInput {
				m_userId: AuthorizationController.getUserId()
			}
		}

		sdlObjectComp: Component {
			ParamsSet {
				onFinished: {
					root.createParamFromJson(this.toJson())
					root.settingsReceived()
				}
			}
		}
	}
	
	property GqlSdlRequestSender setSettingsQuery: GqlSdlRequestSender {
		permissionPath: root.permissionPath
		requestType: 1
		gqlCommandId: ImtbaseSettingsSdlCommandIds.s_setSettings
		inputObjectComp: Component {
			SetSettingsInput {
				m_userId: AuthorizationController.getUserId()
				m_settings: root.paramModel.toJson()
			}
		}

		sdlObjectComp: Component {
			SetSettingsPayload {
				onFinished: {
					if (m_ok){
						root.settingsSaved()
						root.paramModelCreated()
					}
					else{
						root.settingsSaveFailed()
					}
				}
			}
		}
	}
}
