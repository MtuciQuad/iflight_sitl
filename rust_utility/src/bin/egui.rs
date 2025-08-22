use eframe::{
    egui::{self, Atom, DragValue, Ui},
    App,
};
use gz::{msgs::protobuf::rt, transport::Node};
use gz::msgs::{
    image::Image,
    float_v::Float_V,
    imu::IMU,
    world_stats::WorldStatistics,
    pose::Pose
};
use std::sync::mpsc::{channel, Receiver, Sender};
use std::sync::{Arc, Mutex};
use crossbeam_utils::atomic::AtomicCell;
use std::process::Command;

fn main() -> eframe::Result {
    let options = eframe::NativeOptions {
        viewport: egui::ViewportBuilder::default().with_inner_size([2000.0, 1200.0]),
        ..Default::default()
    };
    eframe::run_native(
        "SITL client",
        options,
        Box::new(|cc| {
            // This gives us image support:
            egui_extras::install_image_loaders(&cc.egui_ctx);
            Ok(Box::<MyApp>::default())
        }),
    )
}

struct MyApp {
    node: Node,
    camera_rx: Receiver<Vec<u8>>,
    rtf: Arc<Mutex<f64>>,
    // motor_rx: Receiver<Vec<f32>>
}

static MOTOR_DATA: AtomicCell<[f32; 4]> = AtomicCell::new([0f32; 4]);
static ACC_DATA: AtomicCell<[f32; 3]> = AtomicCell::new([0f32; 3]);
static GYRO_DATA: AtomicCell<[f32; 3]> = AtomicCell::new([0f32; 3]);
static POS_DATA: AtomicCell<[f32; 3]> = AtomicCell::new([0f32; 3]);
static QUAT_DATA: AtomicCell<[f32; 4]> = AtomicCell::new([0f32; 4]);
// static RTF: Arc<Mutex<f64>> = Arc::new(Mutex::new(0.0f64));
// static RTF: AtomicCell<f64> = AtomicCell::new(0.0);

impl Default for MyApp {
    fn default() -> Self {
        let mut node = Node::new().unwrap();
        let (camera_tx, camera_rx) = channel();
        assert!(node.subscribe("/camera", move |msg: Image| {
            camera_tx.send(msg.data).unwrap();
        }));

        assert!(node.subscribe("/MotorData", |msg: Float_V| {
            MOTOR_DATA.store(msg.data.try_into().unwrap());
        }));

        assert!(node.subscribe("/imu", |msg: IMU| {
            let ang = msg.angular_velocity.as_ref().unwrap();
            let lin = msg.linear_acceleration.as_ref().unwrap();
            let gx = -ang.x as f32;
            let gy = -ang.y as f32;
            let gz = -ang.z as f32;
            let ax = lin.x as f32;
            let ay = lin.y as f32;
            let az = lin.z as f32;
            ACC_DATA.store([ax, ay, az]);
            GYRO_DATA.store([gx, gy, gz]);
        }));

        let rtf = Arc::new(Mutex::new(0.0f64));
        let rtf_clone = Arc::clone(&rtf);
        assert!(node.subscribe("/world/iflight_world/stats", move |msg: WorldStatistics| {
            let mut rtf_clone = rtf_clone.lock().unwrap();
            *rtf_clone = msg.real_time_factor;
            
            // let out = Command::new("gz")
            // .args(["model", "--m", "iris_with_ardupilot"])
            // .output().expect("Error");
        
            // println!("{:?}", out);
            // println!("{:?}", String::from_utf8(out.stdout).unwrap());
            // println!("123");
        }));
        
        assert!(node.subscribe("/telem", move |msg: Pose| {
            // camera_tx.send(msg.data).unwrap();
            let pos = msg.position;
            let quat = msg.orientation;
            let pos_x = pos.x as f32;
            let pos_y = pos.y as f32;
            let pos_z = pos.z as f32;
            let quat_x = quat.x as f32;
            let quat_y = quat.y as f32;
            let quat_z = quat.z as f32;
            let quat_w = quat.w as f32;
            POS_DATA.store([pos_x, pos_y, pos_z]);
            QUAT_DATA.store([quat_x, quat_y, quat_z, quat_w]);
        }));

        Self {
            node: node,
            camera_rx,
            rtf
            // motor_rx
        }
    }
}

impl eframe::App for MyApp {
    fn update(&mut self, ctx: &egui::Context, _frame: &mut eframe::Frame) {
        egui::CentralPanel::default().show(ctx, |ui| {
            self.test_update(ui).expect("failed");
        });
        ctx.request_repaint();
    }
}

impl MyApp {
    fn test_update(&mut self, ui: &mut egui::Ui) -> Result<(), Box<dyn std::error::Error>> {
        // ui.label(format!("Hello '{}', age {}", self.name, self.age));
        let frame = self.camera_rx.recv().unwrap();
        let frame_size = [1280 as usize, 720 as usize];
        let img = egui::ColorImage::from_rgb(frame_size, &frame);
        let texture = ui.ctx().load_texture("gazebo", img, Default::default());
        ui.horizontal(|ui| {
            ui.image(&texture);
            // if ui.button("Reset").clicked() {
            //     let out = Command::new("gz")
            //     .args(["service", "-s", "/world/iflight_world/control", "--reqtype", "gz.msgs.WorldControl", "--reptype", "gz.msgs.Boolean", "--req", "reset: {all: true}"])
            //     .output().expect("Error");
            // }
        });
        ui.add_space(20.0);
        ui.label("21332313132");
        ui.label(format!("Motor forces: {:?}", MOTOR_DATA.take()));
        ui.label(format!("ACC: {:?}", ACC_DATA.take()));
        ui.label(format!("GYRO: {:?}", GYRO_DATA.take()));
        // let rtf_val = RTF.take();
        // ui.label(format!("RTF: {:?}", RTF.take()));
        ui.label(format!("RTF: {:?}", self.rtf.lock().unwrap()));
        ui.label(format!("POS: {:?}", POS_DATA.take()));
        ui.label(format!("QUAT: {:?}", QUAT_DATA.take()));
        Ok(())
    }
}