use gz::transport::Node;
use gz::msgs::image::Image;
use image::{ImageBuffer, Rgb};


fn main() {
    let mut node = Node::new().unwrap();
    assert!(node.subscribe("/camera", |msg: Image| {
        // println!("{:?}", msg.data);
        println!("123");
        let image = msg.data;
        let img: ImageBuffer<Rgb<u8>, Vec<u8>> = ImageBuffer::from_vec(
            1920,
            1080,
            image,
        ).expect("123");
        img.save("test_output.png").unwrap();
    }));
    gz::transport::wait_for_shutdown();
}