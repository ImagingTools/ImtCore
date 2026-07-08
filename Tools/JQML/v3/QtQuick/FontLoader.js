const QtObject = require("../QtQml/QtObject")
const Real = require("../QtQml/Real")
const String = require("../QtQml/String")
const Signal = require("../QtQml/Signal")

class FontLoader extends QtObject {
    static Null = 0
    static Loading = 1
    static Ready = 2
    static Error = 3

    static meta = Object.assign({}, QtObject.meta, {
        name: { type: String, value: ''},
        source: { type: String, value: ''},
        status: { type: Real, value: FontLoader.Null},

        nameChanged: { type:Signal, args:[] },
        sourceChanged: { type:Signal, args:[] },
        statusChanged: { type:Signal, args:[] },
    })

     SLOT_sourceChanged(oldValue, newValue){
        if(this.__font) document.fonts.delete(this.__font)

        let url

        if(newValue.startsWith('data:') || newValue.startsWith('http:') || newValue.startsWith('https://')){
            url = newValue
        } else {
            url = JQApplication.rootPath + newValue.replaceAll('qrc:/','').replaceAll('../','')
        }

        let name = newValue.split('/').pop().split('.')[0]

        this.__font = new FontFace(name, `url('${url.replace(/(?<!:)\/{2,}/g, '/')}')`)
        this.__font.load().then(()=>{
            document.fonts.add(this.__font)
            this.name = name
        },
        (error)=>{
            console.log(error)
        })
    }
    
    __destroy(){
        document.fonts.delete(this.__font)
        super.__destroy()
    }
}



module.exports = FontLoader