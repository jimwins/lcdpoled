# lcdpoled

[![Crates.io](https://img.shields.io/crates/v/lcdpoled.svg)](https://crates.io/crates/lcdpoled)
[![Docs.rs](https://docs.rs/lcdpoled/badge.svg)](https://docs.rs/lcdpoled)
[![CI](https://github.com/jimwins/lcdpoled/workflows/CI/badge.svg)](https://github.com/jimwins/lcdpoled/actions)

This utility knows how to push text to a Logic Controls line display device (also
known as a pole display).

It uses [rusb](https://github.com/a1ien/rusb), a safe Rust wrapper for libusb,
so it should work on Linux, macOS, and Windows, although only Linux has been
tested.

## Installation

### Cargo

* Install the rust toolchain in order to have cargo installed by following
  [this](https://www.rust-lang.org/tools/install) guide.
* run `cargo install lcdpoled`

## License

Licensed under either of

 * Apache License, Version 2.0
   ([LICENSE-APACHE](LICENSE-APACHE) or http://www.apache.org/licenses/LICENSE-2.0)
 * MIT license
   ([LICENSE-MIT](LICENSE-MIT) or http://opensource.org/licenses/MIT)

at your option.

Please note that libusb is licensed under the LGPL, so depending on how you
build and link `lcdpoled` you may have some obligations under that license.

## Contribution

Unless you explicitly state otherwise, any contribution intentionally submitted
for inclusion in the work by you, as defined in the Apache-2.0 license, shall be
dual licensed as above, without any additional terms or conditions.

See [CONTRIBUTING.md](CONTRIBUTING.md).
