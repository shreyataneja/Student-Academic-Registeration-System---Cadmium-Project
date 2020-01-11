

#ifndef __APPLICATIONPROCESS_HPP__
#define __APPLICATIONPROCESS_HPP__

#include <cadmium/modeling/ports.hpp>
#include <cadmium/modeling/message_bag.hpp>

#include <limits>
#include <assert.h>
#include <string>
#include <random>
#include <time.h>   

//#include "../data_structures/message.hpp"

using namespace cadmium;
using namespace std;

//Port definition
struct ApplicationProcess_defs{
	
    struct applicationProcessOUT : public out_port<int> {};
    struct applicationProcessStatus : public out_port<int> { };
    struct applicationProcessIN : public in_port<int> { };
   
};

template<typename TIME> class ApplicationProcess{
    public:
        //Parameters to be overwriten when instantiating the atomic model
        TIME   applicationProcessTime;
        // default constructor
        ApplicationProcess() noexcept{
          applicationProcessTime  = TIME("00:00:05");
          state.next_internal1    = std::numeric_limits<TIME>::infinity();
          state.model_state     = false;
        }
        
        // state definition
        struct state_type{
          	int tempNum;
			int checkNum;
            bool model_state;
            TIME next_internal1;
        }; 
        state_type state;
        // ports definition
        using input_ports=std::tuple<typename ApplicationProcess_defs::applicationProcessIN>;
        using output_ports=std::tuple<typename ApplicationProcess_defs::applicationProcessOUT, typename ApplicationProcess_defs::applicationProcessStatus>;

        // internal transition
        void internal_transition() {
                
                    state.model_state = false;
               //    state.next_internal = std::numeric_limits<TIME>::infinity();
              
        }

        // external transition
        void external_transition(TIME e, typename make_message_bags<input_ports>::type mbs) { 
            if((get_messages<typename ApplicationProcess_defs::applicationProcessIN>(mbs).size())>1) 
                    assert(false && "one message per time uniti");
            for(const auto &x : get_messages<typename ApplicationProcess_defs::applicationProcessIN>(mbs)){
                if(state.model_state == false){
                    state.tempNum = x;
                    
					srand((unsigned)time(0));
					// 70 % probability that students choose to submit their appication
					bool choice = ((rand() % 100) < 70) ? true : false;
					//std::cout << "	choice output: " << choice;

					if(choice == 1)
					{
						state.checkNum = 1;
					}
					else {
						state.checkNum = 0;
					}
					 state.model_state = true;
					  state.next_internal1 = applicationProcessTime;
					
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
           
           if (state.checkNum == 1)
			{
               
                get_messages<typename ApplicationProcess_defs::applicationProcessOUT>(bags).push_back(state.tempNum);
				get_messages<typename ApplicationProcess_defs::applicationProcessStatus>(bags).push_back(state.tempNum);
           
			} 
			else
				get_messages<typename ApplicationProcess_defs::applicationProcessStatus>(bags).push_back(state.tempNum);
           
            return bags;
        }

        // time_advance function
        TIME time_advance() const {  
             return state.next_internal1;
        }

        friend std::ostringstream& operator<<(std::ostringstream& os, const typename ApplicationProcess<TIME>::state_type& i) {
            os << "ApplicationProcessOUT: " << i.tempNum ; 
        return os;
        }
};     
#endif // __APPLICATIONPROCESS_HPP__	