/*
 *  AnimStateMachine.h
 *  luciolinae_ctl
 *
 *  Created by damian on 19/05/10.
 *  Copyright 2010 frey damian@frey.co.nz. All rights reserved.
 *
 */

#pragma once

#include "Animation.h"
class StateAnimation;
class Light;

class AnimStateMachine: public Animation
{
public:
	AnimStateMachine( Lights* _lights );
	
	static const char* NAME;

	void update( float elapsed );
	void draw();

private:
	
	// transition management
	void addTransition( string from, string to, float prob );
	void loadTransitions();
	
	void nextState();
	void enterState( string next_state );

	StateAnimation* current_state;
	
	typedef struct StateTransitionProbability
	{
		string next_state;
		float probability;
		StateTransitionProbability( string s, float p ) { next_state = s; probability = p; }
		StateTransitionProbability() { next_state = "idle"; probability = 1; }
	} StateTransitionProbability;
	typedef map< string, vector<StateTransitionProbability> > Transitions;
	Transitions transitions;

	

};