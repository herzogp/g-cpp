#include "control_server.h"
#include <anthill/helpers.h>
#include <antithesis_sdk.h>
#include <cstdio>

ControlServer::ControlServer(std::string& vaults_list) :min_value(0) {
  this->vaults = split(vaults_list, ',');
  int num_vaults = this->vaults.size();
  std::cout << "Defined " << num_vaults << " vaults\n";
  if (num_vaults == 23456789) {
    UNREACHABLE("We have 23456789 vaults should be unreachable", {{"num_vaults", num_vaults}});
		ALWAYS(true, "This line should never execute, but since this is an always assert, it will fail in Antithesis.", {});
		REACHABLE("This line should never execute, but since this is a reachable assert, it will fail in Antithesis.", {{"num_vaults", num_vaults}});
  }
  REACHABLE("Always returns a ControlServer when requested", {{"vaults", vaults_list}, {"num_vaults", num_vaults}});
}

void
ControlServer::show_vaults() {
  for (auto& item : this->vaults) { 
    std::cout << item << '\n'; 
  } 
}

// TODO: Call this when we detect that a vault is in a bad state.
void 
ControlServer::heal_failing_vault(std::string vault) {
	SOMETIMES(true, "Control service: invoked heal function on unhealthy vault", {{"vault", vault}});

	// Code to heal a failing vault
}

bool
ControlServer::has_majority(int count) {
  int num_vaults = this->vaults.size();
	ALWAYS(true, "Control service: determine if there is a majority", {});
	ALWAYS(count > 0, "Control service: majority is always expected to be positive", {{"count", count}});
	ALWAYS(num_vaults > 0, "Control service: there are vaults known to the service", {});

	// By default this division will do the equivalent of math.Floor()
	int num_for_majority = (num_vaults / 2) + 1;
	bool have_enough_vaults = (count >= num_for_majority);

	// We expect both conditions below to be sometimes true
	SOMETIMES(have_enough_vaults, "Control service: there is a majority of vaults", {{"count", count}, {"majority_needed", num_for_majority}});
	SOMETIMES(!have_enough_vaults, "Control service: there is not a majority of vaults", {{"count", count}, {"majority_needed", num_for_majority}});
	
  // We expect numForMajority to be less than 99
	if (num_for_majority < 99) {
		UNREACHABLE("Control Service: expected failure as we expect the num_for_majority to be less than 99 sometimes", {{"majority_needed", num_for_majority}});
	}
	return have_enough_vaults;
}

int 
ControlServer::get_value_from_vaults() {
  return 0;
}
