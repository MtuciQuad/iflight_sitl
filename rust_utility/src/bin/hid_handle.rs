use hidapi;
use std::time::Duration;

pub enum ControlState {
    Test,
    Acro,
    Stab,
}

pub struct Controls {
    pub thr: f32,
    pub pch: f32,
    pub rll: f32,
    pub yaw: f32,
    pub arm: bool,
    pub state: ControlState,
    pub raw: [f32; 16],
}

fn process_data(raw_data: &[u8;19]) {
    let mut data = [0f32; 8];

    for (i, ch) in raw_data[3..].chunks(2).enumerate() {
        data[i] = u16::from_le_bytes(ch.try_into().unwrap()) as f32 / 2047.0;    
    }
    println!("{:?}", data);

//     Controls {
//         thr: (),
//         pch: (),
//         rll: (),
//         yaw: (),
//         arm: (),
//         state: (),
//         raw: ()
//     }
}

fn main() {
    let api = hidapi::HidApi::new().unwrap();

    for device in api.device_list() {
        println!("{:#?}", device);
    }
    let (VID, PID) = (4617, 20308);
    let device = api.open(VID, PID).unwrap();
    let mut buf = [0u8; 19];
    loop {
        let res = device.read(&mut buf[..]).unwrap();
        // println!("Read: {:?}", &buf[..res]);
        process_data(&buf);
        // std::thread::sleep(Duration::from_secs(1));
    }

}