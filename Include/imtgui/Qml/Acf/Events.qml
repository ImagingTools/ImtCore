pragma Singleton
import QtQuick 2.0

Item {
    id: eventsContainer;
    property var events;

    Component.onCompleted: {
        eventsContainer.events = {};
    }

    function sendEvent(key, parameters){
        console.log("EVENTS sendEvent", key, parameters);
        if(key in this.events){
            for(var i = 0; i < this.events[key].length; i++){
                console.log("key i", key, i, this.events[key][i]);
                if (parameters){
                    this.events[key][i](parameters)
                }
                else{
                    this.events[key][i]()
                }
            }
        }
    }

    function subscribeEvent(key, slot){
        console.log("EVENTS subscribeEvent", key, slot);
        if(key in this.events){
            this.events[key].push(slot)
        } else {
            this.events[key] = [slot]
        }
    }

    function unSubscribeEvent(key, slot){
        if(key in this.events){
            var indx = this.events[key].indexOf(slot)
            if(indx >= 0) this.events[key].splice(indx, 1)
        }
    }
}
