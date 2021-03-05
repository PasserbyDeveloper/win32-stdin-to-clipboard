# Stdin to Clipboard

This is an open source project that compiles a native windows console application to set the global windows clipboard into the data read in the standard input.

it uses Win32 API, such as `OpenClipboard`, `GlobalAlloc`, `SetClipboardData`, `CloseClipboard`, etc.

## Compiling

Compiling is done by the `compile-windows-loop.bat` batch script.

## Usage

1. Run the `stdin-to-clipboard.exe` process.
2. Send a string to stdin, such as "hello world".
3. Close the stream by any means, such as pressing Ctrl+C.
4. Program will exit. If the exit code was 0 then your global clipboard should contain the "hello world" string. Otherwise check stdout and stderr for details.
5. Test it by pressing Ctrl+V in a text editor.

## NodeJS Interface

The file `./interface/string_to_clipboard.js` exposes a single NodeJS asyncronous function that depends on `child_process` module to operate the native application to achieve its goal

```js
/**
 * Set the global clipboard string
 * @param {string} text
 * @param {boolean} [debug] default false, no extra information is printed to stdout
 * @returns {Promise<{success: true} | {error: true, message: string}>}
 */
async function string_to_clipboard(text, debug = false) {
	// implementation hidden
}
```

