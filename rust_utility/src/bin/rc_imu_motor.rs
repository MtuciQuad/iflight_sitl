use gz::transport::Node;
use gz::msgs::float_v::Float_V;
use gz::msgs::imu::IMU;
use std::{thread::sleep, time::Duration};

fn main() {
    let mut node = Node::new().unwrap();
    assert!(node.subscribe("/RcData", |msg: Float_V| {
        let data = msg.data;
        println!("{:?}", data);
    }));
    assert!(node.subscribe("/imu", |msg: IMU| {
        let gx = -msg.angular_velocity.as_ref().unwrap().x;
        let gy = -msg.angular_velocity.as_ref().unwrap().y;
        let gz = -msg.angular_velocity.as_ref().unwrap().z;
        let ax = msg.linear_acceleration.as_ref().unwrap().x;
        let ay = msg.linear_acceleration.as_ref().unwrap().y;
        let az = msg.linear_acceleration.as_ref().unwrap().z;
        println!("A: ({:.2} {:.2} {:.2}). G: ({:.2} {:.2} {:.2})", ax, ay, az, gx, gy, gz);
    }));

    let mut publisher: gz::transport::Publisher<Float_V> = node.advertise("MotorData").unwrap();

    loop {
        let v: Vec<f32> = vec![2.0, 2.0, 2.0, 2.0];
        let topic = Float_V {
            data: v,
            ..Default::default()
        };

        assert!(publisher.publish(&topic));
        sleep(Duration::from_millis(100));
    }
    // gz::transport::wait_for_shutdown();
}