#include "galib_lamb.h"
//Nodos del behavior trees
//TODO son muy repetitivos, podria aunar algunos
    CGalibLamb::NodeLamb::Status CGalibLamb::NeedWater::update(){
        if(lamb->current_priority == water){
            // lamb->leds->SetAllColors(CColor::RED);
            return Status::Success;
        }
        return Status::Failure;
    }

    CGalibLamb::NodeLamb::Status CGalibLamb::NeedFood::update(){
        if(lamb->current_priority == food)
            return Status::Success;
        return Status::Failure;
    }

    CGalibLamb::NodeLamb::Status CGalibLamb::NeedRest::update(){
        if(lamb->current_priority == rest)
            return Status::Success;
        return Status::Failure;
    }

    CGalibLamb::NodeLamb::Status CGalibLamb::CanDrink::update(){
        if(lamb->IsInPlace( lamb->GetClosestPoint(&(lamb->water_troughs))))
            return Status::Success;
        return Status::Failure;
    }

    CGalibLamb::NodeLamb::Status CGalibLamb::CanEat::update(){
        if(lamb->IsInPlace( lamb->GetClosestPoint(&(lamb->food_troughs))))
            return Status::Success;
        return Status::Failure;
    }

    CGalibLamb::NodeLamb::Status CGalibLamb::CanSleep::update(){
        if(lamb->IsInPlace( lamb->GetClosestPoint(&(lamb->beds))))
            return Status::Success;
        return Status::Failure;
    }

    CGalibLamb::NodeLamb::Status CGalibLamb::GoToWater::update(){
        CVector2 water = lamb->GetClosestPoint(&(lamb->water_troughs));
        lamb->GoTo(water);
        return Status::Running;
    }

    CGalibLamb::NodeLamb::Status CGalibLamb::GoToFood::update(){
        CVector2 food = lamb->GetClosestPoint(&(lamb->food_troughs));
        lamb->GoTo(food);
        return Status::Running;
    }

    CGalibLamb::NodeLamb::Status CGalibLamb::GoToBed::update(){
        CVector2 bed = lamb->GetClosestPoint(&(lamb->beds));
        lamb->GoTo(bed);
        return Status::Running;
    }

    void CGalibLamb::GoToWater::terminate(Status s){
        if(status == Status::Running){
            lamb->Stop();
            status = Status::Success;
        }
    }

    void CGalibLamb::GoToFood::terminate(Status s){
        if(status == Status::Running){
            lamb->Stop();
            status = Status::Success;
        }
    }

    void CGalibLamb::GoToBed::terminate(Status s){
        if(status == Status::Running){
            lamb->Stop();
            status = Status::Success;
        }
    }

    CGalibLamb::NodeLamb::Status CGalibLamb::Drink::update(){
        lamb->stats[water] += 30;
        if(lamb->stats[water] > STAT_BAD)
            return Status::Success;
        return Status::Running;
    }
    CGalibLamb::NodeLamb::Status CGalibLamb::Eat::update(){
        lamb->stats[food] += 30;
        if(lamb->stats[food] > STAT_BAD)
            return Status::Success;
        return Status::Running;
    }
    CGalibLamb::NodeLamb::Status CGalibLamb::Sleep::update(){
        lamb->stats[rest] += 5;
        if(lamb->stats[rest] > STAT_BAD)
            return Status::Success;
        return Status::Running;
    }

    void CGalibLamb::Drink::terminate(Status s){
            status = Status::Success;
    }
    void CGalibLamb::Eat::terminate(Status s){
            status = Status::Success;
    }
    void CGalibLamb::Sleep::terminate(Status s){
            status = Status::Success;
    }

    CGalibLamb::NodeLamb::Status CGalibLamb::SelectRandomPos::update(){
        //TODO hardcoded el tamaño de la arena
        lamb->random_pos = CVector2(lamb->rng->Uniform(CRange<Real>(-1.75,1.75)),
                            lamb->rng->Uniform(CRange<Real>(-1.75,1.75)));
        return Status::Success;
    }

    CGalibLamb::NodeLamb::Status CGalibLamb::IsAtRandomPos::update(){
        if(lamb->IsInPlace( lamb->random_pos)){
            lamb->stats[walk] = STAT_FULL;
            return Status::Success;
        }
        return Status::Failure;
    }

    CGalibLamb::NodeLamb::Status CGalibLamb::GoToRandomPos::update(){
        lamb->GoTo(lamb->random_pos);
        return Status::Running;
    }

    void CGalibLamb::GoToRandomPos::terminate(Status s){
        if(status == Status::Running){
            lamb->Stop();
            status = Status::Success;
        }
    }
