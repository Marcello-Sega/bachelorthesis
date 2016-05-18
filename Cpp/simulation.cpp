#include "datastructure.h"
#include <fstream>
#include <getopt.h>
using namespace std;

void print_usage() {
    std::cout << "Usage:\t-a \t Amount of particles \n\t\
-t \t Amount of timesteps\n\t\
-r \t Neighborlist refresh rate\n\t\
-d \t Stepsize\n\t\
-T \t Temperature\n\t\
-m \t Mass\n\t\
-D \t Density\n\t\
-c \t Cutoff-radius\n\t\
-g \t Whitenoise factor\n\t\
-s \t Seed\n\t\
-o \t Number of threads (optional)\n\t\
-f \t Filename for positions (optional)" << endl;
}

int main(int argc, char *argv[]){

  int opt          = 0;
  int amount       = -1;
  int refresh      = 1;
  int timesteps    = 1;
  int num_threads  = 1;
  double dt        = 0.001;
  double temp      = 1.0;
  double mass      = 1.0;
  double rho       = 0.6;
  double radius    = 0;
  double g         = 0.1; 
  double seed      = 1;
  char* filename_pos;
  bool pos_file = false;

  static struct option long_options[] = {
        {"amount",   	required_argument, NULL,  'a' },
        {"timesteps", 	required_argument, NULL,  't' },
        {"refresh",   	required_argument, NULL,  'r' },
        {"dt",   	required_argument, NULL,  'd' },
        {"T",   	required_argument, NULL,  'T' },
        {"m",   	required_argument, NULL,  'm' },
        {"rho",   	required_argument, NULL,  'D' },
        {"r",   	required_argument, NULL,  'c' },
        {"g",   	required_argument, NULL,  'g' },
        {"seed",   	required_argument, NULL,  's' },
        {"threads", 	optional_argument, NULL,  'o' },
        {"file",   	optional_argument, NULL,  'f' },   
    };

  int long_index =0;
  while ((opt = getopt_long(argc, argv,"a:t:r:d:T:m:D:c:g:s:o:f:M:", 
                   long_options, &long_index )) != -1) {
    switch (opt) {
      case 'a' : amount = atoi(optarg);
        break;
      case 't' : timesteps = atoi(optarg);
        break;
      case 'r' : refresh = atoi(optarg);
        break;
      case 'd' : dt = atof(optarg);
        break;
      case 'T' : temp = atof(optarg);
        break;
      case 'm' : mass = atof(optarg);
        break;
      case 'D' : rho = atof(optarg);
        break;
      case 'c' : radius = atof(optarg);
        break;
      case 'g' : g = atof(optarg);
        break;
      case 's' : seed = atof(optarg);
        break;
      case 'f' : filename_pos = optarg;
        pos_file = true;
        break;
      case 'o' : num_threads = atoi(optarg);
        break;
      default: print_usage(); 
        exit(EXIT_FAILURE);
      }
    }
    if (amount < 0) {
      print_usage();
      exit(EXIT_FAILURE);
    }
  //////////
  // MAIN //
  //////////

  // Init
  System system(amount,refresh,timesteps,num_threads,dt,temp,mass,rho,radius,g,seed);
  system.printout();
  const char filename[] = "initial_positions.xyz";
  double pot, kin, e0, avk=0, avk2=0;
  Vec momentum;

  system.update_neighborlist();
  system.update_force();

  // Initial energy
  e0 = system.potential_energy() + system.kinetic_energy();

  for (int i=1; i<=timesteps; ++i){
    momentum = system.total_momentum();
    // Check momentum conservation
    if (0){
      std::cout << momentum.vec[0] << "\t" << momentum.vec[1] << "\t" << momentum.vec[2] << std::endl;
    }
    // Write positions to std::cout
    if (0){
      std::cout << amount << endl;
      std::cout << "Simualation" << endl;
      for (int j=0; j<amount; ++j){
        std::cout << "Ar" << "\t" << system.particles.particlelist[j].position.vec[0] << "\t" << system.particles.particlelist[j].position.vec[1] << "\t" << system.particles.particlelist[j].position.vec[2] << std::endl;
      }
    }
    system.update_neighborlist();
    system.update_velocity();
    system.update_position();
    system.update_force();
    system.update_velocity();
    std::cout << "----" << std::endl;
    system.dump_force();
    std::cout << "----" << std::endl;
    pot = system.potential_energy();
    kin = system.kinetic_energy();
    avk += kin;
    avk2 += kin*kin;

    // Energyshift
    if (1){
      std::cout << i << "\t" << (kin+pot-e0)/e0 << std::endl;
    }
    // Mean kinetic energy
    if (0){
      std::cout << i << "\t" << avk/i << "\t" << sqrt(avk2/i-(avk/i)*(avk/i))/sqrt(i) << std::endl;
    }
    // Total energy
    if (0){
      std::cout << i << "\t" << kin+pot << std::endl;
    }
  }
  return 0;
}