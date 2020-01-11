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
#include "../atomics/application.hpp"

//C++ libraries
#include <iostream>
#include <string>

using namespace std;
using namespace cadmium;
using namespace cadmium::basic_models::pdevs;

using TIME = NDTime;

/***** Define input port for coupled models *****/
    struct applicationFillingComplete : public cadmium::in_port<int> { };
    struct applicationIN : public cadmium::in_port<int> { };

/***** Define output ports for coupled model *****/
struct appOUT : public cadmium::out_port<int> {};

/****** Input Reader atomic model declaration *******************/

template<typename T>
class InputReader_Int : public iestream_input<int,T> {
    public:
        InputReader_Int () = default;
        InputReader_Int (const char* file_path) : iestream_input<int,T>(file_path) {}
};

template<typename T>
class InputReader_Int2 : public iestream_input<int,T> {
    public:
        InputReader_Int2 () = default;
        InputReader_Int2 (const char* file_path) : iestream_input<int,T>(file_path) {}
};


int main(){


    /****** Input Reader atomic models instantiation *******************/
    const char * i_input_data_applicationIN = "../input_data/application_input_applicationIN.txt";
    shared_ptr<dynamic::modeling::model> input_reader_applicationIN = dynamic::translate::make_dynamic_atomic_model<InputReader_Int, TIME, const char* >("input_reader_applicationIN" , move(i_input_data_applicationIN));

    const char * i_input_data_applicationFillingComplete = "../input_data/application_input_applicationFillingComplete.txt";
    shared_ptr<dynamic::modeling::model> input_reader_applicationFillingComplete = dynamic::translate::make_dynamic_atomic_model<InputReader_Int2 , TIME, const char* >("input_reader_applicationFillingComplete" , move(i_input_data_applicationFillingComplete));


    /****** Application atomic model instantiation *******************/
    shared_ptr<dynamic::modeling::model> application1 = dynamic::translate::make_dynamic_atomic_model<Application, TIME>("application1");

    /*******TOP MODEL********/
    dynamic::modeling::Ports iports_TOP = {};
    dynamic::modeling::Ports oports_TOP = {typeid(appOUT)};
    dynamic::modeling::Models submodels_TOP = {input_reader_applicationIN, input_reader_applicationFillingComplete, application1};
    dynamic::modeling::EICs eics_TOP = {};
    dynamic::modeling::EOCs eocs_TOP = {
        dynamic::translate::make_EOC< Application_defs::applicationOUT,appOUT>("application1")
    };
    dynamic::modeling::ICs ics_TOP = {
        dynamic::translate::make_IC<iestream_input_defs<int>::out,Application_defs::applicationIN>("input_reader_applicationIN","application1"),
        dynamic::translate::make_IC<iestream_input_defs<int>::out,Application_defs::applicationFillingComplete>("input_reader_applicationFillingComplete","application1")
    };
    shared_ptr<dynamic::modeling::coupled<TIME>> TOP;
    TOP = make_shared<dynamic::modeling::coupled<TIME>>(
        "TOP", submodels_TOP, iports_TOP, oports_TOP, eics_TOP, eocs_TOP, ics_TOP
    );

    /*************** Loggers *******************/
    static ofstream out_messages("../simulation_results/application_test_output_messages.txt");
    struct oss_sink_messages{
        static ostream& sink(){          
            return out_messages;
        }
    };
    static ofstream out_state("../simulation_results/application_test_output_state.txt");
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
    r.run_until(NDTime("08:00:00:000"));
    return 0;
}
