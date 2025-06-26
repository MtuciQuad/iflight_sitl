use std::{thread::sleep, time::Duration};

use gz::{msgs::float_v::Float_V, transport::Node};

fn main() {
    let mut node = Node::new().unwrap();
    let mut publisher: gz::transport::Publisher<Float_V> = node.advertise("MotorData").unwrap();

    for i in 0..100 {
        let v: Vec<f32> = vec![1.0, 2.0, 9.67, 98.7];
        let topic = Float_V {
            data: v,
            ..Default::default()
        };

        assert!(publisher.publish(&topic));
        sleep(Duration::from_millis(100));
    }
}