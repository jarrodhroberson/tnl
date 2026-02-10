// tnlPlatform.h conversion
use crate::types::*;
use std::time::Instant;
use std::sync::OnceLock;

static START_TIME: OnceLock<Instant> = OnceLock::new();

fn get_start_time() -> &'static Instant {
    START_TIME.get_or_init(Instant::now)
}

pub fn get_real_milliseconds() -> u32 {
    get_start_time().elapsed().as_millis() as u32
}

pub fn get_high_precision_timer_value() -> u64 {
    // Using microseconds for precision
    get_start_time().elapsed().as_micros() as u64
}

pub fn get_high_precision_milliseconds(timer_delta: u64) -> f64 {
    // timer_delta is in micros
    timer_delta as f64 / 1000.0
}

pub fn output_debug_string(s: &str) {
    eprintln!("[DEBUG]: {}", s);
}

pub fn debug_break() {
    // Use inline assembly for a debug break on supported platforms
    #[cfg(any(target_arch = "x86", target_arch = "x86_64"))]
    unsafe {
        std::arch::asm!("int3");
    }
    #[cfg(not(any(target_arch = "x86", target_arch = "x86_64")))]
    {
        // Fallback for other architectures or if asm is not available
        eprintln!("Debug break hit (asm not supported on this arch)");
    }
}

pub fn force_quit() {
    std::process::exit(1);
}

pub fn alert_ok(title: &str, message: &str) {
    // In a GUI app this would be a popup.
    eprintln!("ALERT [{}]: {}", title, message);
}

pub fn alert_ok_cancel(title: &str, message: &str) -> bool {
    eprintln!("ALERT [{}]: {} (OK/Cancel - defaulting to OK)", title, message);
    true
}

pub fn alert_retry(title: &str, message: &str) -> bool {
    eprintln!("ALERT [{}]: {} (Retry/Cancel - defaulting to Retry)", title, message);
    true
}

pub fn sleep(ms_count: u32) {
    std::thread::sleep(std::time::Duration::from_millis(ms_count as u64));
}

pub fn check_heap() -> bool {
    // Rust manages heap safely (mostly).
    true
}

// Helpers
pub fn d_sprintf(buffer: &mut [u8], format: &str) {
    // Hard to replicate dSprintf C varargs in Rust directly without macros.
    // Usually users should use format!.
    // We'll stub this or provide a macro.
}
