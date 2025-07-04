use gz::transport::Node;
use gz::msgs::imu::IMU;

fn main() {
    let mut node = Node::new().unwrap();
    assert!(node.subscribe("/imu", |msg: IMU| {
        let gx = -msg.angular_velocity.as_ref().unwrap().x;
        let gy = -msg.angular_velocity.as_ref().unwrap().y;
        let gz = -msg.angular_velocity.as_ref().unwrap().z;
        let ax = msg.linear_acceleration.as_ref().unwrap().x;
        let ay = msg.linear_acceleration.as_ref().unwrap().y;
        let az = msg.linear_acceleration.as_ref().unwrap().z;
        println!("A: ({:.2} {:.2} {:.2}). G: ({:.2} {:.2} {:.2})", ax, ay, az, gx, gy, gz);
    }));
    gz::transport::wait_for_shutdown();
}