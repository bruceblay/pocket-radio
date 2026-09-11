#pragma once

static const char setupPageStart[] PROGMEM = R"HTML(<!doctype html><html lang="en"><meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1"><title>Pocket Radio setup</title><style>
body{background:#10110f;color:#f3df69;font:17px system-ui,sans-serif;max-width:440px;margin:32px auto;padding:0 22px}h1{font-size:36px;margin-bottom:8px}h2{font-size:24px;margin-top:32px}p{line-height:1.5}label{display:block;margin:22px 0 8px}input,button{box-sizing:border-box;width:100%;padding:14px;font:inherit;border-radius:8px;border:1px solid #82783f}input{background:#20211b;color:#fff}button{background:#f3df69;color:#10110f;font-weight:bold;margin-top:20px;cursor:pointer}button:disabled{opacity:.5}a{color:inherit}.note{font-size:14px;color:#cac6a6}.status{padding:14px;border:1px solid #82783f;border-radius:8px}.station{display:flex;gap:14px;align-items:center;padding:12px;border:1px solid #454638;border-radius:8px;margin:8px 0}.station input{width:22px;height:22px;flex-shrink:0;accent-color:#f3df69}.station small{display:block;font-size:12px;color:#cac6a6;margin-top:4px}.secondary{background:transparent;color:#f3df69}.networks{max-height:225px;overflow:auto;margin-top:6px}.network{display:flex;justify-content:space-between;gap:10px;text-align:left;margin:3px 0;background:#20211b;color:#fff;font-weight:normal;overflow-wrap:anywhere}.network small{color:#cac6a6;white-space:nowrap}.network[aria-selected=true]{outline:2px solid #f3df69;outline-offset:-2px}.row{display:flex;gap:12px;align-items:center}.row button{width:auto;padding:8px;margin:0;font-size:14px}fieldset{border:0;padding:0;margin:0}legend{font-size:24px;font-weight:bold;margin:28px 0 12px}</style><body><p>YOUR LITTLE INTERNET RADIO</p><h1>Make it yours.</h1>)HTML";

static const char setupNetworkFields[] PROGMEM = R"HTML(<label for="ssid">Wi-Fi network</label><input id="ssid" name="ssid" maxlength="32" required autocapitalize="none" spellcheck="false" autocomplete="off" role="combobox" aria-autocomplete="list" aria-controls="networks" aria-expanded="false"><div id="networks" class="networks" role="listbox" aria-label="Nearby Wi-Fi networks"></div><div class="row"><p id="scan-status" class="note" role="status">Looking for nearby networks…</p><button id="rescan" type="button" class="secondary">Refresh</button></div><noscript><p class="note">Type your network name above. Enable JavaScript to see nearby networks.</p></noscript><label for="password">Wi-Fi password</label><input id="password" name="password" type="password" maxlength="64" autocomplete="new-password"><p class="note">Choose a 2.4 GHz network, or type a hidden network's name. Leave the password empty for an open network. Hotel sign-in pages and enterprise Wi-Fi aren't supported.</p><button>Connect &amp; play</button></form>)HTML";

static const char setupPageScript[] PROGMEM = R"HTML(<script>
(() => {
  const input = document.getElementById('ssid');
  if (!input) return;
  const list = document.getElementById('networks');
  const status = document.getElementById('scan-status');
  const refresh = document.getElementById('rescan');
  let networks = [], visible = [], active = -1, timer, polls = 0;
  function choose(network) {
    input.value = network.ssid;
    list.replaceChildren(); input.setAttribute('aria-expanded', 'false');
    input.removeAttribute('aria-activedescendant');
    document.getElementById('password').focus();
  }
  function render() {
    list.replaceChildren(); active = -1; input.removeAttribute('aria-activedescendant');
    visible = networks.filter(n => n.ssid.toLowerCase().includes(input.value.toLowerCase()));
    visible.forEach((network, index) => {
      const option = document.createElement('button');
      option.type = 'button'; option.className = 'network'; option.id = 'network-' + index;
      option.setAttribute('role', 'option'); option.setAttribute('aria-selected', 'false');
      const name = document.createElement('span'); name.textContent = network.ssid;
      const detail = document.createElement('small');
      detail.textContent = (network.open ? 'Open · ' : '') + (network.rssi > -60 ? 'Strong' : network.rssi > -75 ? 'Good' : 'Weak');
      option.append(name, detail); option.addEventListener('click', () => choose(network)); list.append(option);
    });
    input.setAttribute('aria-expanded', visible.length ? 'true' : 'false');
    if (networks.length) status.textContent = visible.length ? 'Tap a network, or keep typing to filter.' : 'No match. You can enter the name manually.';
  }
  input.addEventListener('input', render); input.addEventListener('focus', render);
  input.addEventListener('keydown', event => {
    if (event.key === 'Escape') { list.replaceChildren(); input.setAttribute('aria-expanded', 'false'); input.removeAttribute('aria-activedescendant'); active = -1; return; }
    if (event.key === 'Enter' && active >= 0) { event.preventDefault(); choose(visible[active]); return; }
    if (!['ArrowDown', 'ArrowUp'].includes(event.key) || !visible.length) return;
    event.preventDefault(); if (!list.children.length) render();
    active = (active + (event.key === 'ArrowDown' ? 1 : visible.length - 1) + visible.length) % visible.length;
    Array.from(list.children).forEach((option, i) => option.setAttribute('aria-selected', String(i === active)));
    input.setAttribute('aria-activedescendant', 'network-' + active); list.children[active].scrollIntoView({block:'nearest'});
  });
  async function scan(rescan = false) {
    clearTimeout(timer); refresh.disabled = true;
    try {
      if (rescan) {
        polls = 0;
        const response = await fetch('/scan', {method:'POST', body:new URLSearchParams({token:input.form.elements.token.value})});
        if (!response.ok) throw Error();
      }
      const response = await fetch('/networks', {cache:'no-store'});
      if (!response.ok) throw Error();
      const data = await response.json();
      if (data.scanning && polls++ < 20) {
        status.textContent = 'Looking for nearby networks…'; timer = setTimeout(() => scan(), 1000); return;
      }
      networks = data.networks; render();
      if (!networks.length) status.textContent = 'No networks found. Refresh or enter the name manually.';
    } catch (_) { status.textContent = 'Could not load networks. Refresh or enter the name manually.'; }
    refresh.disabled = false;
  }
  refresh.addEventListener('click', () => scan(true)); scan();
  const stations = document.getElementById('stations-form');
  stations.addEventListener('submit', async event => {
    event.preventDefault();
    const feedback = document.getElementById('stations-status');
    if (!stations.querySelector('input[type=checkbox]:checked')) { feedback.textContent = 'Choose at least one station.'; return; }
    const button = stations.querySelector('button'); button.disabled = true;
    try {
      const response = await fetch('/stations', {method:'POST', body:new URLSearchParams(new FormData(stations)), headers:{'Accept':'application/json'}});
      const result = await response.json(); feedback.textContent = result.message;
    } catch (_) { feedback.textContent = 'Could not save. Check your connection to the radio and try again.'; }
    button.disabled = false;
  });
})();
</script>)HTML";
