// Terminal page for the TobiOS wasm build. Everything printed here comes from the compiled
// C++ through ttyWrite. This file only plays the part of a tty line discipline.
const $ = (id) => document.getElementById(id);
const stateEl = $('state');
const failEl = $('fail');
const srLog = $('sr-log');

// The terminal window is always dark, as part of the page's retro desktop look.
const THEME = {
  background: '#14171a', foreground: '#d7f5e1', cursor: '#6ee7a0', cursorAccent: '#14171a', selectionBackground: '#2f4a3b',
  black: '#14171a', white: '#d7f5e1', brightWhite: '#ffffff', brightBlack: '#8a958f',
  red: '#f87171', green: '#6ee7a0', yellow: '#facc15', blue: '#7dd3fc', magenta: '#e879f9', cyan: '#67e8f9',
};
const narrow = matchMedia('(max-width: 560px)');
const reducedMotion = matchMedia('(prefers-reduced-motion: reduce)').matches;
const FONT = '"IBM Plex Mono", ui-monospace, "Cascadia Mono", "SF Mono", Menlo, Consolas, "DejaVu Sans Mono", monospace';

// xterm measures the character cell when it opens, so give the web font a moment to arrive first.
if (document.fonts?.load) {
  await Promise.race([
    Promise.all([document.fonts.load('400 14px "IBM Plex Mono"'), document.fonts.load('700 14px "IBM Plex Mono"')]).catch(() => {}),
    new Promise((r) => setTimeout(r, 2500)),
  ]);
}

const term = new Terminal({
  convertEol: true,
  cursorBlink: !reducedMotion,
  fontFamily: FONT,
  fontSize: narrow.matches ? 11 : 14,
  lineHeight: 1.15,
  scrollback: 2000,
  theme: THEME,
});
const fit = new FitAddon.FitAddon();
term.loadAddon(fit);
term.open($('term'));
fit.fit();
new ResizeObserver(() => fit.fit()).observe($('term'));
// Tab should move focus out of the terminal instead of being swallowed by it.
term.attachCustomKeyEventHandler((e) => e.key !== 'Tab');

// --- stdin ----------------------------------------------------------------------------
const encoder = new TextEncoder();
let session = 0;         // bumps on reboot, so a stale instance can no longer read input
let running = false;
let waitingForInput = null;
let queued = [];         // finished lines the program has not read yet
let unread = null;       // bytes left over when a line did not fit the read buffer
let lineBuf = '';
const history = [];
let historyPos = 0;

function ttyRead(forSession) {
  return new Promise((resolve) => {
    if (forSession !== session) return; // an old instance stays parked and gets collected
    if (unread) { const b = unread; unread = null; resolve(b); return; }
    if (queued.length) { resolve(queued.shift()); return; }
    waitingForInput = resolve;
    setState('waiting for input');
  });
}

function submitLine(line) {
  const bytes = encoder.encode(line + '\n');
  if (line.trim()) { history.push(line); historyPos = history.length; }
  if (waitingForInput) {
    const resolve = waitingForInput;
    waitingForInput = null;
    setState('running');
    resolve(bytes);
  } else {
    queued.push(bytes);
  }
}

function setState(text) { stateEl.textContent = text; }

// --- stdout ---------------------------------------------------------------------------
const decoder = new TextDecoder();
let srText = '';
let srTimer = 0;
function announce(bytes) {
  srText += decoder.decode(bytes, { stream: true });
  clearTimeout(srTimer);
  srTimer = setTimeout(() => {
    srLog.textContent = srText.replace(/\x1b\[[0-9;]*m/g, '').slice(-1200);
    srText = '';
  }, 150);
}

// --- keyboard -------------------------------------------------------------------------
function replaceLine(text) {
  term.write('\b \b'.repeat(lineBuf.length) + text);
  lineBuf = text;
}

term.onData((data) => {
  if (!running) {
    if (data.includes('\r')) boot();
    return;
  }
  if (data === '\x1b[A') {            // up
    if (historyPos > 0) replaceLine(history[--historyPos]);
    return;
  }
  if (data === '\x1b[B') {            // down
    if (historyPos < history.length) {
      historyPos += 1;
      replaceLine(history[historyPos] ?? '');
    }
    return;
  }
  if (data.startsWith('\x1b')) return; // other escape sequences have no meaning at this tty
  for (const ch of data) {
    if (ch === '\r' || ch === '\n') {
      term.write('\r\n');
      const line = lineBuf;
      lineBuf = '';
      submitLine(line);
    } else if (ch === '\x7f' || ch === '\b') {
      if (lineBuf.length) {
        lineBuf = lineBuf.slice(0, -1);
        term.write('\b \b');
      }
    } else if (ch === '\x03') {        // ctrl-c: drop the line, as a cooked tty would
      term.write('^C\r\n$ ');
      lineBuf = '';
    } else if (ch >= ' ') {
      lineBuf += ch;
      term.write(ch);
    }
  }
});

function runFromPage(command) {
  if (!running) boot();
  replaceLine('');
  queueMicrotask(() => {
    term.write(command + '\r\n');
    submitLine(command);
  });
}

$('cmdline').addEventListener('submit', (e) => {
  e.preventDefault();
  const input = $('cmd');
  const value = input.value;
  input.value = '';
  runFromPage(value);
});
document.querySelector('.chips').addEventListener('click', (e) => {
  const button = e.target.closest('button[data-cmd]');
  if (button) runFromPage(button.dataset.cmd);
});
$('reboot').addEventListener('click', () => { boot(); term.focus(); });

// --- boot -----------------------------------------------------------------------------
let createTobiOS = null;

async function boot() {
  session += 1;
  const mine = session;
  waitingForInput = null;
  queued = [];
  unread = null;
  lineBuf = '';
  running = true;
  term.reset();
  setState('booting');
  try {
    createTobiOS ??= (await import('./tobios.mjs')).default;
    const program = await createTobiOS({
      noInitialRun: true,
      ttyWrite: (fd, bytes) => {
        if (mine !== session) return;
        term.write(bytes);
        announce(bytes);
      },
      ttyRead: () => ttyRead(mine),
      ttyUnread: (bytes) => { unread = bytes.slice(); },
    });
    // main() suspends on stdin, so the async call is what reports its real return value.
    const code = await program.ccall('main', 'number', [], [], { async: true });
    if (mine !== session) return;
    running = false;
    setState(`exited with code ${code}`);
    term.write(`\r\n[process exited with code ${code}. Press Enter or Reboot to start again.]\r\n`);
  } catch (err) {
    if (mine !== session) return;
    running = false;
    setState('failed');
    failEl.hidden = false;
    failEl.textContent = `The WebAssembly build could not start: ${err.message}`;
  }
}

if (typeof WebAssembly !== 'object') {
  setState('unavailable');
  failEl.hidden = false;
  failEl.textContent = 'This browser does not support WebAssembly, which the terminal needs.';
} else {
  boot();
  if (!narrow.matches) term.focus();
}
