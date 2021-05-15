#include "lamb.h"
//Nodos del behavior tree

// CLamb::NodeLamb::SetLedColor::SetLedColor(Stat_type stat){
//     this.stat = stat;
// }


CLamb::NodeLamb::Status CLamb::SetLedColor::update(){
    lamb->current_state = stat;
    lamb->leds->SetAllColors(lamb->colors[stat]);
    return Status::Success;
}

//***************************************
//Beber
//***************************************
CLamb::NodeLamb::Status CLamb::CanDrink::update(){
    if(lamb->distanceToTrough(lamb->GetClosestTrough(water)) < lamb->threshold_distance)
        return Status::Success;
    return Status::Failure;
}

CLamb::NodeLamb::Status CLamb::GoToWater::update(){
    CVector2 target = lamb->GetClosestTrough(water);
    lamb->GoToPoint(target);
    return Status::Running;
}

void CLamb::GoToWater::terminate(Status s){
    if(status == Status::Running){
        lamb->Stop();
        status = Status::Success;
    }
}


void CLamb::Drink::initialize(){
    //TODO la duracion debe ser sacada de los datos
    lamb->stats[water] = 20;
}

CLamb::NodeLamb::Status CLamb::Drink::update(){
    if(--lamb->stats[water] <= 0)
        return Status::Success;
    return Status::Running;
}

//TODO es necesario?
// void CLamb::Drink::terminate(Status s){
//         status = Status::Success;
// }

//***************************************
//Comer
//***************************************
CLamb::NodeLamb::Status CLamb::CanEat::update(){
    if(lamb->distanceToTrough(lamb->GetClosestTrough(food)) < lamb->threshold_distance)
        return Status::Success;
    return Status::Failure;
}

CLamb::NodeLamb::Status CLamb::GoToFood::update(){
    CVector2 target = lamb->GetClosestTrough(food);
    lamb->GoToPoint(target);
    return Status::Running;
}

void CLamb::GoToFood::terminate(Status s){
    if(status == Status::Running){
        lamb->Stop();
        status = Status::Success;
    }
}

void CLamb::Eat::initialize(){
    //TODO la duracion debe ser sacada de los datos
    lamb->stats[food] = 20;
}

CLamb::NodeLamb::Status CLamb::Eat::update(){
    if(--lamb->stats[food] <= 0)
        return Status::Success;
    return Status::Running;
}

//TODO es necesario?
// void CLamb::Eat::terminate(Status s){
//         status = Status::Success;
// }

//***************************************
//Descansar
//***************************************
CLamb::NodeLamb::Status CLamb::CanRest::update(){
    if(lamb->distanceToPoint(lamb->GetClosestRestingPlace()) < lamb->threshold_distance)
        return Status::Success;
    return Status::Failure;
}

CLamb::NodeLamb::Status CLamb::GoToRest::update(){
    CVector2 target = lamb->GetClosestRestingPlace();
    lamb->GoToPoint(target);
    return Status::Running;
}

void CLamb::GoToRest::terminate(Status s){
    if(status == Status::Running){
        lamb->Stop();
        status = Status::Success;
    }
}


void CLamb::Rest::initialize(){
    //TODO la duracion debe ser sacada de los datos
    lamb->stats[rest] = 20;
}

CLamb::NodeLamb::Status CLamb::Rest::update(){
    if(--lamb->stats[rest] <= 0)
        return Status::Success;
    return Status::Running;
}

//TODO es necesario?
// void CLamb::Rest::terminate(Status s){
//         status = Status::Success;
// }

//***************************************
//Caminar
//***************************************

CLamb::NodeLamb::Status CLamb::SelectRandomPos::update(){
    //TODO hardcoded el tamaño de la arena
    lamb->random_pos = CVector2(lamb->rng->Uniform(CRange<Real>(0,3)),
                        lamb->rng->Uniform(CRange<Real>(0,-3)));
    return Status::Success;
}

CLamb::NodeLamb::Status CLamb::IsAtRandomPos::update(){
    if(lamb->distanceToPoint(lamb->random_pos) < lamb->threshold_distance){
        return Status::Success;
    }
    return Status::Failure;
}

CLamb::NodeLamb::Status CLamb::GoToRandomPos::update(){
    lamb->GoToPoint(lamb->random_pos);
    return Status::Running;
}

//TODO es necesario?
void CLamb::GoToRandomPos::terminate(Status s){
    if(status == Status::Running){
        lamb->Stop();
        status = Status::Success;
    }
}
