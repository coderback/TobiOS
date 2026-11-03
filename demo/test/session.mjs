// Drives the real wasm build through stdin/stdout in Node, with the same hooks the page uses.
import { fileURLToPath, pathToFileURL } from 'node:url';
import path from 'node:path';
import assert from 'node:assert/strict';

const dist = path.join(path.dirname(fileURLToPath(import.meta.url)), '..', 'dist');
const { default: createTobiOS } = await import(pathToFileURL(path.join(dist, 'tobios.mjs')).href);

const script = [
  'help', 'status', 'ls /', 'ls /home', 'cat /home/welcome.txt', 'ps',
  'mkdir /home/visitor', 'touch /home/visitor/notes.txt', 'ls /home/visitor',
  'cat /home/visitor/notes.txt', 'nonsense', 'mkdir /nope/deep', '', 'exit',
];
const td = new TextDecoder();
let out = '';
let pending = new Uint8Array();
let i = 0;

const program = await createTobiOS({
  noInitialRun: true,
  ttyWrite: (fd, bytes) => { out += td.decode(bytes, { stream: true }); },
  ttyUnread: (b) => { pending = b; },
  ttyRead: async () => {
    if (pending.length) { const b = pending; pending = new Uint8Array(); return b; }
    await new Promise((r) => setTimeout(r, 5));
    assert.ok(out.endsWith('$ '), 'prompt must be flushed before the program blocks on stdin: ' + JSON.stringify(out.slice(-20)));
    const line = script[i++];
    out += `<<${line}>>\n`;
    return new TextEncoder().encode(line + '\n');
  },
});
const code = await program.ccall('main', 'number', [], [], { async: true });

console.log(out);
assert.equal(code, 0);
for (const needle of [
  'TobiOS v1.0', 'TobiOS boot sequence completed successfully', '=== TobiOS Ready ===',
  'ls - List directory contents', 'Memory Manager: Initialized', 'welcome.txt',
  'Welcome to TobiOS!', 'init', 'READY', 'Directory created: /home/visitor',
  'File created: /home/visitor/notes.txt', 'notes.txt', 'File not found or empty: /home/visitor/notes.txt',
  'Unknown command: nonsense', 'Failed to create directory: /nope/deep',
  'Exiting shell...', 'TobiOS has been shut down. Goodbye!',
]) assert.ok(out.includes(needle), 'missing: ' + needle);
console.log('OK: exit code 0 and all expected output present');
