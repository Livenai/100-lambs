#include "footbot_lamb.h"
//Nodos del behavior trees
//TODO son muy repetitivos, podria aunar algunos
    CFootBotLamb::NodeFootBot::Status CFootBotLamb::NeedWater::update(){
        if(lamb->current_priority == water){
            // lamb->leds->SetAllColors(CColor::RED);
            return Status::Success;
        }
        return Status::Failure;
    }

    CFootBotLamb::NodeFootBot::Status CFootBotLamb::NeedFood::update(){
        if(lamb->current_priority == food)
            return Status::Success;
        return Status::Failure;
    }

    CFootBotLamb::NodeFootBot::Status CFootBotLamb::NeedRest::update(){
        if(lamb->current_priority == rest)
            return Status::Success;
        return Status::Failure;
    }

    CFootBotLamb::NodeFootBot::Status CFootBotLamb::CanDrink::update(){
        if(lamb->IsInPlace( lamb->GetClosestPoint(&(lamb->water_troughs))))
            return Status::Success;
        return Status::Failure;
    }

    CFootBotLamb::NodeFootBot::Status CFootBotLamb::CanEat::update(){
        if(lamb->IsInPlace( lamb->GetClosestPoint(&(lamb->food_troughs))))
            return Status::Success;
        return Status::Failure;
    }

    CFootBotLamb::NodeFootBot::Status CFootBotLamb::CanSleep::update(){
        if(lamb->IsInPlace( lamb->GetClosestPoint(&(lamb->beds))))
            return Status::Success;
        return Status::Failure;
    }

    CFootBotLamb::NodeFootBot::Status CFootBotLamb::GoToWater::update(){
        CVector2 water = lamb->GetClosestPoint(&(lamb->water_troughs));
        lamb->GoTo(water);
        return Status::Running;
    }

    CFootBotLamb::NodeFootBot::Status CFootBotLamb::GoToFood::update(){
        CVector2 food = lamb->GetClosestPoint(&(lamb->food_troughs));
        lamb->GoTo(food);
        return Status::Running;
    }

    CFootBotLamb::NodeFootBot::Status CFootBotLamb::GoToBed::update(){
        CVector2 bed = lamb->GetClosestPoint(&(lamb->beds));
        lamb->GoTo(bed);
        return Status::Running;
    }

    void CFootBotLamb::GoToWater::terminate(Status s){
        if(status == Status::Running){
            lamb->Stop();
            status = Status::Success;
        }
    }

    void CFootBotLamb::GoToFood::terminate(Status s){
        if(status == Status::Running){
            lamb->Stop();
            status = Status::Success;
        }
    }

    void CFootBotLamb::GoToBed::terminate(Status s){
        if(status == Status::Running){
            lamb->Stop();
            status = Status::Success;
        }
    }

    CFootBotLamb::NodeFootBot::Status CFootBotLamb::Drink::update(){
        lamb->stats[water] += 30;
        if(lamb->stats[water] > STAT_BAD)
            return Status::Success;
        return Status::Running;
    }
    CFootBotLamb::NodeFootBot::Status CFootBotLamb::Eat::update(){
        lamb->stats[food] += 30;
        if(lamb->stats[food] > STAT_BAD)
            return Status::Success;
        return Status::Running;
    }
    CFootBotLamb::NodeFootBot::Status CFootBotLamb::Sleep::update(){
        lamb->stats[rest] += 5;
        if(lamb->stats[rest] > STAT_BAD)
            return Status::Success;
        return Status::Running;
    }

    void CFootBotLamb::Drink::terminate(Status s){
            status = Status::Success;
    }
    void CFootBotLamb::Eat::terminate(Status s){
            status = Status::Success;
    }
    void CFootBotLamb::Sleep::terminate(Status s){
            status = Status::Success;
    }

    CFootBotLamb::NodeFootBot::Status CFootBotLamb::SelectRandomPos::update(){
        //TODO hardcoded el tamaño de la arena
        lamb->random_pos = CVector2(lamb->rng->Uniform(CRange<Real>(-1.75,1.75)),
                            lamb->rng->Uniform(CRange<Real>(-1.75,1.75)));
        return Status::Success;
    }

    CFootBotLamb::NodeFootBot::Status CFootBotLamb::IsAtRandomPos::update(){
        if(lamb->IsInPlace( lamb->random_pos)){
            lamb->stats[walk] = STAT_FULL;
            return Status::Success;
        }
        return Status::Failure;
    }

    CFootBotLamb::NodeFootBot::Status CFootBotLamb::GoToRandomPos::update(){
        lamb->GoTo(lamb->random_pos);
        return Status::Running;
    }

    void CFootBotLamb::GoToRandomPos::terminate(Status s){
        if(status == Status::Running){
            lamb->Stop();
            status = Status::Success;
        }
    }
