//Cadmium Simulator headers
#include <cadmium/modeling/ports.hpp>
#include <cadmium/modeling/dynamic_model.hpp>
#include <cadmium/modeling/dynamic_model_translator.hpp>
#include <cadmium/engine/pdevs_dynamic_runner.hpp>
#include <cadmium/logger/common_loggers.hpp>

//Time class header
#include <NDTime.hpp>

//Atomic model headers
#include <cadmium/basic_model/pdevs/iestream.hpp> //Atomic model for inputs
#include "../atomics/applicationProcess.hpp"

//C++ libraries
#include <iostream>
#include <string>

using namespace std;
using namespace cadmium;
using namespace cadmium::basic_models::pdevs;

using TIME = NDTime;

/***** Define input port for coupled models *****/
    struct applicationProcessIN : public cadmium::in_port<int> { };
    
/***** Define output ports for coupled model *****/
struct appProcessOUT : public cadmium::out_port<int> {};
 struct appProcessStatus : public cadmium::out_port<int> { };
 
/****** Input Reader atomic model declaration *******************/

template<typename T>
class InputReader_Int : public iestream_input<int,T> {
    public:
        InputReader_Int () = default;
        InputReader_Int (const char* file_path) : iestream_input<int,T>(file_path) {}
};


int main(){


    /****** Input Reader atomic models instantiation *******************/
    const char * i_input_data_applicationProcessIN = "../input_data/applicationProcess_input.txt";
    shared_ptr<dynamic::modeling::model> input_reader_applicationProcessIN = dynamic::translate::make_dynamic_atomic_model<InputReader_Int, TIME, const char* >("input_reader_applicationProcessIN" , move(i_input_data_applicationProcessIN));


    /****** Application atomic model instantiation *******************/
    shared_ptr<dynamic::modeling::model> application1 = dynamic::translate::make_dynamic_atomic_model<ApplicationProcess, TIME>("application1");

    /*******TOP MODEL********/
    dynamic::modeling::Ports iports_TOP = {};
    dynamic::modeling::Ports oports_TOP = {typeid(appProcessOUT),typeid(appProcessStatus)};
    dynamic::modeling::Models submodels_TOP = {input_reader_applicationProcessIN, application1};
    dynamic::modeling::EICs eics_TOP = {};
    dynamic::modeling::EOCs eocs_TOP = {
        dynamic::translate::make_EOC< ApplicationProcess_defs::applicationProcessOUT,appProcessOUT>("application1"),
        dynamic::translate::make_EOC< ApplicationProcess_defs::applicationProcessStatus,appProcessStatus>("application1")
    };
    dynamic::modeling::ICs ics_TOP = {
        dynamic::translate::make_IC<iestream_input_defs<int>::out,ApplicationProcess_defs::applicationProcessIN>("input_reader_applicationProcessIN","application1"),
	};
    shared_ptr<dynamic::modeling::coupled<TIME>> TOP;
    TOP = make_shared<dynamic::modeling::coupled<TIME>>(
        "TOP", submodels_TOP, iports_TOP, oports_TOP, eics_TOP, eocs_TOP, ics_TOP
    );

    /*************** Loggers *******************/
    static ofstream out_messages("../simulation_results/applicationProcess_test_output_messages.txt");
    struct oss_sink_messages{
        static ostream& sink(){          
            return out_messages;
        }
    };
    static ofstream out_state("../simulation_results/applicationProcess_test_output_state.txt");
    struct oss_sink_state{
        static ostream& sink(){          
            return out_state;
        }
    };
    
    using state=logger::logger<logger::logger_state, dynamic::logger::formatter<TIME>, oss_sink_state>;
    using log_messages=logger::logger<logger::logger_messages, dynamic::logger::formatter<TIME>, oss_sink_messages>;
    using global_time_mes=logger::logger<logger::logger_global_time, dynamic::logger::formatter<TIME>, oss_sink_messages>;
    using global_time_sta=logger::logger<logger::logger_global_time, dynamic::logger::formatter<TIME>, oss_sink_state>;

    using logger_top=logger::multilogger<state, log_messages, global_time_mes, global_time_sta>;

    /************** Runner call ************************/ 
    dynamic::engine::runner<NDTime, logger_top> r(TOP, {0});
    r.run_until(NDTime("04:00:00:000"));
    return 0;
}
