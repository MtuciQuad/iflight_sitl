#include <string>
#include <gz/common/Console.hh>
#include <gz/plugin/Register.hh>

#include "Iflight.hh"
#include <gz/sim/Util.hh>


#include <gz/sim/System.hh>
#include <gz/sim/Model.hh>
#include <gz/transport/Node.hh>
#include <gz/sim/components/JointForceCmd.hh>
#include <gz/msgs.hh>


GZ_ADD_PLUGIN(
    gz::sim::systems::Iflight,
    gz::sim::System,
    gz::sim::systems::Iflight::ISystemConfigure,
    gz::sim::systems::Iflight::ISystemPreUpdate)

GZ_ADD_PLUGIN_ALIAS(gz::sim::systems::Iflight, "Iflight")

// Class for each rotor
class Rotor
{
  public: Rotor() {}
  public: ~Rotor() {}
  public: int index;
  public: std::string jointName;
  public: gz::sim::Entity joint;
  public: int direction_k;
  public: double currentForce = 0.0;
};

class gz::sim::systems::IflightPrivate
{
  public: gz::sim::Model model;
  
  public: std::vector<Rotor> rotors;
  
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
}

// Get rotors from sdf model
void gz::sim::systems::Iflight::LoadMotors(
  sdf::ElementPtr _sdf,
  gz::sim::EntityComponentManager &_ecm)
{
  sdf::ElementPtr rotorSDF;
  if (_sdf->HasElement("rotor"))
  {
    rotorSDF = _sdf->GetElement("rotor");
  }

  while (rotorSDF)
  {
    Rotor rotor;

    if (rotorSDF->HasAttribute("index"))
    {
      rotor.index = atoi(rotorSDF->GetAttribute("index")->GetAsString().c_str());
    }
    else
    {
      gzwarn << "A rotor without index was found. Plugin wouldn't work.\n";
      return;
    }

    if (rotorSDF->HasElement("joint_name"))
    {
      rotor.jointName = rotorSDF->Get<std::string>("joint_name");
    }
    else
    {
      gzwarn << "A rotor without joint_name was found. Plugin wouldn't work.\n";
      return;
    }

    if (rotorSDF->HasElement("direction_k"))
    {
      rotor.direction_k = rotorSDF->Get<double>("direction_k");
    }
    else
    {
      gzwarn << "A rotor without direction_k was found. Plugin wouldn't work.\n";
      return;
    }

    auto entities = entitiesFromScopedName(rotor.jointName, _ecm, this->dataPtr->model.Entity());
    rotor.joint = *entities.begin();

    this->dataPtr->rotors.push_back(rotor);
    rotorSDF = rotorSDF->GetNextElement("rotor");
  }
}
  
void gz::sim::systems::Iflight::PreUpdate(const gz::sim::UpdateInfo &_info,
    gz::sim::EntityComponentManager &_ecm)
{
  gz::msgs::Float_V motorMsg;
  {
    std::lock_guard<std::mutex> lock(this->dataPtr->motorMsgMutex);
    if (this->dataPtr->motorMsgValid)
    {
      motorMsg = this->dataPtr->motorMsg;
      if (motorMsg.data().size() == 4) {
        for (size_t i = 0; i < this->dataPtr->rotors.size(); ++i)
        {
          this->dataPtr->rotors[i].currentForce = motorMsg.data().Get(i) * this->dataPtr->rotors[i].direction_k;
        }
      }
    }
    this->ApplyMotorForces(_ecm);
  }
  
  // gzmsg << "123" << std::endl;
}

void gz::sim::systems::Iflight::ApplyMotorForces(
  gz::sim::EntityComponentManager &_ecm)
{
  for (size_t i = 0; i < this->dataPtr->rotors.size(); ++i)
  {
    gz::sim::components::JointForceCmd* jfcComp = nullptr;
    jfcComp = _ecm.Component<gz::sim::components::JointForceCmd>(this->dataPtr->rotors[i].joint);
    if (jfcComp != nullptr) {
      jfcComp->Data()[0] = this->dataPtr->rotors[i].currentForce;
    }
  }
}