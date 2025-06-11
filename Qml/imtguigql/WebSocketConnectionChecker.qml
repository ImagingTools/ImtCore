import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtguigql 1.0


/* Usage example:
	ApplicationMain {
		id: window;
		
		authorizationServerConnected: pumaConnectionChecker.status === 1;
		
		PumaConnectionChecker {
			id: pumaConnectionChecker;
			subscriptionManager: window.subscriptionManager;
		}
	}
*/

SubscriptionClient {
	id: pumaSub;
	
	property int status: -1; // -1 - Unknown, 0 - Connecting, 1 - Connected, 2 - Disconnected
	
	property SubscriptionManager subscriptionManager;
	
	function registerSubscription(){
		status = 0
		let query = getGqlQuery();
		subscriptionManager.registerSubscriptionEvent({"Query": query, "Client": pumaSub, "Headers": pumaSub.getHeaders()});
	}
	
	function unRegisterSubscription(){
		status = 2
		subscriptionManager.unRegisterSubscription(pumaSub);
	}
	
	onMessageReceived: {
		let ok = false
		if (data.containsKey("status")){
			let status = data.getData("status")
			if (status === "Disconnected"){
				pumaSub.status = 2;
				ok = true
			}
			else if (status === "Connected"){
				pumaSub.status = 1;
				ok = true
			}
		}
		
		if (!ok){
			pumaSub.status = -1;
		}
	}
}

