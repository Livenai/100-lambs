#ifndef FOOTBOT_LAMB_H
#define FOOTBOT_LAMB_H

/*
 * Include some necessary headers.
 */
#include <iostream>
#include <cstring>
#include <map>
#include <math.h>
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
#define ANGLE_THRESHOLD 0.0523
#define NUM_SAMPLE_POINTS 24

/*
 * All the ARGoS stuff in the 'argos' namespace.
 * With this statement, you save typing argos:: every time.
 */
using namespace argos;
using namespace std;
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
/*
* A controller is simply an implementation of the CCI_Controller class.
*/
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
    virtual void Destroy();


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

    CVector2 CalculateDirection(CVector2 target);

    HPState GetHPState(UInt32 health_stat);
    bool IsInPlace(CVector2 point);

    CVector3 GetPos();
    CVector3 GetDirection();

    void static SetIdNum(CFootBotLamb* robot);

    Real proxi_limit;
    Real robot_radius;

    /* Generador de números aleatorios */
    CRandom::CRNG*  rng;

    static UInt8 id_counter;
    UInt8 id_num;

    /* Pointer to the differential steering actuator */
    CCI_DifferentialSteeringActuator *wheels_act;
    /* Pointer to the foot-bot proximity sensor */
    CCI_FootBotProximitySensor *proxi_sens;
    // Pointer to pos sensor
    CCI_PositioningSensor *pos_sens;
    //Radio sensor and actuators
    CCI_RangeAndBearingSensor *rb_sens;
    CCI_RangeAndBearingActuator *rb_act;

    Real speed, rot_speed;

    //parametros de para Artificial Potential Fields
    Real alpha, beta;


    //variables para calcular cuando se hace ping o se decrementan los HP(health points)
    Real ping_interval, hp_interval, bt_interval;
    Real ping_timer, hp_timer, bt_timer;

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
    //TODO comentar
    vector<CVector2> sample_points;
    vector<CVector2> sample_points_norm;
    CCI_PositioningSensor::SReading pos_readings;
    CCI_FootBotProximitySensor::TReadings proxi_readings;

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

    class GoTo: public NodeFootBot{
    public:
        GoTo(CFootBotLamb * lamb, string health_stat):NodeFootBot(lamb, health_stat){}
        Status update() override;
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
    };


    class PrintNode : public NodeFootBot{
    public:
        PrintNode(CFootBotLamb * lamb, string health_stat):NodeFootBot(lamb, health_stat){}
        Status update() override;
    };

};

#endif
