import {QtObject} from './QtObject'

export class FontLoader extends QtObject {

    static Null = 0
    static Loading = 1
    static Ready = 2
    static Error = 3

    constructor(args) {
        super(args)

        this.$cP('name', '')
        this.$cP('source', '', this.$sourceChanged)
        this.$cP('status', FontLoader.Null)
    }
    $domCreate(){
        super.$domCreate()

    }

    $sourceChanged(){
        this.status = FontLoader.Loading

        let temp = this.source.split('/')
        this.name = temp[temp.length-1].split('.')[0]

        let path = []
        if(Core.rootPath !== ''){
            path.push(Core.rootPath)
            path.push(this.source.replaceAll('../', '').replaceAll('./', ''))
        } else {    
            // if(Core.hostPath !== '') path.push(Core.hostPath)
            // if(Core.rootPath !== '') path.push(Core.rootPath)
            if(this.$basePath !== '') path.push(this.$basePath)
            path.push(this.source)
        }
        
        

        let domStyle = document.createElement("style")
        domStyle.innerHTML = `@font-face { font-family: ${this.name}; src: url('${path.join('/').replaceAll('//', '/')}'); }`
        document.head.appendChild(domStyle)

        let xhr = new XMLHttpRequest()
        xhr.open('GET', path.join('/').replaceAll('//', '/'), false)
        xhr.onload = ()=>{
            if(xhr.status === 200)
            document.fonts.load(`12px ${this.name}`).then((fonts)=>{
                if(fonts.length) this.status = FontLoader.Ready; else this.status = FontLoader.Error;
            })
        }
        xhr.send()
        

        

        // let xhr = new XMLHttpRequest()
        // xhr.open('GET', path.join('/'))
        // xhr.onload = () => {
        //     if(xhr.status >= 200 && xhr.status <= 300){
        //         this.status = FontLoader.Ready
        //     } else {
        //         this.status = FontLoader.Error
        //     }
        // }
        // xhr.onerror = () => {
        //     this.status = FontLoader.Error
        // }
        // xhr.send()
    }

}

QML.FontLoader = FontLoader