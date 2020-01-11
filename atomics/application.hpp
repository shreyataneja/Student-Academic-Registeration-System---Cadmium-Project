

#ifndef __APPLICATION_HPP__
#define __APPLICATION_HPP__

#include <cadmium/modeling/ports.hpp>
#include <cadmium/modeling/message_bag.hpp>

#include <limits>
#include <assert.h>
#include <string>
#include <random>

//#include "../data_structures/message.hpp"

using namespace cadmium;
using namespace std;

//Port definition
struct Application_defs{
	
    struct applicationOUT : public out_port<int> {};
    struct applicationFillingComplete : public in_port<int> { };
    struct applicationIN : public in_port<int> { };
   
};

template<typename TIME> class Application{
    public:
        //Parameters to be overwriten when instantiating the atomic model
        TIME   applicationTime;
		TIME   timeout;
        // default constructor
        Application() noexcept{
          applicationTime  = TIME("00:00:05");
		  timeout          = TIME("00:00:20");
          state.next_internal    = std::numeric_limits<TIME>::infinity();
          state.model_active     = false;
        }
        
        // state definition
        struct state_type{
           int applicationNum;
		   int totalApplicationNum;
			bool done;
			bool sendApplication;
            bool model_active;
            TIME next_internal;
        }; 
        state_type state;
        // ports definition
        using input_ports=std::tuple<typename Application_defs::applicationIN, typename Application_defs::applicationFillingComplete>;
        using output_ports=std::tuple<typename Application_defs::applicationOUT>;

        // internal transition
        void internal_transition() {
            if (state.done){
                if (state.applicationNum < state.totalApplicationNum)
				{
                state.applicationNum ++;
                state.done = false;
               
                state.sendApplication = true;
                state.model_active = true; 
                state.next_internal = applicationTime;   
                } else {
                    state.model_active = false;
                    state.next_internal = std::numeric_limits<TIME>::infinity();
                }
            } else{
                 if (state.sendApplication){
                    state.sendApplication = false;
                    state.model_active = true;
                    state.next_internal = timeout;
                } else {
                    state.sendApplication = true;
                    state.model_active = true;
                    state.next_internal = applicationTime;    
                } 
            }   
        }

        // external transition
        void external_transition(TIME e, typename make_message_bags<input_ports>::type mbs) { 
            if((get_messages<typename Application_defs::applicationIN>(mbs).size()+get_messages<typename Application_defs::applicationFillingComplete>(mbs).size())>1) 
                    assert(false && "one message per time uniti");
            for(const auto &x : get_messages<typename Application_defs::applicationIN>(mbs)){
                if(state.model_active == false){
                    state.totalApplicationNum = x;
                    if (state.totalApplicationNum > 0){
                        state.applicationNum = 1;
                        state.done = false;
                        state.sendApplication = true;
                     
                        state.model_active = true;
                        state.next_internal = applicationTime;
                    }else{
                        if(state.next_internal != std::numeric_limits<TIME>::infinity()){
                            state.next_internal = state.next_internal - e;
                        }
                    }
                }
            }
            for(const auto &x : get_messages<typename Application_defs::applicationFillingComplete>(mbs)){
                if(state.model_active == true) { 
						state.done = true;
                        state.sendApplication = false;
                        state.next_internal = TIME("00:00:00");
                }else{
                        if(state.next_internal != std::numeric_limits<TIME>::infinity()){
                            state.next_internal = state.next_internal - e;
                        }
                    }
            }              
        }

        // confluence transition
        void confluence_transition(TIME e, typename make_message_bags<input_ports>::type mbs) {
            internal_transition();
            external_transition(TIME(), std::move(mbs));
        }

        // output function
        typename make_message_bags<output_ports>::type output() const {
            typename make_message_bags<output_ports>::type bags;
           
            if (state.sendApplication){
               
               get_messages<typename Application_defs::applicationOUT>(bags).push_back(state.applicationNum);
            }    
			else{
                if (state.done){
                    get_messages<typename Application_defs::applicationOUT>(bags).push_back(state.applicationNum);
                }
            }    
            return bags;
        }

        // time_advance function
        TIME time_advance() const {  
             return state.next_internal;
        }

        friend std::ostringstream& operator<<(std::ostringstream& os, const typename Application<TIME>::state_type& i) {
            os << "applicationNum: " << i.applicationNum << " & totalApplicationNum: " << i.totalApplicationNum; 
        return os;
        }
};     
#endif // __Application_HPP__