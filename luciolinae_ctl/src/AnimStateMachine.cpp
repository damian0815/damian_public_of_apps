/*
 *  AnimStateMachine.cpp
 *  luciolinae_ctl
 *
 *  Created by damian on 19/05/10.
 *  Copyright 2010 frey damian@frey.co.nz. All rights reserved.
 *
 */

#include "AnimStateMachine.h"
#include "StateAnimation.h"
#include "StateAnimIdle.h"
#include "StateAnimBlip.h"
#include "StateAnimStartBlip.h"
#include "StateAnimDelaunayOut.h"
#include "StateAnimGather.h"
#include "StateAnimChunk.h"
#include "StateAnimSweepOnce.h"
#include "StateAnimBlipAll.h"
#include "StateAnimSeq.h"

const char* AnimStateMachine::NAME = "StateMachine";

AnimStateMachine::AnimStateMachine( Lights* _lights )
: Animation(_lights)
{
	loadTransitions();
	enterState( StateAnimIdle::NAME );
}

void AnimStateMachine::addTransition( string from, string to, float prob )
{
	transitions[from].push_back( StateTransitionProbability( to, prob ) );

}

void AnimStateMachine::loadTransitions()
{
/*	ofxXmlSettings& data;
	data.loadFile( "state_transitions.xml" );
	data.pushTag("transitions" );
	int count = data.getNumTags("state");
	for ( int i=0; i<count; i++ )
	{
		data.pushTag( "state", i );
		State state = STATE_IDLE;
		string state_name = data.getValue("name", "" );
		if ( state_name == "idle" )
			state = STATE_IDLE;
		else if ( state_name == "buildup" )
			state = STATE_BUILDUP;
		else if ( state_name == "blip" )
			state = STATE_BLIP;
		data.popTag();
	}
	data.popTag();*/
	
/*	transitions["state_idle"].push_back( StateTransitionProbability( "state_buildup", 1 ) );
	transitions["state_buildup"].push_back( StateTransitionProbability( "state_blip", 1 ) );
	transitions["state_buildup"].push_back( StateTransitionProbability( "state_release", 0.3 ) );
	transitions["state_blip"].push_back( StateTransitionProbability( "state_idle", 1 ) );
	transitions["state_blip"].push_back( StateTransitionProbability( "state_blip", 0.1 ) );*/

#if defined ALWAYS_THE_SAME
	addTransition( StateAnimIdle::NAME,			StateAnimStartBlip::NAME, 1 );
	addTransition( StateAnimStartBlip::NAME,	StateAnimGather::NAME, 0.5f );
	addTransition( StateAnimGather::NAME,		StateAnimBlip::NAME, 1 );
	addTransition( StateAnimBlip::NAME,			StateAnimDelaunayOut::NAME, 1.0f );
	addTransition( StateAnimDelaunayOut::NAME,	StateAnimIdle::NAME, 1 );
#elif defined DARK
	// blip
	addTransition( StateAnimIdle::NAME,			StateAnimStartBlip::NAME, 1 );
	addTransition( StateAnimStartBlip::NAME,	StateAnimBlip::NAME, 0.3f );
	addTransition( StateAnimStartBlip::NAME,	StateAnimGather::NAME, 0.5f );
	addTransition( StateAnimGather::NAME,		StateAnimBlip::NAME, 1 );
	addTransition( StateAnimBlip::NAME,			StateAnimIdle::NAME, 1 );
	addTransition( StateAnimBlip::NAME,			StateAnimBlip::NAME, 0.3f );	
	addTransition( StateAnimBlip::NAME,			StateAnimDelaunayOut::NAME, 1.0f );
	addTransition( StateAnimDelaunayOut::NAME,	StateAnimIdle::NAME, 1 );
	addTransition( StateAnimDelaunayOut::NAME,	StateAnimBlip::NAME, 0.3f );
	
	// chunks
	addTransition( StateAnimIdle::NAME,			StateAnimChunk::NAME, 0.5f );
	addTransition( StateAnimChunk::NAME,		StateAnimStartBlip::NAME, 0.3f );
	addTransition( StateAnimChunk::NAME,		StateAnimIdle::NAME, 0.6f );
#else // light
	addTransition( StateAnimIdle::NAME,		StateAnimBlipAll::NAME, 0.5f );
	addTransition( StateAnimIdle::NAME,		StateAnimSeq::NAME, 0.8f );
	addTransition( StateAnimSweepOnce::NAME,	StateAnimIdle::NAME, 0.1f );
	addTransition( StateAnimSweepOnce::NAME,	StateAnimSweepOnce::NAME, 0.8f );
	addTransition( StateAnimSweepOnce::NAME,	StateAnimBlipAll::NAME, 0.5f );
	addTransition( StateAnimBlipAll::NAME,		StateAnimIdle::NAME, 0.5 );
	addTransition( StateAnimBlipAll::NAME,		StateAnimBlipAll::NAME, 1 );
	//addTransition( StateAnimBlipAll::NAME,		StateAnimSeq::NAME, 0.7f );
	addTransition( StateAnimSeq::NAME, 		StateAnimIdle::NAME, 1 );

#endif

	
	// normalise probabilities
	float total_p = 0;
	for ( Transitions::iterator it = transitions.begin();
		 it != transitions.end();
		 ++it )
	{
		for ( int i=0; i<(*it).second.size(); i++ )
		{
			total_p += (*it).second[i].probability;
		}
		for ( int i=0; i<(*it).second.size(); i++ )
		{
			(*it).second[i].probability /= total_p;
		}
	}
}



void AnimStateMachine::draw()
{
	ofSetColor( 0xff, 0xff, 0xff, 0x80 );
	char buf[1024];
	sprintf(buf, "StateMachine running: bright %7.5f current state '%s'", lights->getSmalLightBrightnessFactor(), 
			current_state->getName().c_str() );
	ofDrawBitmapString( buf, 10, 10 );
	current_state->draw();
}

void AnimStateMachine::update( float elapsed )
{
	current_state->update( elapsed );
	if ( current_state->isFinished() )
		nextState();
}

void AnimStateMachine::nextState()
{
	// go through transition table and enter the next state
	// probabilities are normalised 
	float which = ofRandomuf();
	float so_far = 0;
	// go through all possibiliets, incrementing so_far until we are at which
	// this takes care of probability distribution
	vector<StateTransitionProbability>& transition_chances = transitions[current_state->getName()];
	for ( int i=0; i<transition_chances.size(); i++ )
	{
		so_far += transition_chances[i].probability;
		if ( so_far >= which )
		{
			enterState( transition_chances[i].next_state );
			break;
		}
	}
	
	// done
}

void AnimStateMachine::enterState( string new_state )
{
	current_state = (StateAnimation*)AnimationFactory::makeAnimation( new_state );
	// enter the new state
	current_state->enter();
}



