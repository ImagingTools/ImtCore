import QtQuick 2.15
import Acf 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtbaseImtBaseTypesSdl 1.0

ParamEditorBase {
	id: urlParamEditor
	
	typeId: ParamTypeIdsTypeMetaInfo.s_urlParam
	
	editorModelComp: Component {
		UrlParam {}
	}
	
	property UrlParam urlParam: editorModel
	
	RegularExpressionValidator {
		id: urlValid;
		regularExpression: /^([a-zA-Z0-9+.-]+):\/\/([\w.-]+)(:\d+)?(\/.*)?$/;
	}
	
	sourceComp: Component {
		TextInputElementView {
			width: urlParamEditor.width
			name: urlParamEditor.name
			description: urlParamEditor.description
			controlWidth: 300
			textInputValidator: urlValid
			showErrorWhenInvalid: true
			onEditingFinished: {
				if (urlParamEditor.urlParam && acceptableInput){
					parseUrl(text)
				}
			}
			
			onTextChanged: {
				formatUrl()
			}
			
			property UrlParam urlParam: urlParamEditor.urlParam
			
			onUrlParamChanged: {
				if (urlParam){
					text = urlParam.m_scheme + "://" + urlParam.m_host + ":" + urlParam.m_port + urlParam.m_path
				}
			}
			
			function formatUrl() {
				let parts = text.match(/^([a-zA-Z0-9+.-]+)(:\/{0,2})?(.*)/);
				if (parts) {
					let protocol = parts[1];
					let slashes = parts[2] || "://";
					let rest = parts[3];
					
					if (!parts[2] && rest.length > 0) {
						text = protocol + "://" + rest;
					}
				}
			}
			
			function parseUrl(url){
				if (url.indexOf("://") === -1){
					console.error("Invalid URL");
					return;
				}
				
				let parts = url.split("://");
				urlParam.m_scheme = parts[0];
				
				let hostPart = parts[1].split("/")[0];
				
				if (hostPart.indexOf(":") !== -1) {
					let hostParts = hostPart.split(":");
					urlParam.m_host = hostParts[0];
					urlParam.m_port = hostParts[1];
				} else {
					urlParam.m_host = hostPart;
				}
			}
		}
	}
}
