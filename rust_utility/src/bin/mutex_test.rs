use gz::transport::Node;
use gz::msgs::float_v::Float_V;
use std::sync::{Arc, Mutex};
use std::sync::mpsc::channel;

fn main() {
    let mut node = Node::new().unwrap();
    // let mdata = Arc::new(Mutex::new(0));
    let (tx, rx) = channel();

    assert!(node.subscribe("/RcData", move |msg: Float_V| {
        let data = msg.data;
        // println!("{:?}", data);
        tx.send(data).unwrap();
    }));

    loop {
        let received_data = rx.recv().unwrap();
        println!("Received: {:?}", received_data);
    }
    // gz::transport::wait_for_shutdown();
}