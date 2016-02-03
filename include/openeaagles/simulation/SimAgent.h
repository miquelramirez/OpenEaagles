//------------------------------------------------------------------------------
// Class: SimAgent
//------------------------------------------------------------------------------
#ifndef __oe_simulation_SimAgent_H__
#define __oe_simulation_SimAgent_H__

#include "openeaagles/basic/ubf/Agent.h"

namespace oe {
namespace simulation {

class Simulation;
class Station;

//------------------------------------------------------------------------------
// Class: SimAgent
//
// Description: Generic agent class to control a component in the simulation - the agent's "actor"
//    newUbf version allows component behaviors to return null actions.
//    newUbf actions know how to execute themselves, so agent does not need to know anything about action class.
//    newUbf agent's state is initialized by slot, so agent does not need to know anything about state class.
//
// Factory name: SimAgent
// Slots:
//    actorPlayerName      <String>    ! The agent's actor - playerName
//    actorComponentName   <String>    ! The agent's actor - componentName
//------------------------------------------------------------------------------
class SimAgent : public basic::Ubf::Agent
{
   DECLARE_SUBCLASS(SimAgent, basic::Ubf::Agent)
public:
   SimAgent();

protected:

   void initActor() override;

   Station*     getStation();
   Simulation*  getSimulation();

   // sets which component ("actor") the agent is for
   void setActorPlayerByName(const char*);
   void setActorComponentByName(const char*);

   // slot functions
   // slot to set agent's actor (component to be controlled)
   bool setSlotActorPlayerName(const basic::String* const);
   bool setSlotActorComponentName(const basic::String* const);

private:
   const basic::String*    actorPlayerName;
   const basic::String*    actorComponentName;
   Station* myStation;
};


// A SimAgent can be configured as shown below:  (assuming the existence of the "abc" state and behavior classes)
//( Station
//
//   components: {
//      simpleSimAgent: ( SimAgent
//         actorPlayerName: p01
//         state: ( abcState )
//         behavior: ( abcBehavior1 )
//      )
//   } // components
//
//   simulation: ( Simulation
//
//      players: {
//
//         p01: ()
//
//         p02: ()
//      } // players
//   ) // simulation
//) // Station

} // End simulation namespace
} // End oe namespace

#endif

