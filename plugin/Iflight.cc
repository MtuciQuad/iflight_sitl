#include <string>
#include <gz/common/Console.hh>
#include <gz/plugin/Register.hh>

#include "Iflight.hh"
#include <gz/sim/Util.hh>



#include <gz/sim/System.hh>
#include <gz/sim/Model.hh>
#include <gz/transport/Node.hh>
#include <gz/sim/components/JointForceCmd.hh>
// #include <gz/sensors/ImuSensor.hh>
#include <gz/msgs.hh>

// using namespace gz::sim;
// using namespace iflight;


GZ_ADD_PLUGIN(
    gz::sim::systems::Iflight,
    gz::sim::System,
    gz::sim::systems::Iflight::ISystemConfigure,
    gz::sim::systems::Iflight::ISystemPreUpdate)

GZ_ADD_PLUGIN_ALIAS(gz::sim::systems::Iflight, "Iflight")


class gz::sim::systems::IflightPrivate
{
  public: gz::sim::Model model;
  // public: gz::transport::Node node;
  // public: gz::transport::Node::Publisher pub;
  // public: gz::msgs::IMU imuMsg;
  // public: gz::sim::Entity linkEntity;
};

gz::sim::systems::Iflight::Iflight()
  : dataPtr(new IflightPrivate())
{
}

gz::sim::systems::Iflight::~Iflight()
{
}



void gz::sim::systems::Iflight::Configure(const Entity &_entity,
    const std::shared_ptr<const sdf::Element> &_sdf,
    EntityComponentManager &_ecm,
    EventManager &_eventMgr)
{
  sdf::ElementPtr sdfClone = _sdf->Clone();
  this->dataPtr->model = gz::sim::Model(_entity);



  // this->pub = this->node.Advertise<gz::msgs::StringMsg>("ImuData");
  // this->node.Subscribe("/imu", &iflight::Iflight::cb, this);
  // this->sub_node.Subscribe("/MotorForces", cb);
  
  // this->linkEntity = this->model.LinkByName(_ecm, "link_name");
  // if (!_ecm.EntityHasComponentType(this->linkEntity,
  //                                       components::WorldPose::typeId))
  // {
      // _ecm.CreateComponent(this->linkEntity, components::WorldPose());
  //     gzmsg << "123" << std::endl;
  // }
  }

  void gz::sim::systems::Iflight::PreUpdate(const gz::sim::UpdateInfo &_info,
     gz::sim::EntityComponentManager &_ecm)
  {
    std::string msg = "Hello, world! Simulation is ";
    if (!_info.paused)
      msg += "not ";
    msg += "paused.";
  
  
  
    // {
    //   auto entities = entitiesFromScopedName("rotor_0_joint", _ecm, this->dataPtr->model.Entity());
    //   Entity joint = *entities.begin();
    //   gz::sim::components::JointForceCmd* jfcComp = _ecm.Component<gz::sim::components::JointForceCmd>(joint);
    //   if (jfcComp == nullptr)
    //       {
    //         // jfcComp = _ecm.CreateComponent(joint,
    //         //     gz::sim::components::JointForceCmd({0}));
    //         gzmsg << "null" << std::endl;
    //       }
    //   else {
    //     jfcComp->Data()[0] = 10;
    //     // gzmsg << jfcComp->Data()[0] << std::endl;
    //   }
    // }
    
    // {
    //   auto entities = entitiesFromScopedName("rotor_2_joint", _ecm, this->dataPtr->model.Entity());
    //   Entity joint = *entities.begin();
    //   gz::sim::components::JointForceCmd* jfcComp = _ecm.Component<gz::sim::components::JointForceCmd>(joint);
    //   if (jfcComp == nullptr)
    //       {
    //         // jfcComp = _ecm.CreateComponent(joint,
    //         //     gz::sim::components::JointForceCmd({0}));
    //         gzmsg << "null" << std::endl;
    //       }
    //   else {
    //     jfcComp->Data()[0] = 10;
    //     // gzmsg << jfcComp->Data()[0] << std::endl;
    //   }
    // }

    // {
    //   auto entities = entitiesFromScopedName("rotor_1_joint", _ecm, this->dataPtr->model.Entity());
    //   Entity joint = *entities.begin();
    //   gz::sim::components::JointForceCmd* jfcComp = _ecm.Component<gz::sim::components::JointForceCmd>(joint);
    //   if (jfcComp == nullptr)
    //       {
    //         // jfcComp = _ecm.CreateComponent(joint,
    //         //     gz::sim::components::JointForceCmd({0}));
    //         gzmsg << "null" << std::endl;
    //       }
    //   else {
    //     jfcComp->Data()[0] = -10;
    //     // gzmsg << jfcComp->Data()[0] << std::endl;
    //   }
    // }

    // {
    //   auto entities = entitiesFromScopedName("rotor_3_joint", _ecm, this->dataPtr->model.Entity());
    //   Entity joint = *entities.begin();
    //   gz::sim::components::JointForceCmd* jfcComp = _ecm.Component<gz::sim::components::JointForceCmd>(joint);
    //   if (jfcComp == nullptr)
    //       {
    //         // jfcComp = _ecm.CreateComponent(joint,
    //         //     gz::sim::components::JointForceCmd({0}));
    //         gzmsg << "null" << std::endl;
    //       }
    //   else {
    //     jfcComp->Data()[0] = -10;
    //     // gzmsg << jfcComp->Data()[0] << std::endl;
    //   }
    // }

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


















  

// void Iflight::publishImuData() {
//     float imuData[] = {
//       (float)(imuMsg.linear_acceleration().y() / 10.0),
//       -(float)(imuMsg.linear_acceleration().x() / 10.0),
//       (float)(imuMsg.linear_acceleration().z() / 10.0),
// 			-(float)(imuMsg.angular_velocity().y()),
//       (float)(imuMsg.angular_velocity().x()),
//       -(float)(imuMsg.angular_velocity().z())
//     };

//     // gzmsg << imuData[0] << std::endl;
// }