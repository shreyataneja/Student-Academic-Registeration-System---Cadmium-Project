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
#include "../atomics/applicationProcess.hpp"


//C++ headers
#include <iostream>
#include <chrono>
#include <algorithm>
#include <string>


using namespace std;
using namespace cadmium;
using namespace cadmium::basic_models::pdevs;

using TIME = NDTime;

/***** Define input port for coupled models *****/
struct webAppIN  : public in_port<int>{};

/***** Define output ports for coupled model *****/
struct webAppOUT : public out_port<int>{};

/****** Input Reader atomic model declaration *******************/
template<typename T>
class InputReader_Int : public iestream_input<int,T> {
public:
    InputReader_Int() = default;
    InputReader_Int(const char* file_path) : iestream_input<int,T>(file_path) {}
};

int main() {

 
    /****** Input Reader atomic model instantiation *******************/
   // string input = argv[1];
    const char * i_input = "../input_data/webApp_input.txt";
    shared_ptr<dynamic::modeling::model> input_reader = dynamic::translate::make_dynamic_atomic_model<InputReader_Int, TIME, const char* >("input_reader" , move(i_input));

    /****** Application atomic model instantiation *******************/
    shared_ptr<dynamic::modeling::model> application1 = dynamic::translate::make_dynamic_atomic_model<Application, TIME>("application1");

    /****** ApplicationProcess atomic model instantiation *******************/
    shared_ptr<dynamic::modeling::model> applicationProcess1 = dynamic::translate::make_dynamic_atomic_model<ApplicationProcess, TIME>("applicationProcess1");


    /*******webApp SIMULATOR COUPLED MODEL********/
    dynamic::modeling::Ports iports_webApp = {typeid(webAppIN)};
    dynamic::modeling::Ports oports_webApp = {typeid(webAppOUT)};
    dynamic::modeling::Models submodels_webApp = {application1, applicationProcess1};
    dynamic::modeling::EICs eics_webApp = {
        cadmium::dynamic::translate::make_EIC<webAppIN, Application_defs::applicationIN>("application1")
    };
    dynamic::modeling::EOCs eocs_webApp = {
        dynamic::translate::make_EOC<ApplicationProcess_defs::applicationProcessOUT,webAppOUT>("applicationProcess1")
    };
    dynamic::modeling::ICs ics_webApp = {
        dynamic::translate::make_IC<Application_defs::applicationOUT, ApplicationProcess_defs::applicationProcessIN>("application1","applicationProcess1"),
        dynamic::translate::make_IC<ApplicationProcess_defs::applicationProcessStatus, Application_defs::applicationFillingComplete>("applicationProcess1","application1")
		};
    shared_ptr<dynamic::modeling::coupled<TIME>> webApp;
    webApp = make_shared<dynamic::modeling::coupled<TIME>>(
        "webApp", submodels_webApp, iports_webApp, oports_webApp, eics_webApp, eocs_webApp, ics_webApp 
    );


    /*******TOP COUPLED MODEL********/
    dynamic::modeling::Ports iports_TOP = {};
    dynamic::modeling::Ports oports_TOP = {typeid(webAppOUT)};
    dynamic::modeling::Models submodels_TOP = {input_reader, webApp};
    dynamic::modeling::EICs eics_TOP = {};
     dynamic::modeling::EOCs eocs_TOP = {
        dynamic::translate::make_EOC<webAppOUT,webAppOUT>("webApp")
    };
    dynamic::modeling::ICs ics_TOP = {
        dynamic::translate::make_IC<iestream_input_defs<int>::out, webAppIN>("input_reader","webApp")
    };
    shared_ptr<cadmium::dynamic::modeling::coupled<TIME>> TOP;
    TOP = make_shared<dynamic::modeling::coupled<TIME>>(
        "TOP", submodels_TOP, iports_TOP, oports_TOP, eics_TOP, eocs_TOP, ics_TOP 
    );

    /*************** Loggers *******************/
    static ofstream out_messages("../simulation_results/webApp_output_messages.txt");
    struct oss_sink_messages{
        static ostream& sink(){          
            return out_messages;
        }
    };
    static ofstream out_state("../simulation_results/webApp_output_state.txt");
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
     r.run_until(NDTime("00:10:00:000"));
    return 0;
}