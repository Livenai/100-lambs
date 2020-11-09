#define NODE_0_DECLARATION(NODE_NAME) \
class NODE_NAME: public NodeFootBot{\
    public:    \
    name(CFootBotLamb * lamb):NodeFootBot(lamb){}\
    Status update() override;\
};

#define NODE_1_DECLARATION(NODE_NAME) \
class NODE_NAME: public NodeFootBot{\
    public:    \
    name(CFootBotLamb * lamb):NodeFootBot(lamb){}\
    Status update() override;\
    void terminate(Status s) override;\
};

NODE_0_DECLARATION(NeedWater)
NODE_0_DECLARATION(CanDrink)
NODE_1_DECLARATION(GoToWater)
NODE_0_DECLARATION(Drink)

NODE_0_DECLARATION(NeedFood)
NODE_0_DECLARATION(CanEat)
NODE_1_DECLARATION(GoToFood)
NODE_0_DECLARATION(Eat)

NODE_0_DECLARATION(NeedRest)
NODE_0_DECLARATION(CanSleep)
NODE_1_DECLARATION(GoToBed)
NODE_0_DECLARATION(Sleep)

#undef NODE_0_DECLARATION
#undef NODE_1_DECLARATION
