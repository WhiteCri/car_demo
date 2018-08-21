#include <gazebo/math/Rand.hh>
#include <gazebo/gui/GuiIface.hh>
#include <gazebo/rendering/rendering.hh>
#include <gazebo/gazebo.hh>
#include <gazebo/gui/MouseEventHandler.hh>
#include <gazebo/common/MouseEvent.hh>
#include <cmath>

namespace gazebo
{
  class SystemGUI : public SystemPlugin
  {

    // Destructor
    public: virtual ~SystemGUI()
    {
        gui::MouseEventHandler::Instance()->RemovePressFilter("glwidget");
    }

    // Called after the plugin has been constructed.
    public: void Load(int /*_argc*/, char ** /*_argv*/)
    {   
        gui::MouseEventHandler::Instance()->AddPressFilter("glwidget",
            boost::bind(&SystemGUI::OnMousePress, this, _1));
    }

    public: bool OnMousePress(const common::MouseEvent & _event)
    {
        std::cout << "mouse pressed!\n";
        if (!_event.Control())
            return false;
        // Check intersection
        rendering::UserCameraPtr camera = gui::get_active_camera();
        ignition::math::Vector3d intersection;
        
        if (!camera->GetScene()->FirstContact(
             camera, _event.Pos(), intersection))
        {
          std::cout << "error for using firstContact()\n";
          return false;
        }
        std::cout << "x, y, z : " << intersection.X() << ' ' << intersection.Y() << ' ' << intersection.Z() << '\n';
        std::cout << "x_gap, y_gap, z_gap, orientation(euler_xy) : " << 
            intersection.X() - intersection_p.X() << ' ' <<
            intersection.Y() - intersection_p.Y() << ' ' <<
            intersection.Z() - intersection_p.Z() << ' ' << 
            atan2(intersection.Y() - intersection_p.Y(),intersection.X() - intersection_p.X()) * 180 / 3.141592 
            << '\n';
        intersection_p = intersection;

        return true;
  }
  ignition::math::Vector3d intersection_p;

  // Register this plugin with the simulator
};
  GZ_REGISTER_SYSTEM_PLUGIN(SystemGUI)

}