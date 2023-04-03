import {QtObject} from './QtObject'
import {Signal} from '../utils/Signal'

export class Href extends QtObject  {
    constructor(args) {
        super(args)


        this.$cP('url', '')
        this.$interval = setInterval(()=>{
            if(this.url !== location.href)
            this.url = location.href
        }, 1000 / Core.FPS)

        // this.$s.urlChanged = Signal()
        // console.log('DEBUG::Href', this)
        // this._context.backend.window.addEventListener('locationchange', (e) => {
        //     console.log('DEBUG::', e)
        //     setTimeout(()=>{
        //         this.urlChanged()
        //     }, 100)
            
        // }, false)
    }
    $domCreate(){}

    $destroy(){
        clearInterval(this.$interval)
        super.$destroy()
    }
}

QML.Href = Href