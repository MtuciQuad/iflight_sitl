use hidapi::{HidApi, HidResult};
use gz::{msgs::float_v::Float_V, transport::Node};
use std::time::Duration;
use std::process::Command;

// Data indexes:
// 0 - roll [0..100]
// 1 - pitch [0..100]
// 2 - throttle [0..100]
// 3 - yaw [0..100]
// 4 - motor switch (0, 100)
// 5 - arm (0, 50, 100)
// 6 - flight mode (0, 50, 100)
// 7 - reboot (0, 100)


// Processes and normalizes data
fn process_data(raw_data: &[u8;19]) -> [f32; 8] {
    let mut data = [0f32; 8];
    for (i, ch) in raw_data[3..].chunks(2).enumerate() {
        data[i] = u16::from_le_bytes(ch.try_into().unwrap()) as f32 / 2047.0 * 100.0;
    }
    data
}

fn main() {
    let mut api = HidApi::new().unwrap();
    let (vid, pid) = (4617, 20308);
    let mut buf = [0u8; 19];
    let mut node = Node::new().unwrap();
    let mut publisher: gz::transport::Publisher<Float_V> = node.advertise("RcData").unwrap();
    loop {
        if let HidResult::Ok(device) = api.open(vid, pid) {
            println!("Device was connected");
            let mut last_sd = 0u8;
            loop {
                if let Err(_) =  device.read(&mut buf[..]) {
                    println!("Device was disconnected");
                    break
                }
                // let _res = device.read(&mut buf[..]).unwrap();
                let data = process_data(&buf);
                let topic = Float_V {
                    data: data.to_vec(),
                    ..Default::default()
                };
                if last_sd == 0 && data[7] != 0.0 {
                    Command::new("gz")
                    .args(["service", "-s", "/world/iflight_world/control", "--reqtype", "gz.msgs.WorldControl", "--reptype", "gz.msgs.Boolean", "--req", "reset: {all: true}"])
                    .output().expect("Error");
                }
                // Publish the rc data
                assert!(publisher.publish(&topic));
                last_sd = data[7] as u8;
                api.refresh_devices();
                std::thread::sleep(Duration::from_micros(250));
            }
        }
    }
}