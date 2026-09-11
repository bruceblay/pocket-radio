// Browser checks use the embedded assets with simulated scan/save responses.
// Requires Playwright + Chromium; does not verify ESP32 networking or NVS.
const {chromium} = require('playwright');
const fs = require('node:fs');
const path = require('node:path');
const assert = require('node:assert/strict');
const source = fs.readFileSync(path.join(__dirname, '../include/SetupPage.h'), 'utf8');
const assets = [...source.matchAll(/R"HTML\(([\s\S]*?)\)HTML"/g)].map(match => match[1]);
const catalog = fs.readFileSync(path.join(__dirname, '../include/stations.h'), 'utf8');
const names = [...catalog.matchAll(/\{"([^"]+)", "[^"]+", "[^"]+", "([^"]+)"/g)];
const html = assets[0] + '<h2>Connect to Wi-Fi</h2><form method="post" action="/connect"><input type="hidden" name="token" value="test">' + assets[1] + '<form id="stations-form" method="post" action="/stations"><input type="hidden" name="token" value="test"><fieldset><legend>Your stations</legend>' + names.map((m, i) => (i === 6 ? '<h3>More to explore</h3>' : '') + `<label class="station"><input type="checkbox" name="s${i}" value="1" ${i < 6 ? 'checked' : ''}><span>${m[1]}<small>${m[2]}</small></span></label>`).join('') + '</fieldset><button>Save stations</button><p id="stations-status" role="status"></p></form>' + assets[2] + '</body></html>';
(async () => {
  const browser = await chromium.launch({headless:true});
  try {
    const page = await browser.newPage({viewport:{width:390,height:844}});
    const errors = []; page.on('pageerror', error => errors.push(String(error)));
    let scanReads = 0, stationPost = '', refreshes = 0;
    await page.route('http://radio.test/**', async route => {
      const url = new URL(route.request().url());
      if (url.pathname === '/networks') return route.fulfill({json:{scanning:scanReads++ === 0, networks:scanReads === 1 ? [] : [
        {ssid:'Home Wi-Fi',rssi:-45,open:false}, {ssid:'Cafe Guest',rssi:-70,open:true},
        {ssid:'<img src=x onerror=alert(1)>',rssi:-85,open:false}
      ]}});
      if (url.pathname === '/scan') { refreshes++; return route.fulfill({status:202,body:'Scanning'}); }
      if (url.pathname === '/stations') { stationPost = route.request().postData(); return route.fulfill({json:{message:'Stations saved.'}}); }
      return route.fulfill({contentType:'text/html',body:html});
    });
    await page.goto('http://radio.test/');
    assert.equal(await page.locator('#stations-form input:checked').count(), 6);
    assert.equal(await page.locator('#stations-form input[type=checkbox]').count(), 10);
    assert.equal(await page.locator('#ssid').evaluate(el => el.getBoundingClientRect().top < document.getElementById('stations-form').getBoundingClientRect().top), true);
    await page.locator('.network').first().waitFor();
    assert.equal(await page.locator('.network').count(), 3);
    assert.equal(await page.locator('#networks img').count(), 0);
    await page.locator('#ssid').fill('home');
    assert.equal(await page.locator('.network').count(), 1);
    await page.locator('.network').click();
    assert.equal(await page.locator('#ssid').inputValue(), 'Home Wi-Fi');
    assert.equal(await page.locator('#password').evaluate(el => el === document.activeElement), true);
    await page.locator('#ssid').fill('cafe');
    await page.locator('#ssid').press('ArrowDown'); await page.locator('#ssid').press('Enter');
    assert.equal(await page.locator('#ssid').inputValue(), 'Cafe Guest');
    await page.locator('#ssid').fill('Hidden network');
    assert.equal(await page.locator('.network').count(), 0);
    await page.locator('#rescan').click();
    await page.waitForFunction(() => !document.getElementById('rescan').disabled);
    assert.equal(refreshes, 1);
    assert.equal(await page.locator('#ssid').inputValue(), 'Hidden network');
    for (const box of await page.locator('#stations-form input[type=checkbox]').all()) await box.uncheck();
    await page.getByRole('button', {name:'Save stations',exact:true}).click();
    assert.equal(await page.locator('#stations-status').textContent(), 'Choose at least one station.');
    assert.equal(stationPost, '');
    await page.locator('[name=s0]').check();
    await page.getByRole('button', {name:'Save stations',exact:true}).click();
    await page.waitForFunction(() => document.getElementById('stations-status').textContent === 'Stations saved.');
    assert.equal(new URLSearchParams(stationPost).get('s0'), '1');
    assert.equal(new URLSearchParams(stationPost).has('s1'), false);
    assert.equal(await page.locator('#ssid').inputValue(), 'Hidden network');
    assert.equal(await page.evaluate(() => document.documentElement.scrollWidth <= innerWidth), true);
    await page.locator('#ssid').fill('');
    await page.screenshot({path:'/tmp/sticks3-setup-page.png',fullPage:true});
    assert.deepEqual(errors, []);
    console.log('Setup browser checks passed (simulated network and storage endpoints).');
  } finally { await browser.close(); }
})().catch(error => {console.error(error); process.exitCode = 1;});
