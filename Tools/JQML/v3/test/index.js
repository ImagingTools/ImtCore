const compiler = require('../compiler/compiler')
const fs = require('fs')
const { Builder, Capabilities } = require('selenium-webdriver')
const chrome = require('selenium-webdriver/chrome')
const path = require('path')
require('chromedriver') // Импорт нужен для автоматической настройки пути


function checkIsDirectorySync(path) {
  try {
    return fs.statSync(path).isDirectory()
  } catch (e) {
    return false
  }
}

function prepareTest(){
  const files = fs.readdirSync('./test')
  let testFiles = []

  for(file of files){
    if(checkIsDirectorySync(path.resolve(__dirname, file))){
      testFiles.push(path.resolve(__dirname, `./${file}/test.html`))

      compiler.compile({
        config: `./test/${file}/test.json`,
        output: `./test/${file}`,
        entry: `./test/${file}/Main.qml`,
        name: 'test',
        root: '/',
        mode: 'js',
      })

      compiler.compile({
        config: `./test/${file}/test.json`,
        output: `./test/${file}`,
        entry: `./test/${file}/Main.qml`,
        name: 'test',
        root: '/',
        mode: 'html',
      })
    }
  }

  return testFiles
}

async function runTest(testFiles) {
    // Настройка Chrome (опционально: запуск без окна)
    let options = new chrome.Options()
    options.addArguments('--headless') // Раскомментировать для headless-режима

    const caps = Capabilities.chrome()
    caps.setLoggingPrefs({
        browser: 'ALL', // Собирать все типы сообщений (INFO, WARNING, SEVERE)
        driver: 'WARNING'
    })

    let driver = await new Builder()
        .forBrowser('chrome')
        .setChromeOptions(options)
        .withCapabilities(caps)
        .build()

    try {
        for(let fileUrl of testFiles){
          await driver.get(fileUrl)
          
          // Ожидание (например, чтобы увидеть результат)
          // await driver.sleep(5000)

          // let screenshot = await driver.takeScreenshot()
          // fs.writeFileSync(fileUrl.replace('.html', '.png'), screenshot, 'base64')
          
          const logs = await driver.manage().logs().get('browser')
          console.log(`Логи для файла ${fileUrl}:`)
          logs.forEach(log => {
            if(log.level.name === 'INFO') {
              console.log(`${log.message.split(' ').slice(2).join(' ').slice(1, -1)}`) // Удаляем первые 2 слова и кавычки
            }
          })
        }
        
    } finally {
        await driver.quit() // Закрыть браузер
    }
}



runTest(prepareTest())