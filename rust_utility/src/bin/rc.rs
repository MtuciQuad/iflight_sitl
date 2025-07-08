use hidapi;
use gz::{msgs::float_v::Float_V, transport::Node};
use std::time::Duration;

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
    let api = hidapi::HidApi::new().unwrap();
    let (vid, pid) = (4617, 20308);
    let device = api.open(vid, pid).unwrap();
    let mut buf = [0u8; 19];

    // Node for publishing rc data on RcData topic
    let mut node = Node::new().unwrap();
    let mut publisher: gz::transport::Publisher<Float_V> = node.advertise("RcData").unwrap();

    loop {
        let _res = device.read(&mut buf[..]).unwrap();
        let data = process_data(&buf);
        let topic = Float_V {
            data: data.to_vec(),
            ..Default::default()
        };
        // Publish the rc data
        assert!(publisher.publish(&topic));
        std::thread::sleep(Duration::from_micros(250));
    }

}