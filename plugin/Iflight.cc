#include <string>
#include <gz/common/Console.hh>
#include <gz/plugin/Register.hh>

#include "Iflight.hh"
#include <gz/sim/Util.hh>


#include <gz/sim/System.hh>
#include <gz/sim/Model.hh>
#include <gz/transport/Node.hh>
#include <gz/sim/components/JointForceCmd.hh>
#include <gz/sim/components/ExternalWorldWrenchCmd.hh>

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

class Prop
{
  public: Prop() {}
  public: ~Prop() {}
  public: int index;
  public: std::string linkName;
  public: gz::sim::Entity link;
  public: int direction_k;
  public: double currentForce = 0.0;
  public: double currentTorque = 0.0;
};

class gz::sim::systems::IflightPrivate
{
  public: gz::sim::Model model;
  
  public: std::vector<Prop> props;
  
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

  public: double motorForce = 2;
  public: double motorTorque = 0.01;

  public: gz::sim::Entity imuLink{gz::sim::kNullEntity};
  public: gz::transport::Node::Publisher pub;

  public: gz::math::Pose3<double> wPose;
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

  this->LoadMotors(sdfClone, _ecm);

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
  this->dataPtr->wPose = _ecm.Component<gz::sim::components::WorldPose>(this->dataPtr->imuLink)->Data();
  // gzmsg << this->dataPtr->wPose->Data() << std::endl;
  gz::msgs::Float_V motorMsg;
  {
    std::lock_guard<std::mutex> lock(this->dataPtr->motorMsgMutex);
    if (this->dataPtr->motorMsgValid)
    {
      motorMsg = this->dataPtr->motorMsg;
      if (motorMsg.data().size() == 4) {
        for (size_t i = 0; i < this->dataPtr->props.size(); ++i)
        {
          this->dataPtr->props[i].currentForce = motorMsg.data().Get(i) / 100.0 * this->dataPtr->motorForce;
          this->dataPtr->props[i].currentTorque = motorMsg.data().Get(i) * this->dataPtr->props[i].direction_k / 100.0 * this->dataPtr->motorTorque;
        }
      } else {
        for (size_t i = 0; i < this->dataPtr->props.size(); ++i)
        {
          this->dataPtr->props[i].currentForce = 0;
          this->dataPtr->props[i].currentTorque = 0;
        }
      }
    }
    this->ApplyMotorForces(_ecm);
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

void gz::sim::systems::Iflight::ApplyMotorForces(
  gz::sim::EntityComponentManager &_ecm)
{
  // gzmsg << this->dataPtr->props.size() << std::endl;
  for (size_t i = 0; i < this->dataPtr->props.size(); ++i)
  {
    gz::sim::components::ExternalWorldWrenchCmd* ewwcComp = nullptr;
    ewwcComp = _ecm.Component<gz::sim::components::ExternalWorldWrenchCmd>(this->dataPtr->props[i].link);
    // gzmsg << (ewwcComp == nullptr) << std::endl;
    if (ewwcComp == nullptr)
      {
        ewwcComp = _ecm.CreateComponent(this->dataPtr->props[i].link,
            gz::sim::components::ExternalWorldWrenchCmd());
        // gzmsg << 666 << std::endl;        
      }
    // if (ewwcComp != nullptr) {
    //   gzmsg << 123 << std::endl;
    // }
    gz::math::Quaternion q(this->dataPtr->wPose.Roll(), this->dataPtr->wPose.Pitch(), this->dataPtr->wPose.Yaw());
    gz::math::Vector3d fv(0.0, 0.0, this->dataPtr->props[i].currentForce);
    gz::math::Vector3d tv(0.0, 0.0, this->dataPtr->props[i].currentTorque);
    auto force_new = q.RotateVector(fv);
    auto torque_new = q.RotateVector(tv);
    // gzmsg << force_new << std::endl;

    msgs::Set(ewwcComp->Data().mutable_force(), 
                  force_new);
    msgs::Set(ewwcComp->Data().mutable_torque(), 
                  torque_new);
    // ewwcComp->Data()[0] = this->dataPtr->rotors[i].currentForce;
    
  }
}

void gz::sim::systems::Iflight::LoadMotors(
  sdf::ElementPtr _sdf,
  gz::sim::EntityComponentManager &_ecm)
{
  sdf::ElementPtr propSDF;
  if (_sdf->HasElement("prop"))
  {
    propSDF = _sdf->GetElement("prop");
  }

  while (propSDF)
  {
    Prop prop;

    if (propSDF->HasAttribute("index"))
    {
      prop.index = atoi(propSDF->GetAttribute("index")->GetAsString().c_str());
    }
    else
    {
      gzwarn << "A prop without index was found. Plugin wouldn't work.\n";
      return;
    }

    if (propSDF->HasElement("link_name"))
    {
      prop.linkName = propSDF->Get<std::string>("link_name");
    }
    else
    {
      gzwarn << "A prop without link_name was found. Plugin wouldn't work.\n";
      return;
    }

    if (propSDF->HasElement("direction"))
    {
      std::string dir = propSDF->Get<std::string>("direction");
      if (dir == "cw") {
        prop.direction_k = 1;
      } else if (dir == "ccw") {
        prop.direction_k = -1;
      } else {
        gzwarn << "A prop with invalid direction was found. Plugin wouldn't work.\n";
        return;
      }
    }
    else
    {
      gzwarn << "A prop without direction was found. Plugin wouldn't work.\n";
      return;
    }

    auto entities = entitiesFromScopedName(prop.linkName, _ecm, this->dataPtr->model.Entity());
    prop.link = *entities.begin();

    this->dataPtr->props.push_back(prop);
    propSDF = propSDF->GetNextElement("prop");
  }
}