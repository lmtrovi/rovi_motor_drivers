//
// Created by cschuwerk on 11/13/17.
//

#include "trinamic1640/motor_driver_trinamic1640.h"
#include <boost/scoped_ptr.hpp>
#include <iostream>


int main(int argc, char** argv)
{

    rovi_motor_drivers::config_trinamic1640 cfg;
    cfg.device = "/dev/robot/ttyTrinamic1640";

    rovi_motor_drivers::motor_driver_trinamic1640 trinamic_driver("driver_trinamic", cfg);
    trinamic_driver.open();

    //rovi_motor_drivers::motor_driver_trinamic1640* trinamic_driver= new rovi_motor_drivers::motor_driver_trinamic1640("driver_trinamic", "/dev/robot/ttyTrinamic1640");
    //trinamic_driver->open();

    //boost::scoped_ptr<rovi_motor_drivers::motor_driver_trinamic1640> ptr(new rovi_motor_drivers::motor_driver_trinamic1640("driver_trinamic", config_trinamic1640()));
    //ptr->open();


    //std::cout << trinamic_driver.getVelocity() << std::endl;

    //std::cout << trinamic_driver.getMaxCurrent() << std::endl;
    //std::cout << "Get Comm method: " << trinamic_driver.getCommutationMethod() << std::endl;
    //std::cout << "Set Comm method: " << trinamic_driver.setCommutationMethod(6) << std::endl;
    //std::cout << "Comm method: " << trinamic_driver.getCommutationMethod() << std::endl;
    //std::cout << "setStartCurrent method: " << trinamic_driver.setStartCurrent(100) << std::endl;

//    std::cout << "setVelocity method: " << std::endl;
//    rovi_motor_drivers::cfgPID cfg(100,0,0);
//    std::cout << "setStartCurrent method: " << trinamic_driver.setVelocityPID(cfg) << std::endl;
//    rovi_motor_drivers::cfgPID cfg2;
//    cfg2 =  trinamic_driver.getVelocityPID();
//
//
//    std::cout << "Start: ";
//    trinamic_driver.setVelocity(300.0);
//    sleep(4.0);
//    trinamic_driver.setVelocity(-300.0);
//    sleep(4.0);
//    //std::cout << "Stop: ";
//    trinamic_driver.stop();


    //sleep(1);
    //trinamic_driver.close();
    return 0;
}