/*
 * Initially based on the https://github.com/a1ien/rusb/blob/master/examples/read_device.rs
 * example included with rusb.
 */

use std::time::Duration;

use rusb::{
    Context, Device, DeviceDescriptor, DeviceHandle, Direction, Result, TransferType, UsbContext,
};

#[derive(Debug)]
struct Endpoint {
    config: u8,
    iface: u8,
    setting: u8,
    address: u8,
}

fn main() {
    let args: Vec<String> = std::env::args().collect();

    if args.len() < 2 {
        println!("usage: {} <message>", args[0]);
        return;
    }

    let output = &args[1].as_str();

    /* TODO handle other product IDs: may also be 0xa030, 0xa060, or 0xa010. */
    let vid: u16 = 0x0fa8;
    let pid: u16 = 0xa090;

    match Context::new() {
        Ok(mut context) => match open_device(&mut context, vid, pid) {
            Some((mut device, device_desc, mut handle)) => {
                write_device(&mut device, &device_desc, &mut handle, output).unwrap()
            }
            None => println!("could not find device {:04x}:{:04x}", vid, pid),
        },
        Err(e) => panic!("could not initialize libusb: {}", e),
    }
}

fn open_device<T: UsbContext>(
    context: &mut T,
    vid: u16,
    pid: u16,
) -> Option<(Device<T>, DeviceDescriptor, DeviceHandle<T>)> {
    let devices = match context.devices() {
        Ok(d) => d,
        Err(_) => return None,
    };

    for device in devices.iter() {
        let device_desc = match device.device_descriptor() {
            Ok(d) => d,
            Err(_) => continue,
        };

        if device_desc.vendor_id() == vid && device_desc.product_id() == pid {
            match device.open() {
                Ok(handle) => return Some((device, device_desc, handle)),
                Err(e) => panic!("Device found but failed to open: {}", e),
            }
        }
    }

    None
}

fn write_device<T: UsbContext>(
    device: &mut Device<T>,
    device_desc: &DeviceDescriptor,
    handle: &mut DeviceHandle<T>,
    output: &str,
) -> Result<()> {
    handle.reset()?;

    match find_endpoint(device, device_desc, Direction::Out, TransferType::Bulk) {
        Some(endpoint) => write_endpoint(handle, endpoint, output),
        None => println!("No writable endpoint"),
    }

    Ok(())
}

fn find_endpoint<T: UsbContext>(
    device: &mut Device<T>,
    device_desc: &DeviceDescriptor,
    direction: Direction,
    transfer_type: TransferType,
) -> Option<Endpoint> {
    for n in 0..device_desc.num_configurations() {
        let config_desc = match device.config_descriptor(n) {
            Ok(c) => c,
            Err(_) => continue,
        };

        for interface in config_desc.interfaces() {
            for interface_desc in interface.descriptors() {
                for endpoint_desc in interface_desc.endpoint_descriptors() {
                    if endpoint_desc.direction() == direction
                        && endpoint_desc.transfer_type() == transfer_type
                    {
                        return Some(Endpoint {
                            config: config_desc.number(),
                            iface: interface_desc.interface_number(),
                            setting: interface_desc.setting_number(),
                            address: endpoint_desc.address(),
                        });
                    }
                }
            }
        }
    }

    None
}

fn write_endpoint<T: UsbContext>(handle: &mut DeviceHandle<T>, endpoint: Endpoint, output: &str) {
    match configure_endpoint(handle, &endpoint) {
        Ok(_) => {
            let timeout = Duration::from_secs(1);

            match handle.write_bulk(endpoint.address, output.as_bytes(), timeout) {
                Ok(_) => {}
                Err(err) => println!("could not write to display: {}", err),
            }
        }
        Err(err) => println!("could not configure display: {}", err),
    }
}

fn configure_endpoint<T: UsbContext>(
    handle: &mut DeviceHandle<T>,
    endpoint: &Endpoint,
) -> Result<()> {
    handle.set_active_configuration(endpoint.config)?;
    handle.claim_interface(endpoint.iface)?;
    handle.set_alternate_setting(endpoint.iface, endpoint.setting)?;
    Ok(())
}
