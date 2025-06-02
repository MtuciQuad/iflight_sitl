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
using namespace iflight;


GZ_ADD_PLUGIN(
    iflight::Iflight,
    gz::sim::System,
    iflight::Iflight::ISystemConfigure,
    iflight::Iflight::ISystemPreUpdate)

GZ_ADD_PLUGIN_ALIAS(iflight::Iflight, "Iflight")




void Iflight::Configure(const Entity &_entity,
    const std::shared_ptr<const sdf::Element> &_sdf,
    EntityComponentManager &_ecm,
    EventManager &_eventMgr)
{
  sdf::ElementPtr sdfClone = _sdf->Clone();

  this->pub = this->node.Advertise<gz::msgs::StringMsg>("ImuData");


  // this->sub_node.Subscribe("/MotorForces", cb);
  this->node.Subscribe("/imu", &iflight::Iflight::cb, this);


}

void Iflight::PreUpdate(const gz::sim::UpdateInfo &_info,
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
