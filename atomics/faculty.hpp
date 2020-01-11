

#ifndef __FACULTY_HPP__
#define __FACULTY_HPP__

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
struct Faculty_defs{
	
    struct facultyOUT : public out_port<int> {};
    struct advisorOpinionOUT : public out_port<int> { };
    struct applicationCourseCheck : public in_port<int> { };
   
};

template<typename TIME> class Faculty{
    public:
        //Parameters to be overwriten when instantiating the atomic model
        TIME   facultyTime;
        // default constructor
        Faculty() noexcept{
          facultyTime  = TIME("00:00:10");
          state.next_internal    = std::numeric_limits<TIME>::infinity();
          state.faculty_state     = false;
        }
        
        // state definition
        struct state_type{
			int checkNum;
			int facultyRejected;
			int FacOut;
			int checkFacultyNum;
			bool qualify;
            bool faculty_state;
            TIME next_internal;
        }; 
        state_type state;
        // ports definition
        using input_ports=std::tuple<typename Faculty_defs::applicationCourseCheck>;
        using output_ports=std::tuple<typename Faculty_defs::facultyOUT,typename Faculty_defs::advisorOpinionOUT>;

        // internal transition
        void internal_transition() {
         state.faculty_state = false;
		//  state.next_internal = std::numeric_limits<TIME>::infinity();
        }

        // external transition
        void external_transition(TIME e, typename make_message_bags<input_ports>::type mbs) { 
            if((get_messages<typename Faculty_defs::applicationCourseCheck>(mbs).size())>1) 
                    assert(false && "one message per time uniti");
            for(const auto &x : get_messages<typename Faculty_defs::applicationCourseCheck>(mbs)){
                if(state.faculty_state == false){
                    state.checkFacultyNum = x;
                   state.checkNum = state.checkFacultyNum & '?' ;
					if (state.checkNum != '1')
						{
						state.FacOut=state.checkNum;
						state.qualify = true;
						state.faculty_state = true;
                        state.next_internal = facultyTime;
						}
					else{
						//state.facultyRejected= state.checkNum;
						state.qualify = false;
						state.faculty_state = true;
                        state.next_internal = facultyTime;
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
			if (state.qualify)
			{	
				get_messages<typename Faculty_defs::facultyOUT>(bags).push_back(state.checkFacultyNum);
			}
			else // if you have recommendations !!!!!
			{
				if ((double)rand() / (double) RAND_MAX  > 0.70)
				{
					get_messages<typename Faculty_defs::facultyOUT>(bags).push_back(state.checkFacultyNum);
				}
				else
		
				get_messages<typename Faculty_defs::advisorOpinionOUT>(bags).push_back(state.checkFacultyNum);

			}
           
            return bags;
        }

        // time_advance function
        TIME time_advance() const {  
             return state.next_internal;
        }

        friend std::ostringstream& operator<<(std::ostringstream& os, const typename Faculty<TIME>::state_type& i) {
          os << "facultyOUT: " << i.FacOut ; 
         
        return os;
        }
};     
#endif // __FACULTY_HPP__	