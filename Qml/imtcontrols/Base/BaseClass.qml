import QtQuick 2.0

QtObject {
    signal dataChanged(string name, var sender)
    signal finished
    property bool enableNotifications: true
    property var owner

    Component.onCompleted: {
        console.log("BaseClass onCompleted");

        connectProperties();
    }

    onDataChanged: {
        console.log("BaseClass onDataChanged");

        if(owner && owner.enableNotifications && owner.dataChanged){
            owner.dataChanged(name, sender)
        }
    }

    property bool propertiesIsConnected: false

    function connectProperties(){
        console.log("BaseClass connectProperties");

        if (propertiesIsConnected){
            return;
        }

        let self = this
        let list = getProperties()

        for(let name of list){
            this[name+'Changed'].connect(function(){
                if(self.enableNotifications)
                    self.dataChanged(name, self)
            }
           )
        }

        propertiesIsConnected = true;
    }

    function createMe(){
        return Qt.createComponent('BaseClass.qml').createObject()
    }

    function isEqualWithModel(model){
        if (typeof this != typeof model){
            return false;
        }

        let selfKeys = this.getProperties()
        let sourceKeys = model.getProperties()

        if (selfKeys.length !== sourceKeys.length){
            return false;
        }

        for(let i = 0; i < selfKeys.length; i++){
            let key = selfKeys[i];

            if (!sourceKeys.includes(key)){
                return false;
            }

            if(typeof this[key] !== typeof model[key]){
                return false;
            }

            if(typeof this[key] === 'object'){
                let ok = this[key].isEqualWithModel(model[key]);
                if (!ok){
                    return false;
                }
            }
            else{
                if (this[key] !== model[key]){
                    return false;
                }
            }
        }

        return true;
    }

    function refresh(){
    }

    function copy(item){
        return copyFrom(item)
    }

    function copyMe(){
        let obj = createMe()
        obj.fromJSON(toJson())
        return obj
    }

    function copyFrom(item){
        fromJSON(item.toJson())
        return true
    }

    function copyTo(item){
        item.fromJSON(toJson())
        return true
    }

    function createComponent(name){

    }

    function getJSONKeyForProperty(propertyId){
        return propertyId
    }

    function getProperties(){
        let list = []
        if(Qt.platform.os === 'web'){
            for(let key in this.$properties){
                if(key.indexOf('m_') >= 0 && key !== 'owner' && key !== 's_keys' && key !== 'objectName' && key !== 'enableNotifications' && typeof this[key] !== "function"){
                    list.push(key)
                }
            }
        } else {
            for(let key in this){
                if(key.indexOf('m_') >= 0 && key !== 'objectName' && key !== 'owner' && key !== 's_keys' && key !== 'enableNotifications' && typeof this[key] !== "function"){
                    list.push(key)
                }
            }
        }

        return list
    }

    function createFromJson(json){
        return fromJSON(json);
    }

    function toJson(){
        let list = getProperties()

        let json = '{'
        for(let i = 0; i < list.length; i++){
            let key = list[i]
            if(typeof this[key] === 'object'){
                json += '"' + this.getJSONKeyForProperty(key) + '":' + this[key].toJson()
            } else {
                let value = this[key]
                if (value === undefined){
                    value = null
                }

                json += '"' + this.getJSONKeyForProperty(key) + '":' + (typeof this[key] === 'string' ? '"' + this[key] + '"' : value)
            }
            if(i < list.length - 1) json += ','
        }
        json +='}'
        return json
    }

    function toGraphQL(){
        let list = getProperties()

        let graphQL = '{'
        for(let i = 0; i < list.length; i++){
            let key = list[i]
            if(typeof this[key] === 'object'){
                graphQL += this.getJSONKeyForProperty(key) + ':' + this[key].toGraphQL()
            } else {
                let value = this[key]
                if (value === undefined){
                    value = null
                }

                graphQL += this.getJSONKeyForProperty(key) + ':' + (typeof this[key] === 'string' ? '"' + this[key] + '"' : value)
            }
            if(i < list.length - 1) graphQL += ','
        }
        graphQL +='}'
        return graphQL
    }

    function fromJSON(json){
        return fromObject(JSON.parse(json))
    }

    function fromObject(sourceObject){
        for(let key in sourceObject){
            let _key = "m_" + key[0].toLowerCase() + key.slice(1,key.length)
            if(typeof sourceObject[key] === "object"){
                if(Array.isArray(sourceObject[key])){
                    if(this[_key]){
                        this[_key].clear()
                    } else {
                        let obj = Qt.createComponent('BaseModel.qml').createObject(this)
                        this[_key] = obj
                    }
                    for(let sourceObjectInner of sourceObject[key]){
                        let obj = createComponent(_key).createObject(this)
                        obj.fromObject(sourceObjectInner)
                        this[_key].append({item: obj})
                        obj.owner = this
                        obj.connectProperties()

                    }
                } else {
                    let obj = createComponent(_key).createObject(this)
                    obj.fromObject(sourceObject[key])
                    this[_key] = obj
                    obj.owner = this
                    obj.connectProperties()

                }
            } else {
                this[_key] = sourceObject[key]
            }

        }
        finished()
    }
}
