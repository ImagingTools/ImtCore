import QtQuick 2.0
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0
import imtauthgui 1.0


/*
	WEB Component
*/
QtObject {
	id: root;

	signal ok(string gqlData, var gqlRequestRef);
	signal forbidden(string gqlData, var gqlRequestRef);
	signal unauthorized(string gqlData, var gqlRequestRef);
	signal internalError(string gqlData, var gqlRequestRef);

	Component.onCompleted: {
		if (Qt.platform.os == "web"){
			var originalXHROpen = XMLHttpRequest.prototype.open;
			var originalXHRSend = XMLHttpRequest.prototype.send;

			XMLHttpRequest.prototype.open = function(method, url){
				this._method = method;
				this._url = url;
				originalXHROpen.apply(this, arguments);
			};

			XMLHttpRequest.prototype.send = function(data){
				let gqlBody = data;

				var xhr = this;
				var originalOnReadyStateChange = xhr.onreadystatechange;
				var gqlRequestRef = xhr.gqlRequest;

				if (AuthorizationController.productId) {
					xhr.setRequestHeader("productId", AuthorizationController.productId);
				}

				xhr.onreadystatechange = function(){
					if (xhr.readyState === XMLHttpRequest.DONE){
						handleResponse(gqlBody, xhr, gqlRequestRef);
					}

					if (originalOnReadyStateChange){
						originalOnReadyStateChange.apply(xhr, arguments);
					}
				};

				xhr.addEventListener("loadend", function(){
					handleResponse(gqlBody, xhr, gqlRequestRef);
				});

				originalXHRSend.apply(xhr, arguments);
			};
		}
		else{
			NetworkEventInterceptor.unauthorized.connect(unauthorized)
			NetworkEventInterceptor.forbidden.connect(forbidden)
		}
	}

	function handleResponse(gqlBody, xhr, gqlRequestRef){
		switch(xhr.status){
			case 200:
				ok(gqlBody, gqlRequestRef);
				break;
			case 403:
				forbidden(gqlBody, gqlRequestRef);
				break;
			case 401:
				unauthorized(gqlBody, gqlRequestRef);
				break;
			case 500:
				internalError(gqlBody, gqlRequestRef);
				break;
		}
	}
}
