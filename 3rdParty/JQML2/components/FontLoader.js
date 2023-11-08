const { QtObject } = require('./QtObject')
const { QString, QReal } = require('../utils/properties')

class FontLoader extends QtObject {
    static Null = 0
    static Loading = 1
    static Ready = 2
    static Error = 3

    static defaultProperties = {
        name: { type: QString, value: '' },
        source: { type: QString, value: '', changed: 'sourceChanged' },
        status: { type: QReal, value: FontLoader.Null },
    }
    
    
    sourceChanged(){
        if(this.$font) document.fonts.delete(this.$font)

        let path = this.getPropertyValue('source').split('/')
        let name = path[path.length-1].split('.')[0]

        // let xhr = new XMLHttpRequest()
        // xhr.responseType = "arraybuffer"
        // xhr.open('GET', this.getPropertyValue('source'), true)
        
        this.$font = new FontFace(name, `url('${this.getPropertyValue('source')}')`)
        this.$font.load().then(()=>{
            document.fonts.add(this.$font)
            this.getProperty('name').reset(name)
        },
        (error)=>{
            console.log(error)
        })

        

        // xhr.onload = (event) => {
        //     if (xhr.response) {

        //         this.$font = new FontFace(name, xhr.response)
        //         document.fonts.add(this.$font)
        //         this.getProperty('name').reset(name)

        //         // console.log('FontLoader', this.$font, xhr.response)
        //     }

        //     // if (xhr.status === 200) {
        //     //     let data = new ArrayBuffer(xhr.response.length);
        //     //     let buffer = new Uint8Array(data);
            
        //     //     for (let i = 0; i < xhr.response.length; i++) {
        //     //         buffer[i] = xhr.response.charCodeAt(i);
        //     //     }
                
        //     //     this.$font = new FontFace(name, data)
        //     //     document.fonts.add(this.$font)
        //     //     this.getProperty('name').reset(name)

        //     //     console.log('FontLoader', this.$font, data)
        //     // }
            
        //     // ...
            

        // }

        // xhr.send(null)

    }
    
    $destroy(){
        document.fonts.delete(this.$font)
        super.$destroy()
    }
}

module.exports.FontLoader = FontLoader