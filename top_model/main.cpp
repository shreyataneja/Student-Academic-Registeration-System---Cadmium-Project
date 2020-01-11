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
#include "../atomics/faculty.hpp"
#include "../atomics/Registrar.hpp"
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
struct studentsRegistering : public in_port<int>{};
struct studentApplicationIN   : public in_port<int>{};
struct webAppIN  : public in_port<int>{};


/***** Define output ports for coupled model *****/
struct facultyRejected : public out_port<int>{};
struct standardsNotMet : public out_port<int>{};
struct rejectedApplicationID : public out_port<int>{};
struct advisorRejectedApplicationID : public out_port<int>{};
struct webAppOUT : public out_port<int>{};


/****** Input Reader atomic model declaration *******************/
template<typename T>
class InputReader_Int : public iestream_input<int,T> {
public:
    InputReader_Int() = default;
    InputReader_Int(const char* file_path) : iestream_input<int,T>(file_path) {}
};

int main(int argc, char ** argv) {

    if (argc < 2) {
        cout << "Program used with wrong parameters. The program must be invoked as follow:";
        cout << argv[0] << " path to the input file " << endl;
        return 1; 
    }
    /****** Input Reader atomic model instantiation *******************/
    string input = argv[1];
    const char * i_input = input.c_str();
    shared_ptr<dynamic::modeling::model> input_reader = dynamic::translate::make_dynamic_atomic_model<InputReader_Int, TIME, const char* >("input_reader" , move(i_input));

    /****** WebApp atomic models instantiation *******************/
        /****** Application atomic model instantiation *******************/
    shared_ptr<dynamic::modeling::model> application1 = dynamic::translate::make_dynamic_atomic_model<Application, TIME>("application1");

    /****** ApplicationProcess atomic model instantiation *******************/
    shared_ptr<dynamic::modeling::model> applicationProcess1 = dynamic::translate::make_dynamic_atomic_model<ApplicationProcess, TIME>("applicationProcess1");

	
    /****** University atomic models instantiation *******************/
    /****** Faculty atomic model instantiation *******************/
    shared_ptr<dynamic::modeling::model> faculty1 = dynamic::translate::make_dynamic_atomic_model<Faculty, TIME>("faculty1");

    /****** Registrar atomic model instantiation *******************/
    shared_ptr<dynamic::modeling::model> registrar1 =dynamic::translate::make_dynamic_atomic_model<Registrar, TIME>("registrar1");


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
    shared_ptr<dynamic::modeling::coupled<TIME>> WEBAPP;
    WEBAPP = make_shared<dynamic::modeling::coupled<TIME>>(
        "webApp", submodels_webApp, iports_webApp, oports_webApp, eics_webApp, eocs_webApp, ics_webApp 
    );


    /*******University SIMULATOR COUPLED MODEL********/
	
   dynamic::modeling::Ports iports_University = {typeid(studentApplicationIN)};
    dynamic::modeling::Ports oports_University = {typeid(rejectedApplicationID),typeid(advisorRejectedApplicationID)};
    dynamic::modeling::Models submodels_University = {faculty1, registrar1};
    dynamic::modeling::EICs eics_University = {
        cadmium::dynamic::translate::make_EIC<studentApplicationIN, Registrar_defs::applicationVerificationIN>("registrar1")
    };
    dynamic::modeling::EOCs eocs_University = {
        dynamic::translate::make_EOC<Registrar_defs::minStandardsNotMet,rejectedApplicationID>("registrar1"),
        dynamic::translate::make_EOC<Faculty_defs::advisorOpinionOUT,advisorRejectedApplicationID>("faculty1")
    };
    dynamic::modeling::ICs ics_University = {
        dynamic::translate::make_IC< Registrar_defs::applicationVerificationOUT,Faculty_defs::applicationCourseCheck>("registrar1","faculty1")
		};
    shared_ptr<dynamic::modeling::coupled<TIME>> UNIVERSITY;
    UNIVERSITY = make_shared<dynamic::modeling::coupled<TIME>>(
        "university", submodels_University, iports_University, oports_University, eics_University, eocs_University, ics_University 
    );


    /*******SARS SIMULATOR COUPLED MODEL********/
	
    dynamic::modeling::Ports iports_SARS = {typeid(studentsRegistering)};
    dynamic::modeling::Ports oports_SARS = {typeid(facultyRejected),typeid(standardsNotMet)};
    dynamic::modeling::Models submodels_SARS = {WEBAPP, UNIVERSITY};
    dynamic::modeling::EICs eics_SARS = {
        cadmium::dynamic::translate::make_EIC<studentsRegistering, webAppIN>("webApp")
    };
    dynamic::modeling::EOCs eocs_SARS = {
        dynamic::translate::make_EOC<advisorRejectedApplicationID,facultyRejected>("university"),
        dynamic::translate::make_EOC<rejectedApplicationID,standardsNotMet>("university")
    };
    dynamic::modeling::ICs ics_SARS = {
        dynamic::translate::make_IC<webAppOUT, studentApplicationIN>("webApp","university")
  
    };
    shared_ptr<dynamic::modeling::coupled<TIME>> SARS;
    SARS = make_shared<dynamic::modeling::coupled<TIME>>(
        "SARS", submodels_SARS, iports_SARS, oports_SARS, eics_SARS, eocs_SARS, ics_SARS 
    );


    /*******TOP COUPLED MODEL********/
    dynamic::modeling::Ports iports_TOP = {};
    dynamic::modeling::Ports oports_TOP = {typeid(facultyRejected),typeid(standardsNotMet)};
    dynamic::modeling::Models submodels_TOP = {input_reader, SARS};
    dynamic::modeling::EICs eics_TOP = {};
    dynamic::modeling::EOCs eocs_TOP = {
        dynamic::translate::make_EOC<facultyRejected,facultyRejected>("SARS"),
        dynamic::translate::make_EOC<standardsNotMet,standardsNotMet>("SARS")
    };
    dynamic::modeling::ICs ics_TOP = {
        dynamic::translate::make_IC<iestream_input_defs<int>::out, studentsRegistering>("input_reader","SARS")
    };
    shared_ptr<cadmium::dynamic::modeling::coupled<TIME>> TOP;
    TOP = make_shared<dynamic::modeling::coupled<TIME>>(
        "TOP", submodels_TOP, iports_TOP, oports_TOP, eics_TOP, eocs_TOP, ics_TOP 
    );

    /*************** Loggers *******************/
    static ofstream out_messages("../simulation_results/SARS_output_messages.txt");
    struct oss_sink_messages{
        static ostream& sink(){          
            return out_messages;
        }
    };
    static ofstream out_state("../simulation_results/SARS_output_state.txt");
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