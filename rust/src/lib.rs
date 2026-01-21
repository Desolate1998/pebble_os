#![no_std]
#![no_main]
use core::slice;
use core::panic::PanicInfo;

extern "C" {
    // C shim that calls into the C++ terminal::clear()
    fn terminal_clear();
    // C shim to print a byte buffer (not necessarily NUL-terminated)
    fn terminal_print_ptr(ptr: *const u8, len: usize);
}

#[panic_handler]
fn panic(_info: &PanicInfo) -> ! {
    loop {}
}

fn trim_ascii_whitespace(bytes: &[u8]) -> &[u8] {
    let mut start = 0usize;
    let mut end = bytes.len();
    while start < end {
        match bytes[start] {
            b' ' | b'\n' | b'\r' | b'\t' => start += 1,
            _ => break,
        }
    }
    while end > start {
        match bytes[end - 1] {
            b' ' | b'\n' | b'\r' | b'\t' => end -= 1,
            _ => break,
        }
    }
    &bytes[start..end]
}

#[no_mangle]
pub extern "C" fn handle_command(ptr: *const u8, len: usize) -> i32 {
    if ptr.is_null() || len == 0 {
        return 0;
    }
    let bytes = unsafe { slice::from_raw_parts(ptr, len) };
    let trimmed = trim_ascii_whitespace(bytes);

    // match the literal ASCII command "clear"
    if trimmed == b"clear" {
        unsafe { terminal_clear(); }
        return 1; // indicate handled
    }

    // handle `print` command: either `print` (no args) or `print <text>`
    if trimmed == b"print" {
        // print default message
        let msg = b"yo";
        unsafe { terminal_print_ptr(msg.as_ptr(), msg.len()); }
        return 1;
    }

    if trimmed.len() > 6 && &trimmed[0..6] == b"print " {
        let content = trim_ascii_whitespace(&trimmed[6..]);
        if !content.is_empty() {
            unsafe { terminal_print_ptr(content.as_ptr(), content.len()); }
            return 1;
        }
    }

    0 // not handled
}
