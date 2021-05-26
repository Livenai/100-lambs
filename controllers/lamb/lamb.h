#ifndef LAMB_H
#define LAMB_H

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
// #include "trough_entity.h" TODO usar el directorio donde está instalado
#include "/home/cerbero/TFG/100-lambs/plugins/simulator/entities/trough_entity.h"

//en radianes
#define ANGLE_THRESHOLD 0.0523

/*
 * All the ARGoS stuff in the 'argos' namespace.
 * With this statement, you save typing argos:: every time.
 */
using namespace argos;
using namespace std;

 struct Neightbor_Info{
     CVector2 pos;
     CVector2 velocity;
     // Real range;
     // CRadians bearing;
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
class CLamb : public CCI_Controller {
public:

    CLamb();

    virtual ~CLamb() {}

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

    void SendPosition();
    void PollMessages();

    CVector2 simplifiedAPF(CVector2 target);
    void GoToPoint(CVector2 target);

    Real distanceToPoint(CVector2 point);
    Real distanceToTrough(CVector2 trough);

    CVector2 GetClosestTrough(Stat_type stat);
    CVector2 GetClosestRestingPlace();

    void static SetIdNum(CLamb* robot);

    /* Generador de números aleatorios */
    CRandom::CRNG*  rng;

    static UInt8 id_counter;
    UInt8 id_num;

    //TODO eliminar las variables static?
    static vector<CVector2> water_troughs;
    static vector<CVector2> food_troughs;
    CVector2 random_pos;
    CVector2 bed_pos;


    /* Pointer to sensors and actuator */
    CCI_DifferentialSteeringActuator *wheels_act;
    CCI_LEDsActuator *leds;
    CCI_LambBotProximitySensor *proxi_sens;
    CCI_PositioningSensor *pos_sens;
    CCI_RangeAndBearingSensor *rb_sens;
    CCI_RangeAndBearingActuator *rb_act;

    Real max_vel, rot_vel, speed_factor;

    //parametros de Artificial Potential Fields
    Real alpha, beta;

    Real threshold_distance;

    //Para regular el ritmo de ejecución
    Real bt_interval, bt_timer;

    UInt8 mess_count;
    bool clear_message;//bandera
    map<UInt8,Neightbor_Info> neightbors;

    CVector2 pos;
    EulerRotation rot;

    Stat_type current_state;
    map<Stat_type, UInt32> stats;

    BrainTree::BehaviorTree bt;
    bool show_debug;
    map<Stat_type, CColor> colors;

    //FIXME está hardcoded, puede que esté bien asi
    //Es importante que cada fila sume 1 exactamente
    double transition_matrix[4][4] = {{0.25, 0.05, 0.05, 0.65},
                                    {0.01, 0.41, 0.20, 0.38},
                                    {0.05, 0.30, 0.05, 0.60},
                                    {0.10, 0.10, 0.05, 0.75}};

    /****************************************************/

    CCI_PositioningSensor::SReading pos_readings;
    CCI_LambBotProximitySensor::TReadings proxi_readings;

    /****************************************************/
    // TODO mover declaracion de nodos a otro header
    //Clases privadas que representan comportamientos en el behavior tree
    class NodeLamb: public BrainTree::Node{
    protected:
        CLamb *lamb;
    public:
        NodeLamb(CLamb *lamb):Node(){
            this->lamb = lamb;
        }
    };

    //Varias definiciones de clases similares mediante un MACRO
    #define NODE_DECLARATION_0(NODE_NAME) \
    class NODE_NAME: public NodeLamb{\
        public:    \
        NODE_NAME(CLamb * lamb):NodeLamb(lamb){}\
        Status update() override;\
    };

    #define NODE_DECLARATION_1(NODE_NAME) \
    class NODE_NAME: public NodeLamb{\
        public:    \
        NODE_NAME(CLamb * lamb):NodeLamb(lamb){}\
        Status update() override;\
        void terminate(Status s) override;\
    };

    #define NODE_DECLARATION_2(NODE_NAME) \
    class NODE_NAME: public NodeLamb{\
        public:    \
        NODE_NAME(CLamb * lamb):NodeLamb(lamb){}\
        void initialize() override;\
        Status update() override;\
    };

    #define NODE_DECLARATION_3(NODE_NAME) \
    class NODE_NAME: public NodeLamb{\
        public:    \
        NODE_NAME(CLamb * lamb):NodeLamb(lamb){}\
        void initialize() override;\
        Status update() override;\
        void terminate(Status s) override;\
    };

    #define NODE_DECLARATION_4(NODE_NAME) \
    class NODE_NAME: public NodeLamb{\
        public:    \
        NODE_NAME(CLamb * lamb, Stat_type stat):NodeLamb(lamb),stat(stat){}\
        Status update() override;\
        private:    \
        Stat_type stat; \
    };


    NODE_DECLARATION_0(CanDrink)
    NODE_DECLARATION_1(GoToWater)
    NODE_DECLARATION_2(Drink)

    NODE_DECLARATION_0(CanEat)
    NODE_DECLARATION_1(GoToFood)
    NODE_DECLARATION_2(Eat)

    NODE_DECLARATION_0(CanRest)
    NODE_DECLARATION_1(GoToRest)
    NODE_DECLARATION_2(Rest)

    NODE_DECLARATION_0(SelectRandomPos)
    NODE_DECLARATION_0(IsAtRandomPosition)
    NODE_DECLARATION_3(Walk)
    NODE_DECLARATION_2(Wait)

    NODE_DECLARATION_4(SetLedColor)


    #undef NODE_DECLARATION_0
    #undef NODE_DECLARATION_1
    #undef NODE_DECLARATION_2
    #undef NODE_DECLARATION_4


};

#endif
