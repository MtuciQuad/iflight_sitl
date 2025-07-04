use gz::transport::Node;
use gz::msgs::float_v::Float_V;

fn main() {
    let mut node = Node::new().unwrap();
    assert!(node.subscribe("/RcData", |msg: Float_V| {
        let data = msg.data;
        
    }));
    gz::transport::wait_for_shutdown();
}