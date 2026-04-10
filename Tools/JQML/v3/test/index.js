const compiler = require('../compiler/compiler')
const fs = require('fs')
const { Builder, Capabilities,  By, until} = require('selenium-webdriver')
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
  let testFiles = {}

  for(file of files){
    if(checkIsDirectorySync(path.resolve(__dirname, file))){
      testFiles[path.resolve(__dirname, `./${file}/test.html`)] = file

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

const colors = {
  reset: "\x1b[0m",
  red: "\x1b[31m",
  green: "\x1b[32m",
  yellow: "\x1b[33m"
}

async function runTest(testFiles) {
    // Настройка Chrome (опционально: запуск без окна)
    let options = new chrome.Options()
    options.addArguments('--headless', '--window-size=800,600') // Раскомментировать для headless-режима

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
        for(let fileUrl in testFiles){
          await driver.get(fileUrl)
          
          await driver.wait(async () => {
            const readyState = await driver.executeScript('return document.readyState')
            return readyState === 'complete'
          }, 10000); // таймаут 10 секунд

          let containTimer = fs.readFileSync(path.resolve(__dirname, `./${testFiles[fileUrl]}/Main.qml`), 'utf-8').indexOf('Timer') >= 0

          if(containTimer) await driver.sleep(1000)

          let screenshot = await driver.takeScreenshot()
          fs.writeFileSync(fileUrl.replace('.html', '.png'), screenshot, 'base64')
          
          const logs = await driver.manage().logs().get('browser')
          
          let originMsg = fs.readFileSync(path.resolve(__dirname, `./${testFiles[fileUrl]}/log.txt`), 'utf-8')
          let messages = []

          logs.forEach(log => {
            if(log.level.name === 'INFO') {
              messages.push(`${log.message.split(' ').slice(2).join(' ').replaceAll('"', '')}`) // Удаляем первые 2 слова
            }
          })
          // console.log(originMsg, messages.join('\n'))

          //eval(`\`${originMsg}\``) === eval(`\`${messages.join('\n')}\``)

          if(eval(`\`${originMsg}\``) === eval(`\`${messages.join('\n')}\``)){
            console.log(`${colors.green}[+] ${testFiles[fileUrl]}${colors.reset}`)
          } else {
            console.log(`${colors.red}[-] ${testFiles[fileUrl]}${colors.reset}`)
          }

          fs.writeFileSync(path.resolve(__dirname, `./${testFiles[fileUrl]}/output.txt`), messages.join('\n'), 'utf-8')
          

          
        }
        
    } finally {
        await driver.quit() // Закрыть браузер
    }
}



runTest(prepareTest())