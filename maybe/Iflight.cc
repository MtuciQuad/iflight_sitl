#include <string>
#include <gz/common/Console.hh>
#include <gz/plugin/Register.hh>

#include "Iflight.hh"



#include <gz/sim/System.hh>
#include <gz/sim/Model.hh>
#include <gz/transport/Node.hh>
#include <gz/sensors/ImuSensor.hh>
#include <gz/msgs.hh>

using namespace gz::sim;

GZ_ADD_PLUGIN(
    gz::sim::systems::Iflight,
    gz::sim::System,
    gz::sim::systems::Iflight::ISystemConfigure,
    gz::sim::systems::Iflight::ISystemPreUpdate)

GZ_ADD_PLUGIN_ALIAS(gz::sim::systems::Iflight, "Iflight")

// using namespace iflight;


class gz::sim::systems::IflightPrivate
{
  public: gz::transport::Node node;
  public: gz::transport::Node::Publisher pub;
  public: gz::msgs::IMU imuMsg;
  public: void cb(const gz::msgs::IMU &_msg)
  {
    // std::cout << "123" << std::endl;
    this->imuMsg = _msg;
    // std::cout << "Msg: " << _msg << std::endl << std::endl;
  };

}

gz::sim::systems::Iflight::Iflight()
  : dataPtr(new IflightPrivate())
{
}

void gz::sim::systems::Iflight::Configure(const Entity &_entity,
    const std::shared_ptr<const sdf::Element> &_sdf,
    EntityComponentManager &_ecm,
    EventManager &_eventMgr)
{
  // sdf::ElementPtr sdfClone = _sdf->Clone();

  // this->pub = this->node.Advertise<gz::msgs::StringMsg>("ImuData");


  // this->sub_node.Subscribe("/MotorForces", cb);
  this->dataPtr->node.Subscribe("/imu", &gz::sim::systems::IflightPrivate::cb);


}


void gz::sim::systems::Iflight::PreUpdate(const gz::sim::UpdateInfo &_info,
  gz::sim::EntityComponentManager &/*_ecm*/)
{
  std::string msg = "Hello, world! Simulation is ";
  if (!_info.paused)
    msg += "not ";
  msg += "paused.";

  // gzmsg << imuMsg << std::endl;

  // this->sub_node.Subscribe("/imu", cb);
  
  // gz::msgs::StringMsg gz_msg;
  // gz_msg.set_data("HELLO");
  // this->pub.Publish(gz_msg);
  // gzmsg << msg << std::endl;
  
  // if (!this->node.Subscribe("/imu", cb))
  // {
  //   gzmsg << "Error subscribing to topic" << std::endl;
  // }
}
