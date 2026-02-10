// tnlAssert.h conversion

pub struct Assert;

impl Assert {
    pub fn process_assert(filename: &str, line_number: u32, message: &str) {
        eprintln!("TNL ASSERT: {}:{}: {}", filename, line_number, message);
        // We could integration with Platform::debug_break here.
        #[cfg(debug_assertions)]
        crate::platform::debug_break();
        
        // In idiomatic Rust, we panic.
        panic!("TNL ASSERT: {}", message);
    }

    pub fn processing_assert() -> bool {
        // Placeholder
        false
    }
}

// Emulate avar
pub fn avar(args: std::fmt::Arguments) -> String {
    format!("{}", args)
}

#[macro_export]
macro_rules! tnl_assert {
    ($cond:expr, $($arg:tt)+) => {
        if !($cond) {
            $crate::assert::Assert::process_assert(file!(), line!(), &format!($($arg)+));
        }
    };
}
