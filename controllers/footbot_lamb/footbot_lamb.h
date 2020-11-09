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
// #include "trough_entity.h"
#include "/home/cerbero/TFG/100-lambs/plugins/simulator/entities/trough_entity.h"

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

    void static SetTroughs();

private:

    void TurnLeft();
    void TurnRight();
    void MoveForward();
    void Stop();

    void Ping();
    void SendPosition();
    void PollMessages();

    CVector2 CalculateDirection(CVector2 target);
    void GoTo(CVector2 target);

    void UpdatePriority();
    bool IsInPlace(CVector2 point);
    CVector2 GetClosestPoint(vector<CVector2> *targets);

    CVector3 GetPos();
    CVector3 GetDirection();

    void static SetIdNum(CFootBotLamb* robot);

    Real proxi_limit;
    Real robot_radius;

    /* Generador de números aleatorios */
    CRandom::CRNG*  rng;

    static UInt8 id_counter;
    UInt8 id_num;
    static vector<CVector2> water_troughs;
    static vector<CVector2> food_troughs;

    /* Pointer to the differential steering actuator */
    CCI_DifferentialSteeringActuator *wheels_act;
    /* Pointer to the foot-bot proximity sensor */
    CCI_FootBotProximitySensor *proxi_sens;
    // Pointer to pos sensor
    CCI_PositioningSensor *pos_sens;
    //Radio sensor and actuators
    CCI_RangeAndBearingSensor *rb_sens;
    CCI_RangeAndBearingActuator *rb_act;

    Real speed;

    //parametros de para Artificial Potential Fields
    Real alpha, beta;


    //variables para calcular cuando se hace ping o se decrementan los HP(health points)
    Real ping_interval, hp_interval, bt_interval;
    Real ping_timer, hp_timer, bt_timer;

    UInt8 mess_count;
    bool clear_message;//bandera
    std::map<UInt8,Neightbor_Info> neightbors;

    CVector2 pos, target;
    EulerRotation rot;

    //Puntos de salud o HP
    UInt32 rest, food, water;
    string priority;
    CVector2 current_target;

    Real radius;

    BrainTree::BehaviorTree bt;

    /****************************************************/

    CCI_PositioningSensor::SReading pos_readings;
    CCI_FootBotProximitySensor::TReadings proxi_readings;

    /****************************************************/
    //Clases privadas que representan comportamientos en el behavior tree
    class NodeFootBot: public BrainTree::Node{
    protected:
        CFootBotLamb *lamb;
    public:
        NodeFootBot(CFootBotLamb *lamb):Node(){
            this->lamb = lamb;
        }
    };

    //TODO Quiza se puedan definir las clases con un MACRO y quedaria
    //más limpio
    class NeedWater :
     public NodeFootBot{
    public:
        NeedWater(CFootBotLamb * lamb):NodeFootBot(lamb){}
        Status update() override;
    };

    class NeedFood :
     public NodeFootBot{
    public:
        NeedFood(CFootBotLamb * lamb):NodeFootBot(lamb){}
        Status update() override;
    };

    class NeedRest :
     public NodeFootBot{
    public:
        NeedRest(CFootBotLamb * lamb):NodeFootBot(lamb){}
        Status update() override;
    };

    class CanDrink:
     public NodeFootBot{
    public:
        CanDrink(CFootBotLamb * lamb):NodeFootBot(lamb){}
        Status update() override;
    };

    class GoToWater :
     public NodeFootBot{
    public:
        GoToWater(CFootBotLamb * lamb):NodeFootBot(lamb){}
        Status update() override;
        void terminate(Status s) override;
    };

    class Drink :
     public NodeFootBot{
    public:
        Drink(CFootBotLamb * lamb):NodeFootBot(lamb){}
        Status update() override;
    };



};

#endif
