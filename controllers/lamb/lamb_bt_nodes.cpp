#include "lamb.h"
//Nodos del behavior trees
//TODO son muy repetitivos, podria aunar algunos
    CLamb::NodeLamb::Status CLamb::NeedWater::update(){
        if(lamb->current_priority == water){
            // lamb->leds->SetAllColors(CColor::RED);
            return Status::Success;
        }
        return Status::Failure;
    }

    CLamb::NodeLamb::Status CLamb::NeedFood::update(){
        if(lamb->current_priority == food)
            return Status::Success;
        return Status::Failure;
    }

    CLamb::NodeLamb::Status CLamb::NeedRest::update(){
        if(lamb->current_priority == rest)
            return Status::Success;
        return Status::Failure;
    }

    CLamb::NodeLamb::Status CLamb::CanDrink::update(){
        if(lamb->IsInPlace( lamb->GetClosestPoint(&(lamb->water_troughs))))
            return Status::Success;
        return Status::Failure;
    }

    CLamb::NodeLamb::Status CLamb::CanEat::update(){
        if(lamb->IsInPlace( lamb->GetClosestPoint(&(lamb->food_troughs))))
            return Status::Success;
        return Status::Failure;
    }

    CLamb::NodeLamb::Status CLamb::CanSleep::update(){
        if(lamb->IsInPlace( lamb->GetClosestPoint(&(lamb->beds))))
            return Status::Success;
        return Status::Failure;
    }

    CLamb::NodeLamb::Status CLamb::GoToWater::update(){
        CVector2 water = lamb->GetClosestPoint(&(lamb->water_troughs));
        lamb->GoTo(water);
        return Status::Running;
    }

    CLamb::NodeLamb::Status CLamb::GoToFood::update(){
        CVector2 food = lamb->GetClosestPoint(&(lamb->food_troughs));
        lamb->GoTo(food);
        return Status::Running;
    }

    CLamb::NodeLamb::Status CLamb::GoToBed::update(){
        CVector2 bed = lamb->GetClosestPoint(&(lamb->beds));
        lamb->GoTo(bed);
        return Status::Running;
    }

    void CLamb::GoToWater::terminate(Status s){
        if(status == Status::Running){
            lamb->Stop();
            status = Status::Success;
        }
    }

    void CLamb::GoToFood::terminate(Status s){
        if(status == Status::Running){
            lamb->Stop();
            status = Status::Success;
        }
    }

    void CLamb::GoToBed::terminate(Status s){
        if(status == Status::Running){
            lamb->Stop();
            status = Status::Success;
        }
    }

    CLamb::NodeLamb::Status CLamb::Drink::update(){
        lamb->stats[water] += 30;
        if(lamb->stats[water] > STAT_BAD)
            return Status::Success;
        return Status::Running;
    }
    CLamb::NodeLamb::Status CLamb::Eat::update(){
        lamb->stats[food] += 30;
        if(lamb->stats[food] > STAT_BAD)
            return Status::Success;
        return Status::Running;
    }
    CLamb::NodeLamb::Status CLamb::Sleep::update(){
        lamb->stats[rest] += 5;
        if(lamb->stats[rest] > STAT_BAD)
            return Status::Success;
        return Status::Running;
    }

    void CLamb::Drink::terminate(Status s){
            status = Status::Success;
    }
    void CLamb::Eat::terminate(Status s){
            status = Status::Success;
    }
    void CLamb::Sleep::terminate(Status s){
            status = Status::Success;
    }

    CLamb::NodeLamb::Status CLamb::SelectRandomPos::update(){
        //TODO hardcoded el tamaño de la arena
        lamb->random_pos = CVector2(lamb->rng->Uniform(CRange<Real>(-1.75,1.75)),
                            lamb->rng->Uniform(CRange<Real>(-1.75,1.75)));
        return Status::Success;
    }

    CLamb::NodeLamb::Status CLamb::IsAtRandomPos::update(){
        if(lamb->IsInPlace( lamb->random_pos)){
            lamb->stats[walk] = STAT_FULL;
            return Status::Success;
        }
        return Status::Failure;
    }

    CLamb::NodeLamb::Status CLamb::GoToRandomPos::update(){
        lamb->GoTo(lamb->random_pos);
        return Status::Running;
    }

    void CLamb::GoToRandomPos::terminate(Status s){
        if(status == Status::Running){
            lamb->Stop();
            status = Status::Success;
        }
    }
