

#ifndef __REGISTRAR_HPP__
#define __REGISTRAR_HPP__

#include <cadmium/modeling/ports.hpp>
#include <cadmium/modeling/message_bag.hpp>

#include <limits>
#include <assert.h>
#include <string>
#include <random>
#include <time.h>    // time()
//#include "../data_structures/message.hpp"

using namespace cadmium;
using namespace std;

//Port definition
struct Registrar_defs{
	
    struct minStandardsNotMet : public out_port<int> {};
    struct applicationVerificationOUT : public out_port<int> { };
    struct applicationVerificationIN : public in_port<int> { };
   
};

template<typename TIME> class Registrar{
    public:
        //Parameters to be overwriten when instantiating the atomic model
        TIME   registrarTime;
        // default constructor
        Registrar() noexcept{
          registrarTime  = TIME("00:00:09");
          state.next_internal    = std::numeric_limits<TIME>::infinity();
          state.registrar_state     = false;
        }
        
        // state definition
        struct state_type{
			int  studentGrade;
			int  checkNum;
			bool lowGrade;
            bool registrar_state;
            TIME next_internal;
        }; 
        state_type state;
        // ports definition
        using input_ports=std::tuple<typename Registrar_defs::applicationVerificationIN>;
        using output_ports=std::tuple<typename Registrar_defs::minStandardsNotMet,typename Registrar_defs::applicationVerificationOUT>;

        // internal transition
        void internal_transition() {
         state.registrar_state = false;
		//  state.next_internal = std::numeric_limits<TIME>::infinity();
        }

        // external transition
        void external_transition(TIME e, typename make_message_bags<input_ports>::type mbs) { 
            if((get_messages<typename Registrar_defs::applicationVerificationIN>(mbs).size())>1) 
                    assert(false && "one message per time uniti");
            for(const auto &x : get_messages<typename Registrar_defs::applicationVerificationIN>(mbs)){
                if(state.registrar_state == false){
                    state.studentGrade = x;
                  state.checkNum = state.studentGrade & 30 ;
					if( state.checkNum > 25 )
					{
						state.lowGrade = true;
						state.registrar_state = true;
						state.next_internal = registrarTime;
					}
					else{

						state.lowGrade = false;
						state.registrar_state = true;
						state.next_internal = registrarTime;
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
			
			if ( state.lowGrade )
			{	
				
				get_messages<typename Registrar_defs::minStandardsNotMet>(bags).push_back(state.studentGrade);
			}
			else
			get_messages<typename Registrar_defs::applicationVerificationOUT>(bags).push_back(state.studentGrade);
	
			
            return bags;
        }

        // time_advance function
        TIME time_advance() const {  
             return state.next_internal;
        }

        friend std::ostringstream& operator<<(std::ostringstream& os, const typename Registrar<TIME>::state_type& i) {
            os << "studentGrade: " << i.studentGrade; 
        return os;
        }
};     
#endif // __REGISTRAR_HPP__	