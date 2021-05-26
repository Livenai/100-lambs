#include "lamb.h"
//Nodos del behavior tree

CLamb::NodeLamb::Status CLamb::SetLedColor::update(){
    lamb->current_state = stat;
    lamb->leds->SetAllColors(lamb->colors[stat]);
    status = Status::Success;

    return status;
}

//***************************************
//Beber
//***************************************
CLamb::NodeLamb::Status CLamb::CanDrink::update(){
    if(lamb->distanceToTrough(lamb->GetClosestTrough(water)) < lamb->threshold_distance)
        status = Status::Success;
    else
        status = Status::Failure;

    return status;

}

CLamb::NodeLamb::Status CLamb::GoToWater::update(){
    CVector2 target = lamb->GetClosestTrough(water);
    lamb->speed_factor = 1;
    lamb->GoToPoint(target);
    status = Status::Running;

    return status;

}

void CLamb::GoToWater::terminate(Status s){
    if(status == Status::Running){
        lamb->Stop();
        status = Status::Success;
    }
}


void CLamb::Drink::initialize(){
    //TODO la duracion debe ser sacada de los datos
    lamb->stats[water] = 500;
}

CLamb::NodeLamb::Status CLamb::Drink::update(){
    if(--lamb->stats[water] <= 0)
        status = Status::Success;
    else
        status = Status::Running;

    return status;
}


//***************************************
//Comer
//***************************************
CLamb::NodeLamb::Status CLamb::CanEat::update(){
    if(lamb->distanceToTrough(lamb->GetClosestTrough(food)) < lamb->threshold_distance)
        status = Status::Success;
    else
        status = Status::Failure;

    return status;
}

CLamb::NodeLamb::Status CLamb::GoToFood::update(){
    CVector2 target = lamb->GetClosestTrough(food);
    lamb->speed_factor = 1;
    lamb->GoToPoint(target);
    status = Status::Running;

    return status;
}

void CLamb::GoToFood::terminate(Status s){
    if(status == Status::Running){
        lamb->Stop();
        status = Status::Success;
    }
}

void CLamb::Eat::initialize(){
    //TODO la duracion debe ser sacada de los datos
    lamb->stats[food] = 500;
}

CLamb::NodeLamb::Status CLamb::Eat::update(){
    if(--lamb->stats[food] <= 0)
        status = Status::Success;
    else
        status = Status::Running;

    return status;
}

//***************************************
//Descansar
//***************************************
CLamb::NodeLamb::Status CLamb::CanRest::update(){
    if(lamb->distanceToPoint(lamb->GetClosestRestingPlace()) < lamb->threshold_distance)
        status = Status::Success;
    else
        status = Status::Failure;

    return status;
}

CLamb::NodeLamb::Status CLamb::GoToRest::update(){
    CVector2 target = lamb->GetClosestRestingPlace();
    lamb->speed_factor = 0.75;
    lamb->GoToPoint(target);
        status = Status::Running;

    return status;
}

void CLamb::GoToRest::terminate(Status s){
    if(status == Status::Running){
        lamb->Stop();
        status = Status::Success;
    }
}


void CLamb::Rest::initialize(){
    //TODO la duracion debe ser sacada de los datos
    lamb->stats[rest] = 500;
}

CLamb::NodeLamb::Status CLamb::Rest::update(){
    if(--lamb->stats[rest] <= 0)
        status = Status::Success;
    else
        status = Status::Running;

    return status;
}


//***************************************
//Caminar
//***************************************

CLamb::NodeLamb::Status CLamb::SelectRandomPos::update(){
    //TODO hardcoded el tamaño de la arena
    lamb->random_pos = CVector2(lamb->rng->Uniform(CRange<Real>(0,3)),
                        lamb->rng->Uniform(CRange<Real>(0,-3)));
    status = Status::Success;

    return status;
}

CLamb::NodeLamb::Status CLamb::IsAtRandomPosition::update(){
    if(lamb->distanceToPoint(lamb->random_pos) < lamb->threshold_distance)
        status = Status::Success;
    else
        status = Status::Failure;

    return status;

}

void CLamb::Walk::initialize(){
    lamb->stats[walk] = 300;
}

CLamb::NodeLamb::Status CLamb::Walk::update(){
    if(--lamb->stats[walk] <= 0){
        status = Status::Success;
    }
    else{
        status = Status::Running;
        lamb->speed_factor = 0.3;
        lamb->GoToPoint(lamb->random_pos);
    }
    return status;
}

void CLamb::Walk::terminate(Status s){
    if(status == Status::Running){
        lamb->Stop();
        status = Status::Success;
    }

}

void CLamb::Wait::initialize(){
    //TODO la duracion debe ser sacada de los datos
    lamb->Stop();
    lamb->stats[walk] = 500;

}

CLamb::NodeLamb::Status CLamb::Wait::update(){
    if(--lamb->stats[walk] <= 0)
        status = Status::Success;
    else
        status = Status::Running;

    return status;
}
