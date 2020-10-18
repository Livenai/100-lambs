#ifndef FOOTBOT_LAMB_H
#define FOOTBOT_LAMB_H

/*
 * Include some necessary headers.
 */
#include <iostream>
#include <cstring>
#include <map>
#include "BrainTree.h"

// core for log
#include <argos3/core/simulator/simulator.h>
#include <argos3/core/utility/math/angles.h>
#include <argos3/core/utility/math/rng.h>
/* Definition of the CCI_Controller class. */
#include <argos3/core/control_interface/ci_controller.h>
/* Definition of the differential steering actuator */
#include <argos3/plugins/robots/generic/control_interface/ci_differential_steering_actuator.h>
/* Definition of the foot-bot proximity sensor */
#include <argos3/plugins/robots/foot-bot/control_interface/ci_footbot_proximity_sensor.h>
/* Definition of the positioning sensor */
#include <argos3/plugins/robots/generic/control_interface/ci_positioning_sensor.h>
/*radio sensor and actuartor*/
#include <argos3/plugins/robots/generic/control_interface/ci_range_and_bearing_sensor.h>
#include <argos3/plugins/robots/generic/control_interface/ci_range_and_bearing_actuator.h>

// id obtaining and entity workflow
#include <argos3/plugins/simulator/visualizations/qt-opengl/qtopengl_user_functions.h>
#include <argos3/plugins/robots/foot-bot/simulator/footbot_entity.h>


#define CODE_PING 1
#define CODE_PING_REPLY 2

#define HP_STAT_FULL 700
#define HP_STAT_BAD 200
#define HP_STAT_CRITIC 100

//en radianes
#define ANGLE_THRESHOLD 0.035

/*
 * All the ARGoS stuff in the 'argos' namespace.
 * With this statement, you save typing argos:: every time.
 */
using namespace argos;
using namespace std;
/*
 * A controller is simply an implementation of the CCI_Controller class.
 */
 enum HPState {GOOD, BAD, CRITIC};

 struct Neightbor_Info{
     CVector2 pos;
     Real range;
     CRadians bearing;
 };

struct EulerRotation{
    CRadians x,y,z;
    EulerRotation(){
        x = y = z = CRadians::ZERO;
    }
};
class CFootBotLamb : public CCI_Controller {

public:


    /* Class constructor. */
    CFootBotLamb();

    /* Class destructor. */
    virtual ~CFootBotLamb() {}

    /*
    * This function initializes the controller.
    * The 't_node' variable points to the <parameters> section in the XML
    * file in the <controllers><footbot_diffusion_controller> section.
    */
    virtual void Init(TConfigurationNode& t_node);

    /*
    * This function is called once every time step.
    * The length of the time step is set in the XML file.
    */
    virtual void ControlStep();

    /*
    * This function resets the controller to its state right after the
    * Init().
    */
    virtual void Reset();

    /*
    * Called to cleanup what done by Init() when the experiment finishes.
    * In this example controller there is no need for clean anything up,
    * so the function could have been omitted. It's here just for
    * completeness.
    */
    virtual void Destroy() {}


    // activa o desactiva los prints
    bool ENABLE_DEBUG = false;

private:

    void TurnLeft();
    void TurnRight();
    void MoveForward();
    void Stop();

    void Ping();
    void SendPosition();
    void PollMessages();

    HPState getHPState(UInt32 health_stat);
    bool isInPlace(CVector2 point);

    void static SetIdNum(CFootBotLamb* robot);

    /* Generador de números aleatorios */
    CRandom::CRNG*  rng;

    static UInt8 id_counter;
    UInt8 id_num;

    /* Pointer to the differential steering actuator */
    CCI_DifferentialSteeringActuator* m_pcWheels;
    /* Pointer to the foot-bot proximity sensor */
    CCI_FootBotProximitySensor* m_pcProximity;
    // Pointer to pos sensor
    CCI_PositioningSensor* m_pcPosSens;
    //Radio sensor and actuators
    CCI_RangeAndBearingSensor *m_pcRBSens;
    CCI_RangeAndBearingActuator *m_pcRBAct;
    /*
    * The following variables are used as parameters for the
    * algorithm. You can set their value in the <parameters> section
    * of the XML configuration file, under the
    * <controllers><footbot_diffusion_controller> section.
    */

    /* Maximum tolerance for the angle between
    * the robot heading direction and
    * the closest obstacle detected. */
    CDegrees m_cAlpha;
    /* Maximum tolerance for the proximity reading between
    * the robot and the closest obstacle.
    * The proximity reading is 0 when nothing is detected
    * and grows exponentially to 1 when the obstacle is
    * touching the robot.
    */
    Real m_fDelta;
    /* Wheel speed. */
    Real m_fWheelVelocity;
    /* Angle tolerance range to go straight.
    * It is set to [-alpha,alpha]. */
    CRange<CRadians> m_cGoStraightAngleRange;

    //Variables para determinar cuando se hace ping y se reducen los puntos de salud
    Real ping_interval, hp_interval;
    Real ping_timer, hp_timer;

    UInt8 mess_count;
    bool clear_message;//bandera
    std::map<UInt8,Neightbor_Info> neightbors;

    CVector2 pos;
    EulerRotation rot;

    //Puntos de salud o HP
    UInt32 energy, food, water;

    CVector2 water_pos, food_pos, bed_pos, random_pos;
    Real radius;

    BrainTree::BehaviorTree bt;

    /****************************************************/
    //variables para evitar el alojamiento de memoria dinamico en metodos que se llaman repetidamente
    Real disY;
    Real disX;
    CCI_PositioningSensor::SReading readings;

    /****************************************************/
    //Clases privadas que representan comportamientos en el behavior tree
    class NodeFootBot: public BrainTree::Node{
    protected:
        CFootBotLamb *lamb;
        UInt32 *health_stat;
        CVector2 *target_pos;
    public:
        NodeFootBot(CFootBotLamb *lamb, string const  health_stat):Node(){
            this->lamb = lamb;
            if (health_stat == "water"){
                this->health_stat = &(lamb->water);
                target_pos = &(lamb->water_pos);
            }
            else if(health_stat == "food"){
                this->health_stat = &(lamb->food);
                target_pos = &(lamb->food_pos);
            }
            else if(health_stat == "energy"){
                this->health_stat = &(lamb->energy);
                target_pos = &(lamb->bed_pos);
            }
            else if(health_stat == "random"){
                this->health_stat = NULL;
                target_pos = &(lamb->random_pos);
            }

        }
    };


    class IncreaseHP : public NodeFootBot{
    public:
        IncreaseHP(CFootBotLamb * lamb, string health_stat):NodeFootBot(lamb, health_stat){}
        Status update() override;
    };

    class Aligne: public NodeFootBot{
    public:
        Aligne(CFootBotLamb * lamb, string health_stat):NodeFootBot(lamb, health_stat){}
        Status update() override;
    private:
        CRadians angle_to_target;
    };

    class Advance: public NodeFootBot{
    public:
        Advance(CFootBotLamb * lamb, string health_stat):NodeFootBot(lamb, health_stat){}
        Status update() override;
    };

    class SelectRandomPos: public NodeFootBot{
    public:
        SelectRandomPos(CFootBotLamb * lamb, string health_stat):NodeFootBot(lamb, health_stat){}
        Status update() override;
    };

    class ConditionDepletion : public NodeFootBot{
    public:
        ConditionDepletion(CFootBotLamb * lamb, string health_stat):NodeFootBot(lamb, health_stat){}
        Status update() override;
    };


    class ConditionIsInPlace : public NodeFootBot{
    public:
        ConditionIsInPlace(CFootBotLamb * lamb, string health_stat):NodeFootBot(lamb, health_stat){}
        Status update() override;
    };


    class ConditionAligned: public NodeFootBot{
    public:
        ConditionAligned(CFootBotLamb * lamb, string health_stat):NodeFootBot(lamb, health_stat){}
        Status update() override;
    private:
        CRadians angle_to_target;
    };


    class PrintNode : public NodeFootBot{
    public:
        PrintNode(CFootBotLamb * lamb, string health_stat):NodeFootBot(lamb, health_stat){}
        Status update() override;
    };

};

#endif
