#ifndef SYSTEM_PLUGIN_IFLIGHT_HH_
#define SYSTEM_PLUGIN_IFLIGHT_HH_


#include <gz/sim/Model.hh>
#include <gz/sim/System.hh>
#include <gz/transport/Node.hh>
#include <gz/msgs.hh>

namespace gz
{
  namespace sim 
  {
    namespace systems
    {
      class IflightPrivate;
      
      class GZ_SIM_VISIBLE Iflight:
      public gz::sim::System,
      public gz::sim::ISystemConfigure,
      public gz::sim::ISystemPreUpdate
      {
      private:
        std::unique_ptr<IflightPrivate> dataPtr;

      public: Iflight();
    
      public: ~Iflight();

      public: void Configure(const gz::sim::Entity &_entity,
                          const std::shared_ptr<const sdf::Element> &_sdf,
                          gz::sim::EntityComponentManager &_ecm,
                          gz::sim::EventManager &) override;

      public: void PreUpdate(const gz::sim::UpdateInfo &_info,
                          gz::sim::EntityComponentManager &_ecm) override;

      private: void LoadMotors(sdf::ElementPtr _sdf,
                          gz::sim::EntityComponentManager &_ecm);

      private: void ApplyMotorForces(gz::sim::EntityComponentManager &_ecm);


      // public: void cb(const gz::msgs::IMU &_msg)
      // {
      //   // std::cout << "123" << std::endl;
      //   this->imuMsg = _msg;
      //   // std::cout << "Msg: " << _msg << std::endl << std::endl;
      // };

      // public: void publishImuData();
      };
    }
  }
}

#endif