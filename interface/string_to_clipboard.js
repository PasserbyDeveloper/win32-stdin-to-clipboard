// @ts-check
const cp = require("child_process");

const native_application_path = "../build/stdin-to-clipboard.exe";

/**
 * Set the global clipboard string
 * @param {string} text
 * @param {boolean} [debug] default false so that no extra information is printed to stdout
 * @returns {Promise<{success: true} | {error: true, message: string}>}
 */
function string_to_clipboard(text, debug = false) {
	return new Promise((resolve, reject) => {
		const child = cp.spawn(
			native_application_path,
			[],
			{
				stdio: [
					"pipe",
					debug === false ? "ignore" : process.stdout,
					"pipe"
				]
			}
		);

		const ok_func = function(code, sig) {
			if (code === 0 && sig === null) {
				resolve({success: true});
			} else {
				reject({error: true, message: "unexpected exit state", arguments: [...arguments]});
			}
		};

		child.on("exit", ok_func);
		child.on("close", ok_func);
		child.on("error", (err) => reject({error: true, message: err.message, stack: err.stack}));
		child.stderr.on("data", (message) => reject({error: true, message: message, arguments: [...arguments]}));
		child.stdin.write(text);
		child.stdin.end();
	});
}

// string_to_clipboard("yes");

module.exports = string_to_clipboard;