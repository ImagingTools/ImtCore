pragma Singleton
import QtQuick 2.0

QtObject {
    id: eventsContainer;
    property var events;

    Component.onCompleted: {
        eventsContainer.events = {};
    }

    function sendEvent(key, parameters){
        if(key in this.events){
            for(var i = 0; i < this.events[key].length; i++){

                if (parameters !== undefined){
                    this.events[key][i](parameters)
                }
                else{
                    this.events[key][i]()
                }
            }
        }
    }

    function subscribeEvent(key, slot){
        if(key in this.events){
            let slots = this.events[key]

            if (!slots.includes(slot)){
                this.events[key].push(slot)
            }
        }
        else {
            this.events[key] = [slot]
        }
    }

    function unSubscribeAllFromSlot(slot){
        for(let key in this.events){
            let slots = this.events[key]
            if (slots.includes(slot)){
                this.unSubscribeEvent(key, slot);
            }
        }
    }

    function unSubscribeEvent(key, slot){
        if(key in this.events){
            var indx = this.events[key].indexOf(slot)
            if(indx >= 0) this.events[key].splice(indx, 1)
        }
    }
}
