use std::{thread::sleep, time::Duration};

use gz::{msgs::{entity::Entity, entity_wrench::EntityWrench, float_v::Float_V, pose::Pose, protobuf::{EnumOrUnknown, MessageField}, vector3d::Vector3d, wrench::Wrench}, transport::Node};
use nalgebra::{Quaternion, UnitQuaternion};

fn main() {
    let mut node = Node::new().unwrap();
    let mut publisher: gz::transport::Publisher<EntityWrench> = node.advertise("/world/iflight_world/wrench").unwrap();

    // for i in 0..100 {
    //     // let v: Vec<f32> = vec![2.0, 2.0, 52.0, 2.0];
    //     let ent1 = Entity {
    //         name: String::from("iflight_force::prop_0"),
    //         type_: EnumOrUnknown::from_i32(3),
    //         ..Default::default()
    //     };
    //     let ent3 = Entity {
    //         name: String::from("iflight_force::prop_2"),
    //         type_: EnumOrUnknown::from_i32(3),
    //         ..Default::default()
    //     };
    //     let mut frc = Vector3d::new();
    //     frc.x = 0.0;
    //     frc.y = 0.0;
    //     frc.z = 500.0;
    //     let mut trq = Vector3d::new();
    //     trq.x = 0.0;
    //     trq.y = 0.0;
    //     trq.z = 0.0;
    //     let wr = Wrench {
    //         force: MessageField::some(frc),
    //         torque: MessageField::some(trq),
    //         ..Default::default()
    //     };

    //     let topic1 = EntityWrench {
    //         entity: MessageField::some(ent1),
    //         wrench: MessageField::some(wr),
    //         ..Default::default()
    //     };
    //     let mut frc = Vector3d::new();
    //     frc.x = 0.0;
    //     frc.y = 0.0;
    //     frc.z = 500.0;
    //     let mut trq = Vector3d::new();
    //     trq.x = 0.0;
    //     trq.y = 0.0;
    //     trq.z = 0.0;
    //     let wr = Wrench {
    //         force: MessageField::some(frc),
    //         torque: MessageField::some(trq),
    //         ..Default::default()
    //     };
    //     let topic3 = EntityWrench {
    //         entity: MessageField::some(ent3),
    //         wrench: MessageField::some(wr),
    //         ..Default::default()
    //     };

    //     assert!(publisher.publish(&topic1));
    //     assert!(publisher.publish(&topic3));
    //     sleep(Duration::from_millis(100));
    // }
    
    assert!(node.subscribe("/telem", move |msg: Pose| {
            let ent1 = Entity {
                name: String::from("iflight_force::prop_0"),
                type_: EnumOrUnknown::from_i32(3),
                ..Default::default()
            };
            let ent2 = Entity {
                name: String::from("iflight_force::prop_1"),
                type_: EnumOrUnknown::from_i32(3),
                ..Default::default()
            };
            let ent3 = Entity {
                name: String::from("iflight_force::prop_2"),
                type_: EnumOrUnknown::from_i32(3),
                ..Default::default()
            };
            let ent4 = Entity {
                name: String::from("iflight_force::prop_3"),
                type_: EnumOrUnknown::from_i32(3),
                ..Default::default()
            };

            let quat = msg.orientation;
            let quat_x = quat.x as f32;
            let quat_y = quat.y as f32;
            let quat_z = quat.z as f32;
            let quat_w = quat.w as f32;
            let uq = Quaternion::new(quat_w, quat_x, quat_y, quat_z);
            let cuq = uq.conjugate();
            let fq1 = Quaternion::new(0.0, 0.0, 0.0, 5.0);
            let fq2 = Quaternion::new(0.0, 0.0, 0.0, 4.99);
            let newq1 = uq * fq1 * cuq;
            let newq2 = uq * fq2 * cuq;

            let mut frc1 = Vector3d::new();
            frc1.x = newq1.i as f64;
            frc1.y = newq1.j as f64;
            frc1.z = newq1.k as f64;

            let mut frc2 = Vector3d::new();
            frc2.x = newq2.i as f64;
            frc2.y = newq2.j as f64;
            frc2.z = newq2.k as f64;
            let trq = Vector3d::new();
            
            // println!("{} {} {}", newq.i, newq.j, newq.k);
            let wr1 = Wrench {
                force: MessageField::some(frc1),
                torque: MessageField::some(trq.clone()),
                ..Default::default()
            };
            let wr2 = Wrench {
                force: MessageField::some(frc2),
                torque: MessageField::some(trq.clone()),
                ..Default::default()
            };

            let topic1 = EntityWrench {
                entity: MessageField::some(ent1),
                wrench: MessageField::some(wr1.clone()),
                ..Default::default()
            };
            let topic2 = EntityWrench {
                entity: MessageField::some(ent2),
                wrench: MessageField::some(wr1.clone()),
                ..Default::default()
            };
            let topic3 = EntityWrench {
                entity: MessageField::some(ent3),
                wrench: MessageField::some(wr2.clone()),
                ..Default::default()
            };
            let topic4 = EntityWrench {
                entity: MessageField::some(ent4),
                wrench: MessageField::some(wr2.clone()),
                ..Default::default()
            };
            assert!(publisher.publish(&topic1));
            assert!(publisher.publish(&topic2));
            assert!(publisher.publish(&topic3));
            assert!(publisher.publish(&topic4));
            // let eul = unit_quat.euler_angles();
            // let eul_arr = [eul.0, -eul.1, -eul.2].map(|x| x / PI * 180.0);
            // RPY_DATA.store(eul_arr);
            // sleep(Duration::from_millis(100));
        }));
    gz::transport::wait_for_shutdown();
}