pragma Singleton
import QtQuick 2.0
import Acf 1.0
import imtqml 1.0

Item {
    id: eventsContainer;
    property var events;

    Component.onCompleted: {
        eventsContainer.events = {};
    }

    function sendEvent(key){
        console.log("sendEvent", key);
        if(key in this.events){
            for(var i = 0; i < this.events[key].length; i++){
                console.log("key i", key, i, this.events[key][i]);
                this.events[key][i]()
            }
        }
    }

    function subscribeEvent(key, slot){
        console.log("subscribeEvent", key, slot);
        if(key in this.events){
            this.events[key].push(slot)
        } else {
            this.events[key] = [slot]
        }
        console.log("eventsContainer.events", eventsContainer.events);
    }

    function unSubscribeEvent(key, slot){
        if(key in this.events){
            var indx = this.events[key].indexOf(slot)
            if(indx >= 0) this.events[key].splice(indx, 1)
        }
    }
}
