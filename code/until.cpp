int until(std::vector<beacls::FloatVec> datas, beacls::FloatVec alpha, 
  beacls::FloatVec beta, FLOAT_TYPE tau1, FLOAT_TYPE tau2, 
  helperOC::DynSysSchemeData* schemeData, beacls::FloatVec tau, 
  helperOC::HJIPDE_extraArgs extraArgs){

const FLOAT_TYPE small = 1e-3;
const size_t numel = schemeData->get_grid()->get_numel();

  std::vector<beacls::FloatVec> targets(tau.size());
  std::vector<beacls::FloatVec> obstacles(1);
  obstacles[0].assign(numel, 1.);
  std::copy(alpha.cbegin(), alpha.cend(), obstacles[0].begin());

  for (size_t i = 0; i < tau.size(); ++i) {
    targets[i].assign(numel, 1.);
    if (tau[i] > tau1 - small && tau[i] < tau2 + small) { 
      // satisfy beta if tau1 < tau < tau2
     std::copy(beta.begin(), beta.end(), targets[i].begin());
    }

    // obstacles[i].assign(numel, 1.);
    // if (true) {//(tau[i] > tau2) { // satisfy alpha if tau < tau1
    //   std::copy(alpha.cbegin(), alpha.cend(), obstacles[i].begin());
    // }
  }

  helperOC::HJIPDE_extraOuts extraOuts;

  extraArgs.targets = targets;
  extraArgs.obstacles = obstacles;

  helperOC::HJIPDE* hjipde;
  hjipde = new helperOC::HJIPDE();

  beacls::FloatVec tau_out;

  hjipde->solve(datas, tau_out, extraOuts, targets, tau, schemeData, 
    helperOC::HJIPDE::MinWithType_None, extraArgs);

  if (hjipde) delete hjipde;
  return 0;
}