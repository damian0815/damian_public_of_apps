/*
 *  AnimStateMachine.cpp
 *  luciolinae_ctl
 *
 *  Created by damian on 19/05/10.
 *  Copyright 2010 frey damian@frey.co.nz. All rights reserved.
 *
 */

#include "AnimStateMachine.h"

const char* AnimStateMachine::NAME = "StateMachine";

AnimStateMachine::AnimStateMachine( Lights* _lights )
: Animation(_lights)
{
	loadTransitions();
	current_state = STATE_IDLE;
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
	
	transitions[STATE_IDLE].push_back( StateTransitionProbability( STATE_BUILDUP, 1 ) );
	transitions[STATE_BUILDUP].push_back( StateTransitionProbability( STATE_BLIP, 1 ) );
	transitions[STATE_BUILDUP].push_back( StateTransitionProbability( STATE_RELEASE, 0.3 ) );
	transitions[STATE_BLIP ].push_back( StateTransitionProbability( STATE_IDLE, 1 ) );
	transitions[STATE_BLIP ].push_back( StateTransitionProbability( STATE_BLIP, 0.1 ) );
	
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
}

void AnimStateMachine::update( float elapsed )
{
	bool go_to_next = updateState( elapsed );

	if ( go_to_next )
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
	vector<StateTransitionProbability> transition_chances;
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

void AnimStateMachine::enterState( State new_state )
{
	// enter the new state
	current_state = new_state;
	
	switch ( current_state )
	{
		case STATE_IDLE:
			// set up a timer
			timer = getSquaredRandom( 2, 8 );
			break;
			
		case STATE_BLIP:
		{
			// blip
			int which_to_blip = lights->getBigLightIndex( ofRandom( 0, 0.9999f*lights->getNumBigLights() ) );
			lights->pulse( which_to_blip, 1, 0 );
			// pause for a little bit
			timer = getSquaredRandom( 1.5f, 0.3f );
			break;
		}
			
		case STATE_BUILDUP:
		case STATE_RELEASE:
			timer = getSquaredRandom( 1.5f, 0.3f );
			break;
	};
}

bool AnimStateMachine::updateState( float elapsed )
{
	switch (current_state)
	{
		case STATE_IDLE:
			timer -= elapsed;
			if ( timer < 0 )
				return true;
			break;
			
		case STATE_BLIP:
			timer -= elapsed;
			if ( timer < 0 )
				return true;
			break;
			
		case STATE_BUILDUP:
			timer -= elapsed;
			if ( timer < 0 )
				return true;
			break;
			
	}
	
	
	return false;
}



float AnimStateMachine::getSquaredRandom( float first, float second )
{
	// get the random pct
	float r = ofRandomuf();
	r*=r;
	
	// so then
	return first + r*(second-first);
	
}
