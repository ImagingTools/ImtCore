import QtQuick 2.0
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtauthgui 1.0


/*
	WEB Component
*/
QtObject {
	id: root;

	signal ok(string gqlData, var gqlRequestRef);
	signal forbidden(string gqlData, var gqlRequestRef, bool credentialsChanged);
	// gqlHeaders: the headers its sender gave the request, without those of the session; it is sent again with them
	signal unauthorized(string gqlData, var gqlRequestRef, var gqlHeaders, bool credentialsChanged);
	signal internalError(string gqlData, var gqlRequestRef);

	Component.onCompleted: {
		if (Qt.platform.os == "web"){
			var originalXHROpen = XMLHttpRequest.prototype.open;
			var originalXHRSend = XMLHttpRequest.prototype.send;
			var originalXHRSetRequestHeader = XMLHttpRequest.prototype.setRequestHeader;

			XMLHttpRequest.prototype.open = function(method, url){
				this._method = method;
				this._url = url;
				this.__imtHeaders = {};
				originalXHROpen.apply(this, arguments);
			};

			XMLHttpRequest.prototype.setRequestHeader = function(name, value){
				if (this.__imtHeaders && root.isSenderHeader(name)){
					this.__imtHeaders[name] = value;
				}
				originalXHRSetRequestHeader.apply(this, arguments);
			};

			XMLHttpRequest.prototype.send = function(data){
				let gqlBody = data;

				var xhr = this;
				var originalOnReadyStateChange = xhr.onreadystatechange;
				var gqlRequestRef = xhr.gqlRequest;
				xhr.__imtSessionGeneration = AuthorizationController.__sessionGeneration;

				// A console works for the product it administers; the server allows it to a superuser only
				let requestProductId = AuthorizationController.administeredProductId !== "" ? AuthorizationController.administeredProductId : AuthorizationController.productId
				if (requestProductId) {
					xhr.setRequestHeader("productId", requestProductId);
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

	// The runtime and this proxy set the session headers of every request themselves
	function isSenderHeader(name){
		let headerId = String(name).toLowerCase()

		return headerId !== "x-authentication-token" && headerId !== "productid" && headerId !== "content-type"
	}

	function handleResponse(gqlBody, xhr, gqlRequestRef){
		// Both the wrapped onreadystatechange (DONE) and the "loadend" listener fire
		// for the same reply, so every response used to be dispatched twice — which
		// on 401 queued the same request twice in AuthorizationController's retry
		// queue and re-sent it twice after the token refresh.
		if (xhr.__imtResponseHandled){
			return;
		}

		xhr.__imtResponseHandled = true;

		switch(xhr.status){
			case 200:
				ok(gqlBody, gqlRequestRef);
				break;
			case 403:
				forbidden(gqlBody, gqlRequestRef, xhr.__imtSessionGeneration !== AuthorizationController.__sessionGeneration);
				break;
			case 401:
				unauthorized(gqlBody, gqlRequestRef, xhr.__imtHeaders ? xhr.__imtHeaders : {}, xhr.__imtSessionGeneration !== AuthorizationController.__sessionGeneration);
				break;
			case 500:
				internalError(gqlBody, gqlRequestRef);
				break;
		}
	}
}
