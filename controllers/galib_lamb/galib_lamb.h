#ifndef GALIB_LAMB_H
#define GALIB_LAMB_H

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
/* Definition of several sensors and actuators */
#include <argos3/plugins/robots/generic/control_interface/ci_differential_steering_actuator.h>
#include <argos3/plugins/robots/generic/control_interface/ci_positioning_sensor.h>
#include <argos3/plugins/robots/generic/control_interface/ci_range_and_bearing_sensor.h>
#include <argos3/plugins/robots/generic/control_interface/ci_range_and_bearing_actuator.h>
#include <argos3/plugins/robots/generic/control_interface/ci_leds_actuator.h>
#include <argos3/plugins/robots/lamb-bot/control_interface/ci_lambbot_proximity_sensor.h>

// id obtaining and entity workflow
#include <argos3/plugins/simulator/visualizations/qt-opengl/qtopengl_user_functions.h>
// TODO es necesario incluir la lambbot_entity?
#include <argos3/plugins/robots/lamb-bot/simulator/lambbot_entity.h>
#include <argos3/plugins/simulator/entities/trough_entity.h>

#define CODE_PING 1
#define CODE_PING_REPLY 2

#define STAT_FULL 1000
#define STAT_HIGH 700
#define STAT_BAD 400
#define STAT_CRITIC 100

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

enum Stat_type {water, food, rest, social, walk};

/*
* A controller is simply an implementation of the CCI_Controller class.
*/
class CGalibLamb : public CCI_Controller {
public:

    CGalibLamb();

    virtual ~CGalibLamb() {}

    /*
    * This function initializes the controller.
    * The 't_node' variable points to the <parameters> section in the XML
    * file in the <controllers><lamb_controller> section.
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

    void ShowDebugInformation(bool show);
    void static SetTroughs();

    CVector2 GetCorrectedPos();
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

    CVector3 GetDirection();

    void static SetIdNum(CGalibLamb* robot);

    Real proxi_limit;
    Real robot_radius;

    /* Generador de números aleatorios */
    CRandom::CRNG*  rng;

    static UInt8 id_counter;
    UInt8 id_num;

    //TODO tratar las camas de la misma forma que los comederos y bebederos
    //static y obteniendo la posicion de la misma forma
    static vector<CVector2> water_troughs;
    static vector<CVector2> food_troughs;
    vector<CVector2> beds;
    CVector2 random_pos;


    /* Pointer to sensors and actuator */
    CCI_DifferentialSteeringActuator *wheels_act;
    CCI_LEDsActuator *leds;
    CCI_LambBotProximitySensor *proxi_sens;
    CCI_PositioningSensor *pos_sens;
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
    map<UInt8,Neightbor_Info> neightbors;

    CVector2 pos;
    EulerRotation rot;

    //Puntos de salud o HP
    map<Stat_type, UInt32> stats;
    //TODO comentar y ordenar
    Stat_type current_priority;

    CVector2 current_target;

    Real radius;

    BrainTree::BehaviorTree bt;
    bool show_debug;
    map<Stat_type, CColor> colors;

    /****************************************************/

    CCI_PositioningSensor::SReading pos_readings;
    CCI_LambBotProximitySensor::TReadings proxi_readings;

    /****************************************************/
    //Clases privadas que representan comportamientos en el behavior tree
    class NodeLamb: public BrainTree::Node{
    protected:
        CGalibLamb *lamb;
    public:
        NodeLamb(CGalibLamb *lamb):Node(){
            this->lamb = lamb;
        }
    };

    //Varias definiciones de clases similares mediante un MACRO
    #define NODE_0_DECLARATION(NODE_NAME) \
    class NODE_NAME: public NodeLamb{\
        public:    \
        NODE_NAME(CGalibLamb * lamb):NodeLamb(lamb){}\
        Status update() override;\
    };

    #define NODE_1_DECLARATION(NODE_NAME) \
    class NODE_NAME: public NodeLamb{\
        public:    \
        NODE_NAME(CGalibLamb * lamb):NodeLamb(lamb){}\
        Status update() override;\
        void terminate(Status s) override;\
    };

    NODE_0_DECLARATION(NeedWater)
    NODE_0_DECLARATION(CanDrink)
    NODE_1_DECLARATION(GoToWater)
    NODE_1_DECLARATION(Drink)

    NODE_0_DECLARATION(NeedFood)
    NODE_0_DECLARATION(CanEat)
    NODE_1_DECLARATION(GoToFood)
    NODE_1_DECLARATION(Eat)

    NODE_0_DECLARATION(NeedRest)
    NODE_0_DECLARATION(CanSleep)
    NODE_1_DECLARATION(GoToBed)
    NODE_1_DECLARATION(Sleep)

    NODE_0_DECLARATION(SelectRandomPos)
    NODE_0_DECLARATION(IsAtRandomPos)
    NODE_1_DECLARATION(GoToRandomPos)



    #undef NODE_0_DECLARATION
    #undef NODE_1_DECLARATION


};

#endif
