//
// Created by cschuwerk on 11/13/17.
//

#include "trinamic1640/motor_driver_trinamic1640.h"

namespace rovi_motor_drivers {

    motor_driver_trinamic1640::motor_driver_trinamic1640()  {
        this->serialConnection = NULL;
    }


    motor_driver_trinamic1640::motor_driver_trinamic1640(std::string name, config_trinamic1640 &cfg) : name(name), cfg(cfg) {
        this->serialConnection = NULL;
    }

    motor_driver_trinamic1640::~motor_driver_trinamic1640() {
        this->stop();
        this->close();
        ROS_INFO_STREAM_NAMED(this->name, "Trinamic driver shutdown: " << this->cfg.device);
    }


    void motor_driver_trinamic1640::setDevice(std::string dev) {
        this->cfg.device = dev;
    }


    double motor_driver_trinamic1640::getVelocity() {
        return 0;
    }


    void motor_driver_trinamic1640::setVelocity(double v) {
        this->setTargetVelocity((int) v);
    }


    bool motor_driver_trinamic1640::open(void) {

        try {
            this->serialConnection = new serial::Serial();
            serialConnection->setPort(this->cfg.device);
            serialConnection->setBaudrate(this->cfg.baudrate);
            serial::Timeout timeout = serial::Timeout::simpleTimeout(1000);
            serialConnection->setTimeout(timeout);
            serialConnection->open();
            ROS_INFO_STREAM_NAMED(this->name, "Serial port opened successfully.");
        } catch (std::exception &e) {
            ROS_ERROR_STREAM_NAMED(this->name, "Serial Error: Failed to open serial port: " << e.what());
            return false;
        }

        // Set commutation method
        this->setCommutationMethod(6);
        return true;

    }


    bool motor_driver_trinamic1640::close(void) {

        this->stop();
        if(serialConnection->isOpen()) {
            serialConnection->close();
        }

    }


    void motor_driver_trinamic1640::stop(void) {
        this->stopMotor();
    }


    bool motor_driver_trinamic1640::setVelocityPID(cfgPID &cfg) {
        int status = 0;
        status+= this->setParamPvelocity((int) cfg.p);
        status+= this->setParamIvelocity((int) cfg.i);
        if (status==200) return true;
        return false;
    }


    cfgPID motor_driver_trinamic1640::getVelocityPID(void) {
        return cfgPID(this->getParamPvelocity(), this->getParamIvelocity(), 0.0);
    }

    bool motor_driver_trinamic1640::setTorquePID(cfgPID &cfg) {
        int status = 0;
        status+= this->setParamPcurrent((int) cfg.p);
        status+= this->setParamIcurrent((int) cfg.i);
        if (status==200) return true;
        return false;
    }

    cfgPID motor_driver_trinamic1640::getTorquePID(void) {
        return cfgPID(this->getParamPcurrent(), this->getParamIcurrent(), 0.0);
    }





    int motor_driver_trinamic1640::SendReceiveData(int32_t value, uint8_t command, uint8_t type, uint8_t readtype) {

        std::vector<uint8_t> writeData(9);
        std::vector<uint8_t> readData;
        int returnData;

        this->constructCommand(value, command, type, writeData);

        // Write the command
        try {
            this->serialConnection->write(writeData.data(), 9);
        }
        catch (std::exception &e) {
            ROS_ERROR_STREAM_NAMED(this->name, "Error while writing to serial port: " << e.what());
            return 0;
        }

        //sleep(0.1);

        // Read the result
        try {
            auto length = this->serialConnection->read(readData, 9);
        }
        catch (std::exception &e) {
            ROS_ERROR_STREAM_NAMED(this->name, "Error while reading from serial port: " << e.what());
            return 0;
        }

        // Read the result: return either the status byte (set methods) or the data (get methods)
        if (readtype == 0) {
            returnData = readData[2];
        }
        if (readtype == 10) {
            returnData = readData[4] << 24 | readData[5] << 16 | readData[6] << 8 | readData[7];
        }

        return returnData;

    }


    void motor_driver_trinamic1640::constructCommand(int32_t value, uint8_t command, uint8_t type,
                                                 std::vector<uint8_t> &commandToTransmit) {

        commandToTransmit[0] = 1;
        commandToTransmit[1] = command;
        commandToTransmit[2] = type;
        commandToTransmit[3] = 0;
        commandToTransmit[4] = value >> 24;
        commandToTransmit[5] = value >> 16;
        commandToTransmit[6] = value >> 8;
        commandToTransmit[7] = value & 0xff;

        // Checksum
        commandToTransmit[8] =
                commandToTransmit[0] + commandToTransmit[1] + commandToTransmit[2] + commandToTransmit[3] +
                commandToTransmit[4] + commandToTransmit[5] + commandToTransmit[6] + commandToTransmit[7];

    }


    int motor_driver_trinamic1640::getMaxCurrent() {
        return SendReceiveData(0, 6, 6, 10);
    }


    int motor_driver_trinamic1640::setMaxCurrent(int current) {
        int status = SendReceiveData(current, 5, 6, 0);
        if (status == 100) {
            status = SendReceiveData(0, 7, 6, 0);
        }
        return status;
    }


    int motor_driver_trinamic1640::setStartCurrent(int current) {
        return SendReceiveData(current, 5, 177, 0);
    }


    int motor_driver_trinamic1640::getStartCurrent() {
        return SendReceiveData(0, 6, 177, 10);
    }


    int motor_driver_trinamic1640::setCommutationMethod(int method) {
        int status = SendReceiveData(method, 5, 159, 0);
        if (status == 100) {
            status = SendReceiveData(0, 7, 159, 0);
        }
        return status;
    }


    int motor_driver_trinamic1640::getCommutationMethod() {
        return SendReceiveData(0, 6, 159, 10);
    }


    int motor_driver_trinamic1640::setMotorPoleNum(int poles) {
        int status = SendReceiveData(poles, 5, 253, 0);
        if (status == 100) {
            status = SendReceiveData(0, 7, 253, 0);
        }
        return status;
    }


    int motor_driver_trinamic1640::getMotorPoleNum() {
        return SendReceiveData(0, 6, 253, 10);
    }


    int motor_driver_trinamic1640::setMaxVelocity(int velocity) {
        int status = SendReceiveData(velocity, 5, 4, 0);
        if (status == 100) {
            status = SendReceiveData(0, 7, 4, 0);
        }
        return status;
    }


    int motor_driver_trinamic1640::getMaxVelocity() {
        return SendReceiveData(0, 6, 4, 10);
    }


    int motor_driver_trinamic1640::setAcceleration(int acceleration) {
        int status = SendReceiveData(acceleration, 5, 11, 0);
        if (status == 100) {
            status = SendReceiveData(0, 7, 11, 0);
        }
        return status;
    }


    int motor_driver_trinamic1640::getAcceleration() {
        return SendReceiveData(0, 6, 11, 10);
    }


    int motor_driver_trinamic1640::stopMotor() {
        return SendReceiveData(0, 3, 0, 0);
    }


    int motor_driver_trinamic1640::rotateRight(int velocity) {
        return SendReceiveData(velocity, 1, 0, 0);
    }


    int motor_driver_trinamic1640::rotateLeft(int velocity) {
        return SendReceiveData(velocity, 2, 0, 0);
    }


    int motor_driver_trinamic1640::moveToPosition(int position) {
        int status;
        int positionNew;

        setActualPos(0);
        positionNew = 6.3 * position;
        status = SendReceiveData(positionNew, 4, 0, 0);
        return status;
    }


    int motor_driver_trinamic1640::setTargetPos(int position) {
        int status;
        int positionNew;

        setActualPos(0);
        positionNew = 6.3 * position;
        status = SendReceiveData(positionNew, 5, 0, 0);
        return status;
    }


    int motor_driver_trinamic1640::setActualPos(int position) {
        int status;
        int positionNew;

        positionNew = 6.3 * position;
        status = SendReceiveData(positionNew, 5, 1, 0);
        return status;
    }


    int motor_driver_trinamic1640::setTargetVelocity(int velocity) {
        return SendReceiveData(velocity, 5, 2, 0);
    }


    int motor_driver_trinamic1640::setTargetMotorCurrent(int current) {
        return SendReceiveData(current, 5, 155, 0);
    }


    int motor_driver_trinamic1640::getTargetPosition() {
        return SendReceiveData(0, 6, 0, 10);
    }


    double motor_driver_trinamic1640::getActualPosition() {
        int position;
        double position_Grad;
        position = SendReceiveData(0, 6, 1, 10);
        position_Grad = (position / 2268.0) * 360.0;
        return position_Grad;
    }


    int motor_driver_trinamic1640::getTargetVelocity() {
        return SendReceiveData(0, 6, 2, 10);
    }


    int motor_driver_trinamic1640::getActualVelocity() {
        return SendReceiveData(0, 6, 3, 10);
    }


    int motor_driver_trinamic1640::getActualMotorCurrent() {
        return SendReceiveData(0, 6, 150, 10);
    }


    int motor_driver_trinamic1640::getTargetMotorCurrent() {
        return SendReceiveData(0, 6, 155, 10);
    }


    int motor_driver_trinamic1640::setPIDdelayPosVel(int delay) {
        int status = SendReceiveData(delay, 5, 133, 0);
        if (status == 100) {
            status = SendReceiveData(0, 7, 133, 0);
        }
        return status;
    }


    int motor_driver_trinamic1640::getPIDdelayPosVel() {
        return SendReceiveData(0, 6, 133, 10);
    }


    int motor_driver_trinamic1640::setPIDdelayCurrent(int delay) {
        int status = SendReceiveData(delay, 5, 134, 0);
        if (status == 100) {
            status = SendReceiveData(0, 7, 134, 0);
        }
        return status;
    }


    int motor_driver_trinamic1640::getPIDdelayCurrent() {
        int delay = SendReceiveData(0, 6, 134, 10);
        return delay;
    }


    int motor_driver_trinamic1640::setParamPcurrent(int p) {
        int status = SendReceiveData(p, 5, 172, 0);
        if (status == 100) {
            status = SendReceiveData(0, 7, 172, 0);
        }
        return status;
    }


    int motor_driver_trinamic1640::getParamPcurrent() {
        return SendReceiveData(0, 6, 172, 10);
    }


    int motor_driver_trinamic1640::setParamIcurrent(int I) {
        int status = SendReceiveData(I, 5, 173, 0);
        if (status == 100) {
            status = SendReceiveData(0, 7, 173, 0);
        }
        return status;
    }


    int motor_driver_trinamic1640::getParamIcurrent() {
        return SendReceiveData(0, 6, 173, 10);
    }


    int motor_driver_trinamic1640::setParamPposition(int p) {
        int status = SendReceiveData(p, 5, 230, 0);
        if (status == 100) {
            status = SendReceiveData(0, 7, 230, 0);
        }
        return status;
    }


    int motor_driver_trinamic1640::getParamPposition() {
        return SendReceiveData(0, 6, 230, 10);
    }


    int motor_driver_trinamic1640::setParamPvelocity(int p) {
        int status = SendReceiveData(p, 5, 234, 0);
        if (status == 100) {
            status = SendReceiveData(0, 7, 234, 0);
        }
        return status;
    }


    int motor_driver_trinamic1640::getParamPvelocity() {
        return SendReceiveData(0, 6, 234, 10);
    }


    int motor_driver_trinamic1640::setParamIvelocity(int I) {
        int status = SendReceiveData(I, 5, 235, 0);
        if (status == 100) {
            status = SendReceiveData(0, 7, 235, 0);
        }
        return status;
    }


    int motor_driver_trinamic1640::getParamIvelocity() {
        return SendReceiveData(0, 6, 235, 10);
    }


    int motor_driver_trinamic1640::getHallAngle() {
        return SendReceiveData(0, 6, 210, 10);
    }

}