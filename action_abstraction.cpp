/* action_abstraction.hpp
 * Richard Gibson, Jun 28, 2013
 *
 * Home of the action_abstraction abstract class and all implementing classes
 *
 * Copyright (C) 2013 by Richard Gibson
 */

/* C / C++ / STL indluces */
#include <assert.h>

/* project_acpc_server includes */
extern "C" {
}

/* Pure CFR includes */
#include "action_abstraction.hpp"

ActionAbstraction::ActionAbstraction( )
{
}

ActionAbstraction::~ActionAbstraction( )
{
}

NullActionAbstraction::NullActionAbstraction( )
{
}

NullActionAbstraction::~NullActionAbstraction( )
{
}

int NullActionAbstraction::get_actions( const Game *game,
					const State &state,
					Action actions
					[ MAX_ABSTRACT_ACTIONS ] ) const
{
  int num_actions = 0;
  bool error = false;
  for( int a = 0; a < NUM_ACTION_TYPES; ++a ) {
    Action action;
    action.type = ( ActionType ) a;
    action.size = 0;
    if( action.type == a_raise ) {
      int32_t min_raise_size;
      int32_t max_raise_size;
      if( raiseIsValid( game, &state, &min_raise_size, &max_raise_size ) ) {
	if( num_actions + ( max_raise_size - min_raise_size + 1 )
	    > MAX_ABSTRACT_ACTIONS ) {
	  error = true;
	  break;
	}
	for( int s = min_raise_size; s <= max_raise_size; ++s ) {
	  actions[ num_actions ] = action;
	  actions[ num_actions ].size = s;
	  ++num_actions;
	}
      }
    } else if( isValidAction( game, &state, 0, &action ) ) {
      /* If you hit this assert, there are too many abstract actions allowed.
       * Either coarsen the betting abstraction or increase MAX_ABSTRACT_ACTIONS
       * in constants.hpp
       */
      if( num_actions >= MAX_ABSTRACT_ACTIONS ) {
	error = true;
	break;
      }
      actions[ num_actions ] = action;
      ++num_actions;
    }
  }

  /* If you hit this assert, there are too many abstract actions allowed.
   * Either coarsen the betting abstraction or increase MAX_ABSTRACT_ACTIONS
   * in constants.hpp
   */
  assert( !error );

  return num_actions;
}

/* 1)	Fold, allin
 */
FaActionAbstraction::FaActionAbstraction( )
{
}

FaActionAbstraction::~FaActionAbstraction( )
{
}

int FaActionAbstraction::get_actions( const Game *game,
					const State &state,
					Action actions
					[ MAX_ABSTRACT_ACTIONS ] ) const
{
  assert( MAX_ABSTRACT_ACTIONS >= 2 );

  int num_actions = 0;
  for( int a = 0; a < NUM_ACTION_TYPES; ++a ) {
    Action action;
    action.type = ( ActionType ) a;
    action.size = 0;
    if( action.type == a_raise ) {
      int32_t min_raise_size;
      int32_t max_raise_size;
      if( raiseIsValid( game, &state, &min_raise_size, &max_raise_size ) ) {
	/* Now add all-in */
	actions[ num_actions ] = action;
	actions[ num_actions ].size = max_raise_size;
	++num_actions;
      }

    } else if( isValidAction( game, &state, 0, &action ) ) {
      /* Fold only */
			if( action.type != a_call ) {
      	actions[ num_actions ] = action;
      	++num_actions;
			}
    }
  }

  return num_actions;
}

/* 2)	Fold, call, pot, allin
 */
FcpaActionAbstraction::FcpaActionAbstraction( )
{
}

FcpaActionAbstraction::~FcpaActionAbstraction( )
{
}

int FcpaActionAbstraction::get_actions( const Game *game,
					const State &state,
					Action actions
					[ MAX_ABSTRACT_ACTIONS ] ) const
{
  assert( MAX_ABSTRACT_ACTIONS >= 4 );

  int num_actions = 0;
  for( int a = 0; a < NUM_ACTION_TYPES; ++a ) {
    Action action;
    action.type = ( ActionType ) a;
    action.size = 0;
    if( action.type == a_raise ) {
      int32_t min_raise_size;
      int32_t max_raise_size;
      if( raiseIsValid( game, &state, &min_raise_size, &max_raise_size ) ) {
	/* Check for pot-size raise being valid.  First, get the pot size. */
	int32_t pot = 0;
	for( int p = 0; p < game->numPlayers; ++p ) {
	  pot += state.spent[ p ];
	}
	/* Add amount needed to call.  This gives the size of a pot-sized raise */
	uint8_t player = currentPlayer( game, &state );
	int amount_to_call = state.maxSpent - state.spent[ player ];
	pot += amount_to_call;
	/* Raise size is total amount of chips committed over all rounds
	 * after making the raise.
	 */
	int pot_raise_size = pot + ( state.spent[ player ] + amount_to_call );
	if( pot_raise_size < max_raise_size ) {
	  actions[ num_actions ] = action;
	  actions[ num_actions ].size = pot_raise_size;
	  ++num_actions;
	}
	/* Now add all-in */
	actions[ num_actions ] = action;
	actions[ num_actions ].size = max_raise_size;
	++num_actions;
      }

    } else if( isValidAction( game, &state, 0, &action ) ) {
      /* Fold and call */
      actions[ num_actions ] = action;
      ++num_actions;
    }
  }

  return num_actions;
}

/* 3)	Fold, call, half pot, allin
 */
FchaActionAbstraction::FchaActionAbstraction( )
{
}

FchaActionAbstraction::~FchaActionAbstraction( )
{
}

int FchaActionAbstraction::get_actions( const Game *game,
					const State &state,
					Action actions
					[ MAX_ABSTRACT_ACTIONS ] ) const
{
  assert( MAX_ABSTRACT_ACTIONS >= 4 );

  int num_actions = 0;
  for( int a = 0; a < NUM_ACTION_TYPES; ++a ) {
    Action action;
    action.type = ( ActionType ) a;
    action.size = 0;
    if( action.type == a_raise ) {
      int32_t min_raise_size;
      int32_t max_raise_size;
      if( raiseIsValid( game, &state, &min_raise_size, &max_raise_size ) ) {
	/* Check for pot-size raise being valid.  First, get the pot size. */
	int32_t pot = 0;
	for( int p = 0; p < game->numPlayers; ++p ) {
	  pot += state.spent[ p ];
	}
	/* Add amount needed to call.  This gives the size of a pot-sized raise */
	uint8_t player = currentPlayer( game, &state );
	int amount_to_call = state.maxSpent - state.spent[ player ];
	pot += amount_to_call;
	/* Raise size is total amount of chips committed over all rounds
	 * after making the raise.
	 */
	int pot_raise_size = pot + ( state.spent[ player ] + amount_to_call );
	double half_pot = pot_raise_size * 0.5;
	if( half_pot < max_raise_size ) {
	  actions[ num_actions ] = action;
	  actions[ num_actions ].size = half_pot;
	  ++num_actions;
	}
	/* Now add all-in */
	actions[ num_actions ] = action;
	actions[ num_actions ].size = max_raise_size;
	++num_actions;
      }

    } else if( isValidAction( game, &state, 0, &action ) ) {
      /* Fold and call */
      actions[ num_actions ] = action;
      ++num_actions;
    }
  }

  return num_actions;
}

/* 4)	Fold, call, minimum, half pot, pot, allin
 */
FcmhpaActionAbstraction::FcmhpaActionAbstraction( )
{
}

FcmhpaActionAbstraction::~FcmhpaActionAbstraction( )
{
}

int FcmhpaActionAbstraction::get_actions( const Game *game,
					const State &state,
					Action actions
					[ MAX_ABSTRACT_ACTIONS ] ) const
{
  assert( MAX_ABSTRACT_ACTIONS >= 6 );

  int num_actions = 0;
  for( int a = 0; a < NUM_ACTION_TYPES; ++a ) {
    Action action;
    action.type = ( ActionType ) a;
    action.size = 0;
    if( action.type == a_raise ) {
      int32_t min_raise_size;
      int32_t max_raise_size;
      if( raiseIsValid( game, &state, &min_raise_size, &max_raise_size ) ) {
	if( min_raise_size < max_raise_size ) {
		actions [ num_actions ] = action;
		actions [ num_actions ].size = min_raise_size;
		++num_actions;
	}
	/* Check for pot-size raise being valid.  First, get the pot size. */
	int32_t pot = 0;
	for( int p = 0; p < game->numPlayers; ++p ) {
	  pot += state.spent[ p ];
	}
	/* Add amount needed to call.  This gives the size of a pot-sized raise */
	uint8_t player = currentPlayer( game, &state );
	int amount_to_call = state.maxSpent - state.spent[ player ];
	pot += amount_to_call;
	/* Raise size is total amount of chips committed over all rounds
	 * after making the raise.
	 */
	int pot_raise_size = pot + ( state.spent[ player ] + amount_to_call );
	double half_pot = pot_raise_size * 0.5;
	if( half_pot < max_raise_size ) {
	  actions[ num_actions ] = action;
	  actions[ num_actions ].size = half_pot;
	  ++num_actions;
	}
	if( pot_raise_size < max_raise_size ) {
	  actions[ num_actions ] = action;
	  actions[ num_actions ].size = pot_raise_size;
	  ++num_actions;
	}
	/* Now add all-in */
	actions[ num_actions ] = action;
	actions[ num_actions ].size = max_raise_size;
	++num_actions;
      }

    } else if( isValidAction( game, &state, 0, &action ) ) {
      /* Fold and call */
      actions[ num_actions ] = action;
      ++num_actions;
    }
  }

  return num_actions;
}

/* 5)	Fold, call, minimum, one-quarter pot, half pot, three-quarters pot, pot, allin
 */
FcmqhtpaActionAbstraction::FcmqhtpaActionAbstraction( )
{
}

FcmqhtpaActionAbstraction::~FcmqhtpaActionAbstraction( )
{
}

int FcmqhtpaActionAbstraction::get_actions( const Game *game,
					const State &state,
					Action actions
					[ MAX_ABSTRACT_ACTIONS ] ) const
{
  assert( MAX_ABSTRACT_ACTIONS >= 8 );

  int num_actions = 0;
  for( int a = 0; a < NUM_ACTION_TYPES; ++a ) {
    Action action;
    action.type = ( ActionType ) a;
    action.size = 0;
    if( action.type == a_raise ) {
      int32_t min_raise_size;
      int32_t max_raise_size;
      if( raiseIsValid( game, &state, &min_raise_size, &max_raise_size ) ) {
	if( min_raise_size < max_raise_size ) {
		actions [ num_actions ] = action;
		actions [ num_actions ].size = min_raise_size;
		++num_actions;
	}
	/* Check for pot-size raise being valid.  First, get the pot size. */
	int32_t pot = 0;
	for( int p = 0; p < game->numPlayers; ++p ) {
	  pot += state.spent[ p ];
	}
	/* Add amount needed to call.  This gives the size of a pot-sized raise */
	uint8_t player = currentPlayer( game, &state );
	int amount_to_call = state.maxSpent - state.spent[ player ];
	pot += amount_to_call;
	/* Raise size is total amount of chips committed over all rounds
	 * after making the raise.
	 */
	int pot_raise_size = pot + ( state.spent[ player ] + amount_to_call );
	double half_pot = pot_raise_size * 0.5;
	double quarter_pot = pot_raise_size * 0.25;
	double three_quarters_pot = pot_raise_size * 0.75;
	if( quarter_pot < max_raise_size ) {
	  actions[ num_actions ] = action;
	  actions[ num_actions ].size = quarter_pot;
	  ++num_actions;
	}
	if( half_pot < max_raise_size ) {
	  actions[ num_actions ] = action;
	  actions[ num_actions ].size = half_pot;
	  ++num_actions;
	}
	if( three_quarters_pot < max_raise_size ) {
	  actions[ num_actions ] = action;
	  actions[ num_actions ].size = three_quarters_pot;
	  ++num_actions;
	}
	if( pot_raise_size < max_raise_size ) {
	  actions[ num_actions ] = action;
	  actions[ num_actions ].size = pot_raise_size;
	  ++num_actions;
	}
	/* Now add all-in */
	actions[ num_actions ] = action;
	actions[ num_actions ].size = max_raise_size;
	++num_actions;
      }

    } else if( isValidAction( game, &state, 0, &action ) ) {
      /* Fold and call */
      actions[ num_actions ] = action;
      ++num_actions;
    }
  }

  return num_actions;
}
