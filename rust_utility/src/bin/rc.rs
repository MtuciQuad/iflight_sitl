use hidapi;
use gz::{msgs::float_v::Float_V, transport::Node};
use std::{thread::sleep, time::Duration};

fn process_data(raw_data: &[u8;19]) -> [f32; 8] {
    let mut data = [0f32; 8];

    for (i, ch) in raw_data[3..].chunks(2).enumerate() {
        data[i] = u16::from_le_bytes(ch.try_into().unwrap()) as f32 / 2047.0 * 100.0;
    }

    println!("{:?}", data);
    data
}

fn main() {
    let api = hidapi::HidApi::new().unwrap();

    for device in api.device_list() {
        println!("{:#?}", device);
    }
    let (VID, PID) = (4617, 20308);
    let device = api.open(VID, PID).unwrap();
    let mut buf = [0u8; 19];

    let mut node = Node::new().unwrap();
    let mut publisher: gz::transport::Publisher<Float_V> = node.advertise("MotorData").unwrap();

    loop {
        let res = device.read(&mut buf[..]).unwrap();
        // println!("Read: {:?}", &buf[..res]);
        let data = process_data(&buf);
        
        let mf = data[2] * 4.0;
        // let mf = 4.0;
        let v: Vec<f32> = vec![mf, mf, mf, mf];
        let topic = Float_V {
            data: v,
            ..Default::default()
        };
        assert!(publisher.publish(&topic));
        std::thread::sleep(Duration::from_micros(250));
    }

}