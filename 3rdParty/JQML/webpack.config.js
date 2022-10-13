const path = require('path');

module.exports = {
    entry: "./Core/src/index.js",
    output: {
        path: path.resolve(__dirname, 'Core/dist'),
    },
    // resolve: {
    //     fallback: { "timers": false }
    // }
    
}