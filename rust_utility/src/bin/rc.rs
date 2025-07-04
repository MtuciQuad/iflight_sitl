use hidapi;
use gz::{msgs::float_v::Float_V, transport::Node};
use std::time::Duration;

fn process_data(raw_data: &[u8;19]) -> [f32; 8] {
    let mut data = [0f32; 8];

    for (i, ch) in raw_data[3..].chunks(2).enumerate() {
        data[i] = u16::from_le_bytes(ch.try_into().unwrap()) as f32 / 2047.0 * 100.0;
    }

    // println!("{:?}", data);
    data
}

fn main() {
    let api = hidapi::HidApi::new().unwrap();

    for device in api.device_list() {
        println!("{:#?}", device);
    }
    let (vid, pid) = (4617, 20308);
    let device = api.open(vid, pid).unwrap();
    let mut buf = [0u8; 19];

    let mut node = Node::new().unwrap();
    let mut publisher: gz::transport::Publisher<Float_V> = node.advertise("MotorData").unwrap();
    // let mut publisher: gz::transport::Publisher<Float_V> = node.advertise("RcData").unwrap();

    loop {
        let _res = device.read(&mut buf[..]).unwrap();
        // println!("Read: {:?}", &buf[..res]);
        let data = process_data(&buf);
        
        let throttle = data[2];

        let yaw = (data[3] - 50.0) / 50.0;
        let roll = (data[0] - 50.0) / 200.0;
        let pitch = (data[1] - 50.0) / 200.0;

        let mut ky1 = 1.0;
        let mut ky2 = 1.0;
        if yaw < 0.0 {
            ky2 = 1.0 + yaw;
            if ky2 < 0.07 {
                ky2 = 0.07;
            }
        } 
        else {
            ky1 = 1.0 - yaw;
            if ky1 < 0.07 {
                ky1 = 0.07;
            }
        }

        let mut mk1 = 1.0;
        let mut mk2 = 1.0;
        let mut mk3 = 1.0;
        let mut mk4 = 1.0;

        mk1 = mk1 + pitch + roll;
        mk2 = mk2 + pitch - roll;
        mk3 = mk3 - pitch - roll;
        mk4 = mk4 - pitch + roll;

        println!("{:?} {:?} {:?} {:?}", mk1, mk2, mk3, mk4);

        let m1 = throttle * ky1 * mk1 / 100.0 * 4.0;
        let m2 = throttle * ky2 * mk2 / 100.0 * 4.0;
        let m3 = throttle * ky1 * mk3 / 100.0 * 4.0;
        let m4 = throttle * ky2 * mk4 / 100.0 * 4.0;


        // println!("{:?} {:?} {:?} {:?}", m1, m2, m3, m4);
        
        let mf = data[2] / 100.0 * 4.0;
        // let mf = 4.0;
        // let v: Vec<f32> = vec![mf, mf, mf, mf];
        let v: Vec<f32> = vec![m1, m2, m3, m4];
        let topic = Float_V {
            data: v,
            ..Default::default()
        };
        // let topic = Float_V {
        //     data: data.to_vec(),
        //     ..Default::default()
        // };
        assert!(publisher.publish(&topic));
        std::thread::sleep(Duration::from_micros(250));
    }

}