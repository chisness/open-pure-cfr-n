/* card_abstraction.cpp
 * Richard Gibson, Jun 28, 2013
 *
 * Home of the card_abstraction abstract class and all implementing classes
 *
 * Copyright (C) 2013 by Richard Gibson
 */

/* C / C++ / STL indluces */

/* project_acpc_server includes */
extern "C" {
  #include "hand_index.h"
}

/* Pure CFR includes */
#include "card_abstraction.hpp"
#include "stdbool.h"

CardAbstraction::CardAbstraction( )
{
}

CardAbstraction::~CardAbstraction( )
{
}

/* By default, assume cannot precompute buckets */
void CardAbstraction::precompute_buckets( const Game *game, hand_t &hand ) const
{
  fprintf( stderr, "precompute_buckets called for base "
	   "card abstraction class!\n" );
  assert( false );
}

NullCardAbstraction::NullCardAbstraction( const Game *game )
  : deck_size( game->numSuits * game->numRanks )
{
  /* Precompute number of buckets per round */
  m_num_buckets[ 0 ] = 1;
  for( int i = 0; i < game->numHoleCards; ++i ) {
    m_num_buckets[ 0 ] *= deck_size;
  }
  for( int r = 0; r < MAX_ROUNDS; ++r ) {
    if( r < game->numRounds ) {
      if( r > 0 ) {
	m_num_buckets[ r ] = m_num_buckets[ r - 1 ];
      }
      for( int i = 0; i < game->numBoardCards[ r ]; ++i ) {
	m_num_buckets[ r ] *= deck_size;
      }
    } else {
      m_num_buckets[ r ] = 0;
    }
  }
}

NullCardAbstraction::~NullCardAbstraction( )
{
}

int NullCardAbstraction::num_buckets( const Game *game,
				      const BettingNode *node ) const
{
  return m_num_buckets[ node->get_round() ];
}

int NullCardAbstraction::num_buckets( const Game *game,
				      const State &state ) const
{
  return m_num_buckets[ state.round ];
}

int NullCardAbstraction::get_bucket( const Game *game,
				     const BettingNode *node,
				     const uint8_t board_cards[ MAX_BOARD_CARDS ],
				     const uint8_t hole_cards
				     [ MAX_PURE_CFR_PLAYERS ]
				     [ MAX_HOLE_CARDS ] ) const
{
  return get_bucket_internal( game, board_cards, hole_cards,
			      node->get_player(), node->get_round() );
}

void NullCardAbstraction::precompute_buckets( const Game *game,
					      hand_t &hand ) const
{
  for( int p = 0; p < game->numPlayers; ++p ) {
    for( int r = 0; r < game->numRounds; ++r ) {
      hand.precomputed_buckets[ p ][ r ] = get_bucket_internal( game,
								hand.board_cards,
								hand.hole_cards,
								p, r );
    }
  }
}

int NullCardAbstraction::get_bucket_internal( const Game *game,
					      const uint8_t board_cards
					      [ MAX_BOARD_CARDS ],
					      const uint8_t hole_cards
					      [ MAX_PURE_CFR_PLAYERS ]
					      [ MAX_HOLE_CARDS ],
					      const int player,
					      const int round ) const
{
  /* Calculate the unique bucket number for this hand */
  int bucket = 0;
  for( int i = 0; i < game->numHoleCards; ++i ) {
    if( i > 0 ) {
      bucket *= deck_size;
    }
    uint8_t card = hole_cards[ player ][ i ];
    bucket += rankOfCard( card ) * game->numSuits + suitOfCard( card );
  }
  for( int r = 0; r <= round; ++r ) {
    for( int i = bcStart( game, r ); i < sumBoardCards( game, r ); ++i ) {
      bucket *= deck_size;
      uint8_t card = board_cards[ i ];
      bucket += rankOfCard( card ) * game->numSuits + suitOfCard( card );
    }
  }

  return bucket;
}

SuitCardAbstraction::SuitCardAbstraction( const Game *game )
  : deck_size( game->numSuits * game->numRanks )
{
  //initialize hand indexer
  hand_indexer_t * idxr = new hand_indexer_t;
  uint8_t gt[] = {2,3};
  hand_indexer_init(2, gt, idxr);



  /* Precompute number of buckets per round */
  //m_num_buckets[ 0 ] = (game->numRanks * (game->numRanks - 1)) + game->numRanks
  //suited + unsuited + pairs (royal holdem = 25, regular holdem = 169)
  /*for( int i = 0; i < game->numHoleCards; ++i ) {
    m_num_buckets[ 0 ] *= deck_size;
  }*/
  /*for( int r = 0; r < MAX_ROUNDS; ++r ) {
    if( r < game->numRounds ) {
      if( r > 0 ) {
	m_num_buckets[ r ] = m_num_buckets[ r - 1 ];
      }
      for( int i = 0; i < game->numBoardCards[ r ]; ++i ) {
	m_num_buckets[ r ] *= deck_size;
      }
    } else {
      m_num_buckets[ r ] = 0;
    }
  }*/
}

SuitCardAbstraction::~SuitCardAbstraction( )
{
}

int SuitCardAbstraction::num_buckets( const Game *game,
				      const BettingNode *node ) const
{
  //return m_num_buckets[ node->get_round() ];
  //return idxr->round_size[node->get_round()];
  return hand_indexer_size(idxr, node->get_round());
}

int SuitCardAbstraction::num_buckets( const Game *game,
				      const State &state ) const
{
  //return m_num_buckets[ state.round ];
  //return idxr->round_size[state.round];
  return hand_indexer_size(idxr, state.round);
}

int SuitCardAbstraction::get_bucket( const Game *game,
				     const BettingNode *node,
				     const uint8_t board_cards[ MAX_BOARD_CARDS ],
				     const uint8_t hole_cards
				     [ MAX_PURE_CFR_PLAYERS ]
				     [ MAX_HOLE_CARDS ] ) const
{
  return get_bucket_internal( game, board_cards, hole_cards,
			      node->get_player(), node->get_round() );
}

void SuitCardAbstraction::precompute_buckets( const Game *game,
					      hand_t &hand ) const
{
  for( int p = 0; p < game->numPlayers; ++p ) {
    for( int r = 0; r < game->numRounds; ++r ) {
      hand.precomputed_buckets[ p ][ r ] = get_bucket_internal( game,
								hand.board_cards,
								hand.hole_cards,
								p, r );
    }
  }
}

int SuitCardAbstraction::get_bucket_internal( const Game *game,
					      const uint8_t board_cards
					      [ MAX_BOARD_CARDS ],
					      const uint8_t hole_cards
					      [ MAX_PURE_CFR_PLAYERS ]
					      [ MAX_HOLE_CARDS ],
					      const int player,
					      const int round ) const
{
  hand_index_t idcs[2] = new hand_index_t[2];
  uint8_t holec[2] = new uint8_t[2];
  if (player == 0) {
    holec[0] = hole_cards[0][0];
    holec[1] = hole_cards[0][1];
  }
  else {
    holec[0] = hole_cards[1][0];
    holec[1] = hole_cards[1][1];
  }
  uint8_t all_cards[] = holec.insert(holec.end(), board_cards.begin(), board_cards.end());
  hand_index_all(idxr, all_cards, idcs);
  if (round == 0)
    return idcs[0];
  else
    return idcs[1];
  //use function hand_index_last

  /* Calculate the unique bucket number for this hand
  int bucket = 0;
  for( int i = 0; i < game->numHoleCards; ++i ) {
    if( i > 0 ) {
      bucket *= deck_size;
    }
    uint8_t card = hole_cards[ player ][ i ];
    bucket += rankOfCard( card ) * game->numSuits + suitOfCard( card );
  }
  for( int r = 0; r <= round; ++r ) {
    for( int i = bcStart( game, r ); i < sumBoardCards( game, r ); ++i ) {
      bucket *= deck_size;
      uint8_t card = board_cards[ i ];
      bucket += rankOfCard( card ) * game->numSuits + suitOfCard( card );
    }
  }

  return bucket;*/
}


BlindCardAbstraction::BlindCardAbstraction( )
{
}

BlindCardAbstraction::~BlindCardAbstraction( )
{
}

int BlindCardAbstraction::num_buckets( const Game *game,
				       const BettingNode *node ) const
{
  return 1;
}

int BlindCardAbstraction::num_buckets( const Game *game,
				       const State &state ) const
{
  return 1;
}

int BlindCardAbstraction::get_bucket( const Game *game,
				      const BettingNode *node,
				      const uint8_t board_cards
				      [ MAX_BOARD_CARDS ],
				      const uint8_t hole_cards
				      [ MAX_PURE_CFR_PLAYERS ]
				      [ MAX_HOLE_CARDS ] ) const
{
  return 0;
}

void BlindCardAbstraction::precompute_buckets( const Game *game, hand_t &hand ) const
{
  for( int p = 0; p < game->numPlayers; ++p ) {
    for( int r = 0; r < game->numRounds; ++r ) {
      hand.precomputed_buckets[ p ][ r ] = 0;
    }
  }
}
