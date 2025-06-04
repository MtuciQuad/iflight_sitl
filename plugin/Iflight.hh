#ifndef SYSTEM_PLUGIN_IFLIGHT_HH_
#define SYSTEM_PLUGIN_IFLIGHT_HH_


#include <gz/sim/Model.hh>
#include <gz/sim/System.hh>
#include <gz/transport/Node.hh>
#include <gz/msgs.hh>

namespace iflight
{
  class GZ_SIM_VISIBLE Iflight:
    public gz::sim::System,
    public gz::sim::ISystemConfigure,
    public gz::sim::ISystemPreUpdate
  {
    private:
      gz::transport::Node node;
      gz::transport::Node::Publisher pub;
      gz::msgs::IMU imuMsg;
      gz::sim::Model model;
      gz::sim::Entity linkEntity;

    public: void Configure(const gz::sim::Entity &_entity,
                        const std::shared_ptr<const sdf::Element> &_sdf,
                        gz::sim::EntityComponentManager &_ecm,
                        gz::sim::EventManager &) override;

    public: void PreUpdate(const gz::sim::UpdateInfo &_info,
                gz::sim::EntityComponentManager &_ecm) override;

    public: void cb(const gz::msgs::IMU &_msg)
    {
      // std::cout << "123" << std::endl;
      this->imuMsg = _msg;
      // std::cout << "Msg: " << _msg << std::endl << std::endl;
    };

    public: void publishImuData();

  };

}
#endif