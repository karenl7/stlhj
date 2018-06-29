#include "theta_diff.cpp"
#include "add_lane.cpp"
#include "get_subvector.cpp"
#include "update_lane.cpp"
#include "add_lane_disturbance_v2.cpp"
#include "add_lane_initial_final.cpp"
#include "add_sq.cpp"
#include "add_lane_plain.cpp"
#include "add_lane_beta_1.cpp"

void fourway_intersection(
  beacls::FloatVec& lane,
  levelset::HJI_Grid* g,
  beacls::FloatVec gmin,
  beacls::FloatVec gmax,
  beacls::FloatVec vrange,
  beacls::FloatVec y2range,
  int Command)
  {

    FLOAT_TYPE square_width;
    FLOAT_TYPE vehicle_width = 0.05; //for car 1
    FLOAT_TYPE vehicle_width2 = 0.138; //for car 2
    FLOAT_TYPE lane_width = 0.4;

    const size_t numel = g->get_numel();
    const size_t num_dim = g->get_num_of_dimensions();
    const std::vector<size_t> shape = g->get_shape();

    beacls::FloatVec lane_temp;
    beacls::FloatVec lane_temp_calc;

    lane.assign(numel, -10.);
    //lane_temp.assign(numel, -20.);
    //lane_temp_calc.assign(numel,0.);
    beacls::FloatVec xs_temp, xs_temp0, xs_temp1, xs_temp2;
    beacls::FloatVec range;
    beacls::FloatVec theta_range;
    FLOAT_TYPE theta_offset;
    FLOAT_TYPE lane_offset;
    FLOAT_TYPE sq_offset;
    FLOAT_TYPE theta_min;
    FLOAT_TYPE theta_max;
    FLOAT_TYPE dim_long_offset;
    FLOAT_TYPE xg, yg, x0, y0, Car2_xmin, Car2_xmax, Car2_ymin, Car2_ymax;
    int position;
    int dim_long; //longitudinal dimension of lane

    xg = -0.4;
    yg = 0.2;
    x0 = 0.2;
    y0 = -0.4;
    position = 0; //1-enhance value function

    for (size_t dim = 0; dim < 5; ++dim) {
      // Assign values for the vertical lanes.
      if (dim == 0 || dim == 1 || dim == 2 || dim == 3 || dim == 4){
        const beacls::FloatVec &xs = g->get_xs(dim);

        if (Command == 0){ //0-alpha; 1-beta
          if (dim != 1){
            theta_offset = M_PI/2.;
            range = {-0.4,0.4,0.,2.}; //{xmin,xmax,ymin,ymax}
            lane_offset = (range[0]+range[1])/2.;
            get_subvector(lane_temp,lane,shape,range,gmin,gmax,dim);
            get_subvector(xs_temp,xs,shape,range,gmin,gmax,dim);
            lane_temp.assign(lane_temp.size(), 2.5);
            if (dim == 3 || dim == 4){
              add_lane(lane_temp,xs_temp,lane_offset,lane_width,theta_offset,vehicle_width,dim,vrange,y2range);
            }
            update_lane(lane_temp,lane,shape,range,gmin,gmax,dim);

            //Car 2 (frame of reference)
            Car2_xmin = 0.2-vehicle_width2/2.;
            Car2_xmax = 0.2+vehicle_width2/2.;
            Car2_ymin = 1.-0.78*vehicle_width2/2.;
            Car2_ymax = 1.+0.78*vehicle_width2/2.;
            range = {Car2_xmin,Car2_xmax,Car2_ymin,Car2_ymax}; //{xmin,xmax,ymin,ymax}
            //lane_offset = (range[3]+range[2])/2.;
            get_subvector(lane_temp,lane,shape,range,gmin,gmax,dim);
            //get_subvector(xs_temp,xs,shape,range,gmin,gmax,dim);
            lane_temp.assign(lane_temp.size(), -10.);
            update_lane(lane_temp,lane,shape,range,gmin,gmax,dim);


          }
        }

        if (Command == 1){ //0-alpha; 1-beta

          theta_offset = M_PI/2;

          dim_long = 1;
          range = {0.,0.4,0.,2.}; //{xmin,xmax,ymin,ymax}
          //dim_long_offset = range[3];
          lane_offset = (range[1]+range[0])/2.;
          get_subvector(lane_temp,lane,shape,range,gmin,gmax,dim);
          get_subvector(xs_temp,xs,shape,range,gmin,gmax,dim);
          //lane_temp.assign(lane_temp.size(), 10.);
          add_lane_beta_1(lane_temp,xs_temp,lane_offset,lane_width,theta_offset,vehicle_width,dim_long,dim_long_offset,dim,vrange,y2range,position);
          update_lane(lane_temp,lane,shape,range,gmin,gmax,dim);

          //Car 2 (frame of reference)
          Car2_xmin = 0.2-vehicle_width2/2.;
          Car2_xmax = 0.2+vehicle_width2/2.;
          Car2_ymin = 1.-0.78*vehicle_width2/2.;
          Car2_ymax = 1.+0.78*vehicle_width2/2.;
          range = {Car2_xmin,Car2_xmax,Car2_ymin,Car2_ymax}; //{xmin,xmax,ymin,ymax}
          //lane_offset = (range[3]+range[2])/2.;
          get_subvector(lane_temp,lane,shape,range,gmin,gmax,dim);
          //get_subvector(xs_temp,xs,shape,range,gmin,gmax,dim);
          lane_temp.assign(lane_temp.size(), -10.);
          update_lane(lane_temp,lane,shape,range,gmin,gmax,dim);

        }
      }

      // Assign values for the horizontal lanes.
      if (dim == 0 || dim == 1 || dim == 2 || dim == 3 || dim == 4){
        const beacls::FloatVec &xs = g->get_xs(dim);

        if (Command == 0){ //0-Green; 1-Red

        }

        if (Command == 1){ //0-Green; 1-Red

        }
      }
    }

    //Account for 2nd Car's position
    if (Command == 0){
      size_t dim = 4;
      range = {0.,2.,0.4,0.8};
      get_subvector(lane_temp,lane,shape,range,gmin,gmax,dim);
      add_lane_disturbance_v2(lane_temp,shape,range,gmin,gmax,vehicle_width2,dim);
      update_lane(lane_temp,lane,shape,range,gmin,gmax,dim);
    }

    // if (Command == 1){
    //   size_t dim = 4;
    //   range = {-0.2,0.,0.,2.};
    //   get_subvector(lane_temp,lane,shape,range,gmin,gmax,dim);
    //   add_lane_disturbance_v2(lane_temp,shape,range,gmin,gmax,vehicle_width2,dim);
    //   update_lane(lane_temp,lane,shape,range,gmin,gmax,dim);
    // }
  }