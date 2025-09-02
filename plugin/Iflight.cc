#include <string>
#include <gz/common/Console.hh>
#include <gz/plugin/Register.hh>

#include "Iflight.hh"
#include <gz/sim/Util.hh>


#include <gz/sim/System.hh>
#include <gz/sim/Model.hh>
#include <gz/transport/Node.hh>
#include <gz/sim/components/JointForceCmd.hh>
#include <gz/sim/components/World.hh>
#include <gz/sim/World.hh>
#include <gz/msgs.hh>
// #include <gz/sim/components/Link.hh>
// #include <gz/sim/components/Name.hh>
#include <gz/sim/components/Pose.hh>
// #include <gz/sim/components/Sensor.hh>
// #include <gz/sim/components/World.hh>


GZ_ADD_PLUGIN(
    gz::sim::systems::Iflight,
    gz::sim::System,
    gz::sim::systems::Iflight::ISystemConfigure,
    gz::sim::systems::Iflight::ISystemPreUpdate,
    gz::sim::systems::Iflight::ISystemPostUpdate)

GZ_ADD_PLUGIN_ALIAS(gz::sim::systems::Iflight, "Iflight")


class gz::sim::systems::IflightPrivate
{
  public: gz::sim::Model model;
  
  // public: std::vector<Rotor> rotors;
  
  public: gz::transport::Node node;

  public: gz::msgs::Float_V motorMsg;
  public: bool motorMsgValid{false};
  public: std::mutex motorMsgMutex;

  public: void MotorCb(const gz::msgs::Float_V &_msg)
  {
    std::lock_guard<std::mutex> lock(this->motorMsgMutex);
    motorMsg = _msg;
    motorMsgValid = true;
  }

  public: double motorForce = 5;

  public: gz::sim::Entity imuLink{gz::sim::kNullEntity};
  public: gz::transport::Node::Publisher pub;
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

  this->dataPtr->node.Subscribe("/MotorData", &gz::sim::systems::IflightPrivate::MotorCb, this->dataPtr.get());

  this->dataPtr->imuLink = this->dataPtr->model.LinkByName(_ecm, "imu_link");
  // gzmsg << this->dataPtr->imuLink << std::endl;
  enableComponent<components::WorldPose>(
          _ecm, this->dataPtr->imuLink, true);

  this->dataPtr->pub = this->dataPtr->
          node.Advertise<msgs::Pose>("telem");
}


  
void gz::sim::systems::Iflight::PreUpdate(const gz::sim::UpdateInfo &_info,
    gz::sim::EntityComponentManager &_ecm)
{
  gz::msgs::Float_V motorMsg;
  {
    // std::lock_guard<std::mutex> lock(this->dataPtr->motorMsgMutex);
    // if (this->dataPtr->motorMsgValid)
    // {
    //   motorMsg = this->dataPtr->motorMsg;
    //   if (motorMsg.data().size() == 4) {
    //     for (size_t i = 0; i < this->dataPtr->rotors.size(); ++i)
    //     {
    //       this->dataPtr->rotors[i].currentForce = motorMsg.data().Get(i) * this->dataPtr->rotors[i].direction_k / 100.0 * this->dataPtr->motorForce;
    //     }
    //   } else {
    //     for (size_t i = 0; i < this->dataPtr->rotors.size(); ++i)
    //     {
    //       this->dataPtr->rotors[i].currentForce = 0;
    //     }
    //   }
    // }
    // this->ApplyMotorForces(_ecm);
  }
}

void gz::sim::systems::Iflight::PostUpdate(const gz::sim::UpdateInfo &_info,
    const gz::sim::EntityComponentManager &_ecm)
{

  const gz::sim::components::WorldPose* worldPose =
      _ecm.Component<gz::sim::components::WorldPose>(
          this->dataPtr->imuLink);
  gz::msgs::Pose poseMsg;
  gz::msgs::Set(&poseMsg, worldPose->Data());
  this->dataPtr->pub.Publish(poseMsg);
}

// void gz::sim::systems::Iflight::ApplyMotorForces(
//   gz::sim::EntityComponentManager &_ecm)
// {
//   // gzmsg << this->dataPtr->rotors.size() << std::endl;
//   for (size_t i = 0; i < this->dataPtr->rotors.size(); ++i)
//   {
//     // gzmsg << this->dataPtr->rotors[i].joint << std::endl;
//     gz::sim::components::JointForceCmd* jfcComp = nullptr;
//     jfcComp = _ecm.Component<gz::sim::components::JointForceCmd>(this->dataPtr->rotors[i].joint);
//     if (jfcComp == nullptr)
//       {
//         jfcComp = _ecm.CreateComponent(this->dataPtr->rotors[i].joint,
//             gz::sim::components::JointForceCmd({0}));
//       }
//     // if (jfcComp != nullptr) {
//     // }
//     jfcComp->Data()[0] = this->dataPtr->rotors[i].currentForce;
    
//   }
// }
