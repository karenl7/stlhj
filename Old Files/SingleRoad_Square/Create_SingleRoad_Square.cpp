#define _USE_MATH_DEFINES
#include <levelset/levelset.hpp>
#include <helperOC/helperOC.hpp>
#include <helperOC/DynSys/DynSys/DynSysSchemeData.hpp>
#include <helperOC/DynSys/Plane/Plane.hpp>
#include <helperOC/DynSys/Plane4D/Plane4D.hpp>
#include <cmath>
#include <numeric>
#include <functional>
#include <cfloat>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <cstring>
#include "until.cpp"
#include "eventually.cpp"
#include "always.cpp"
#include "def_extraArgs.cpp"

#include "Square_alpha_beta.cpp"
#include "Road_alpha_beta.cpp"

/**
@brief Tests the Plane class by computing a reachable set and then computing the optimal trajectory from the reachable set.
*/
int main(int argc, char *argv[])
{
	bool dump_file = true;
	if (argc >= 2) {
		dump_file = (atoi(argv[1]) == 0) ? false : true;
	}
<<<<<<< HEAD:code/Old Files/SingleRoad_Square/Create_SingleRoad_Square.cpp
	bool useTempFile = false;
	if (argc >= 3) {
		useTempFile = (atoi(argv[2]) == 0) ? false : true;
	}
	const bool keepLast = false;
	const bool calculateTTRduringSolving = false;
	levelset::DelayedDerivMinMax_Type delayedDerivMinMax =
	  levelset::DelayedDerivMinMax_Disable;
	if (argc >= 4) {
		switch (atoi(argv[3])) {
			default:
			case 0:
			delayedDerivMinMax = levelset::DelayedDerivMinMax_Disable;
			break;
			case 1:
			delayedDerivMinMax = levelset::DelayedDerivMinMax_Always;
			break;
			case 2:
			delayedDerivMinMax = levelset::DelayedDerivMinMax_Adaptive;
			break;
		}
	}

	bool useCuda = false;
	if (argc >= 5) {
		useCuda = (atoi(argv[4]) == 0) ? false : true;
	}
	int num_of_threads = 0;
	if (argc >= 6) {
		num_of_threads = atoi(argv[5]);
	}
	int num_of_gpus = 0;
	if (argc >= 7) {
		num_of_gpus = atoi(argv[6]);
	}
	size_t line_length_of_chunk = 1;
	if (argc >= 8) {
		line_length_of_chunk = atoi(argv[7]);
	}

	bool enable_user_defined_dynamics_on_gpu = true;
	if (argc >= 9) {
		enable_user_defined_dynamics_on_gpu = (atoi(argv[8]) == 0) ? false : true;
	}
=======
>>>>>>> origin/master:code/Car_test.cpp

//!< Compute reachable set
	const FLOAT_TYPE tMax = 5;
	const FLOAT_TYPE dt = 0.25;
	beacls::FloatVec tau = generateArithmeticSequence<FLOAT_TYPE>(0., dt, tMax);

//!< Plane parameters
	const FLOAT_TYPE wMax = (FLOAT_TYPE)1;
	const beacls::FloatVec vrange{ (FLOAT_TYPE)10, (FLOAT_TYPE)15 };
	const beacls::FloatVec arange{ (FLOAT_TYPE)0, (FLOAT_TYPE)5 };
	const beacls::FloatVec dMax{ (FLOAT_TYPE)0, (FLOAT_TYPE)0 };

	const FLOAT_TYPE inf = std::numeric_limits<FLOAT_TYPE>::infinity();
  const beacls::IntegerVec pdDim{2};

// Grid Target and obstacle
	bool accel = false;
  const beacls::FloatVec initState{(FLOAT_TYPE)0, (FLOAT_TYPE)25,
			(FLOAT_TYPE)(270 * M_PI / 180), (FLOAT_TYPE)15};

  const beacls::FloatVec
    //gmin{(FLOAT_TYPE)(-8), (FLOAT_TYPE)(-8), (FLOAT_TYPE)0, (FLOAT_TYPE)5};
		gmin{(FLOAT_TYPE)(-4), (FLOAT_TYPE)(-8), (FLOAT_TYPE)0, (FLOAT_TYPE)5};

  const beacls::FloatVec
    gmax{(FLOAT_TYPE)4, (FLOAT_TYPE)8, (FLOAT_TYPE)(2*M_PI),(FLOAT_TYPE)25};

  levelset::HJI_Grid* g;
  helperOC::Plane* p3D = new helperOC::Plane(
  	beacls::FloatVec{initState[0], initState[1], initState[2]},
  	wMax, vrange, dMax);
  helperOC::Plane4D* p4D = new helperOC::Plane4D(initState, wMax, arange, dMax);

	if (accel) {
<<<<<<< HEAD:code/Old Files/SingleRoad_Square/Create_SingleRoad_Square.cpp
  	g = helperOC::createGrid(gmin, gmax,
				beacls::IntegerVec{31,31,21,21}, pdDim);
	}
=======
  	g = helperOC::createGrid(gmin, gmax, 
				beacls::IntegerVec{15,15,15,15}, pdDim);	
	} 
>>>>>>> origin/master:code/Car_test.cpp
	else {
		g = helperOC::createGrid(
			beacls::FloatVec{gmin[0], gmin[1], gmin[2]},
			beacls::FloatVec{gmax[0], gmax[1], gmax[2]}, beacls::IntegerVec{40,80,25},
			pdDim);
		}

		const size_t numel = g->get_numel();
		const size_t num_dim = g->get_num_of_dimensions();

/* Define parameters for the until operator
  // satisfy beta if tau1 < tau < tau2 (reach beta)
  // satisfy alpha until beta is satisfied
*/
// Define tau1 and tau2
		FLOAT_TYPE tau1 = 0.;
		FLOAT_TYPE tau2 = 2.;

// Define alpha and beta
	   beacls::FloatVec alpha, beta;


  	   Road_alpha_beta(alpha,beta,g);

    // Dynamical system parameters
		helperOC::DynSysSchemeData* schemeData = new helperOC::DynSysSchemeData;
<<<<<<< HEAD:code/Old Files/SingleRoad_Square/Create_SingleRoad_Square.cpp
		helperOC::HJIPDE_extraArgs extraArgs;

    // Target set and visualization
		extraArgs.visualize = true;
=======
    schemeData->set_grid(g);
>>>>>>> origin/master:code/Car_test.cpp

		if (accel) {
			schemeData->dynSys = p4D;
<<<<<<< HEAD:code/Old Files/SingleRoad_Square/Create_SingleRoad_Square.cpp
			extraArgs.plotData.plotDims = beacls::IntegerVec{ 1, 1, 0, 0};
			extraArgs.plotData.projpt =
			beacls::FloatVec{p4D->get_x()[2], p4D->get_x()[3]};
		}
		else {
			schemeData->dynSys = p3D;
			extraArgs.plotData.plotDims = beacls::IntegerVec{ 1, 1, 0};
			extraArgs.plotData.projpt = beacls::FloatVec{p3D->get_x()[2]};
		}

		extraArgs.deleteLastPlot = true;
		extraArgs.fig_filename = "figs/Car_test";

		extraArgs.execParameters.line_length_of_chunk = line_length_of_chunk;
		extraArgs.execParameters.calcTTR = calculateTTRduringSolving;
		extraArgs.keepLast = keepLast;
		extraArgs.execParameters.useCuda = useCuda;
		extraArgs.execParameters.num_of_gpus = num_of_gpus;
		extraArgs.execParameters.num_of_threads = num_of_threads;
		extraArgs.execParameters.delayedDerivMinMax = delayedDerivMinMax;
		extraArgs.execParameters.enable_user_defined_dynamics_on_gpu =
		enable_user_defined_dynamics_on_gpu;

		// std::vector<beacls::FloatVec> alpha_U_beta;
		// int resultU = until(alpha_U_beta, alpha, beta, tau1, tau2, schemeData, tau,
		// 	extraArgs);
		//
    // std::vector<beacls::FloatVec> event_beta;
		// int resultF = eventually(event_beta, beta, tau1, tau2, schemeData, tau,
		// 	extraArgs);
		//
    // std::vector<beacls::FloatVec> always_alpha;
		// int resultG = always(always_alpha, alpha, tau1, tau2, schemeData, tau,
		// 	extraArgs);

	  // save mat file
		 std::string Car_test_filename("Car_test.mat");
		 beacls::MatFStream* fs = beacls::openMatFStream(Car_test_filename,
		 beacls::MatOpenMode_Write);

		 if (dump_file) {
		 	beacls::IntegerVec Ns = g->get_Ns();

		 	g->save_grid(std::string("g"), fs);
		 	if (!alpha.empty()) {
		 		save_vector(alpha, std::string("data"), Ns, false, fs);
		 	}
		 	// if (!tau.empty()) {
		 	// 	save_vector(tau, std::string("tau"), Ns, false, fs);
			// }
		}

	  	beacls::closeMatFStream(fs);
=======
		} 
		else {
			schemeData->dynSys = p3D;
		}

    helperOC::HJIPDE_extraArgs extraArgs = 
      def_extraArgs(accel, schemeData->dynSys);

		std::vector<beacls::FloatVec> alpha_U_beta;
		int resultU = until(alpha_U_beta, alpha, beta, tau1, tau2, schemeData, tau, 
			extraArgs);

    std::vector<beacls::FloatVec> event_beta;
		int resultF = eventually(event_beta, beta, tau1, tau2, schemeData, tau, 
			extraArgs);

    std::vector<beacls::FloatVec> always_alpha;
		int resultG = always(always_alpha, alpha, tau1, tau2, schemeData, tau, 
			extraArgs);				

	  // save mat file


		if (dump_file) {
			std::string Car_test_filename("Car_test.mat");
			beacls::MatFStream* fs = beacls::openMatFStream(Car_test_filename, 
				beacls::MatOpenMode_Write);			
			beacls::IntegerVec Ns = g->get_Ns();

			g->save_grid(std::string("g"), fs);
			if (!alpha_U_beta.empty()) {
				save_vector_of_vectors(alpha_U_beta, std::string("alpha_U_beta"), Ns, 
					false, fs);
			}

			if (!event_beta.empty()) {
				save_vector_of_vectors(event_beta, std::string("event_beta"), Ns, 
					false, fs);
			}

			if (!always_alpha.empty()) {
				save_vector_of_vectors(always_alpha, std::string("always_alpha"), Ns, 
					false, fs);
			}

			beacls::closeMatFStream(fs);
		}

	  
>>>>>>> origin/master:code/Car_test.cpp

		if (schemeData) delete schemeData;
		if (p3D) delete p3D;
		if (p4D) delete p4D;
		if (g) delete g;
		return 0;
	}
