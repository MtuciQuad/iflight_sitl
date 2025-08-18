use eframe::{
    egui::{self, DragValue, Ui},
    App,
};
use gz::transport::Node;
use gz::msgs::image::Image;
use std::sync::mpsc::{channel, Receiver};
use std::process::Command;

fn main() -> eframe::Result {
    let options = eframe::NativeOptions {
        viewport: egui::ViewportBuilder::default().with_inner_size([1280.0, 720.0]),
        ..Default::default()
    };
    eframe::run_native(
        "My egui App",
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
    rx: Receiver<Vec<u8>>
}

// gz service -s /world/iflight_world/control --reqtype gz.msgs.WorldControl --reptype gz.msgs.Boolean --req 'reset: {all: true}'


impl Default for MyApp {
    fn default() -> Self {
        let mut node = Node::new().unwrap();
        let (tx, rx) = channel();
        assert!(node.subscribe("/camera", move |msg: Image| {
            // println!("{:?}", msg.data);
            // println!("123");
            let image = msg.data;
            tx.send(image).unwrap();

        }));
        Self {
            node: node,
            rx: rx,
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

        if ui.button("Reset").clicked() {
            let out = Command::new("gz")
            .args(["service", "-s", "/world/iflight_world/control", "--reqtype", "gz.msgs.WorldControl", "--reptype", "gz.msgs.Boolean", "--req", "reset: {all: true}"])
            .output().expect("Error");
        }
        // ui.image(egui::include_image!(
        //     "ferris.png"
        // ));
        let frame = self.rx.recv().unwrap();
        // let frame= [self.age as u8; 3 * 320 * 240];
        let frame_size = [1280 as usize, 720 as usize];
        let img = egui::ColorImage::from_rgb(frame_size, &frame);
        let texture = ui.ctx().load_texture("frame", img, Default::default());
        ui.image(&texture);
        Ok(())

    }
}